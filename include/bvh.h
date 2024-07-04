
#ifndef BVH_H
#define BVH_H

#include "shape.h"
#include "vec3.h"
#include <vector>

using namespace std;

struct BVHNode {
  vec3 aabbMin, aabbMax;
  int leftFirst, shapeCount;
  bool isLeaf() { return shapeCount > 0; }
};

struct Bin {
  aabb bounds;
  int shapeCount = 0;
};

class bvh {
public:
  int N;
  vector<shape *> shapes;
  int *shapeIdx;
  BVHNode *bvhNode;
  int rootNodeIdx = 0, nodesUsed = 1;

  bvh() {}
  bvh(vector<shape *> &s) {

    clog << "Constructing BVH...\n";
    shapes = s;
    N = int(shapes.size());
    bvhNode = new BVHNode[N * 2 - 1];
    shapeIdx = new int[N];
    for (int i = 0; i < N; i++) {
      shapeIdx[i] = i;
    }
    BVHNode &root = bvhNode[rootNodeIdx];
    root.leftFirst = 0, root.shapeCount = N;
    UpdateNodeBounds(rootNodeIdx);
    // subdivide recursively
    Subdivide(rootNodeIdx);
  }

  void UpdateNodeBounds(int nodeIdx) {
    BVHNode &node = bvhNode[nodeIdx];
    node.aabbMin = vec3(1e30f, 1e30f, 1e30f);
    node.aabbMax = vec3(-1e30f, -1e30f, -1e30f);
    for (int first = node.leftFirst, i = 0; i < node.shapeCount; i++) {
      int leafShapeIdx = shapeIdx[first + i];
      shape &leafShape = *shapes[first + i];
      node.aabbMin = min(node.aabbMin, leafShape.bb.bbmin);
      node.aabbMax = max(node.aabbMax, leafShape.bb.bbmax);
    }
  }

  void Subdivide(int nodeIdx) {
    BVHNode &node = bvhNode[nodeIdx];

    // determine split axis using SAH
    int axis;
    float splitPos;
    float splitCost = FindBestSplitPlane(node, axis, splitPos);
    float nosplitCost = CalculateNodeCost(node);
    if (splitCost >= nosplitCost)
      return;

    int i = node.leftFirst;
    int j = i + node.shapeCount - 1;
    while (i <= j) {
      if (shapes[i]->centroid.v[axis] < splitPos)
        i++;
      else
        swap(shapes[i], shapes[j--]);
    }

    int leftCount = i - node.leftFirst;
    if (leftCount == 0 || leftCount == node.shapeCount)
      return;
    // create child nodes
    int leftChildIdx = nodesUsed++;
    int rightChildIdx = nodesUsed++;
    bvhNode[leftChildIdx].leftFirst = node.leftFirst;
    bvhNode[leftChildIdx].shapeCount = leftCount;
    bvhNode[rightChildIdx].leftFirst = i;
    bvhNode[rightChildIdx].shapeCount = node.shapeCount - leftCount;
    node.leftFirst = leftChildIdx;
    node.shapeCount = 0;

    UpdateNodeBounds(leftChildIdx);
    UpdateNodeBounds(rightChildIdx);
    // recurse
    Subdivide(leftChildIdx);
    Subdivide(rightChildIdx);
  }

  void IntersectBVH(ray &ray, const int nodeIdx, hit &h) {

    BVHNode *node = &bvhNode[rootNodeIdx], *stack[64];
    int stackPtr = 0;
    while (1) {
      if (node->isLeaf()) {
        hit tempHit;
        for (int i = 0; i < node->shapeCount; i++) {
          if (shapes[shapeIdx[node->leftFirst + i]]->ray_intersect(ray,
                                                                   tempHit)) {
            if (tempHit.t < h.t) {
              h = tempHit;
            }
          }
        }

        if (stackPtr == 0)
          break;
        else
          node = stack[--stackPtr];

        continue;
      }
      BVHNode *child1 = &bvhNode[node->leftFirst];
      BVHNode *child2 = &bvhNode[node->leftFirst + 1];
      aabb bb1(child1->aabbMin, child1->aabbMax);
      float dist1 = bb1.ray_intersect(ray);
      aabb bb2(child2->aabbMin, child2->aabbMax);
      float dist2 = bb2.ray_intersect(ray);
      if (dist1 > dist2) {
        swap(dist1, dist2);
        swap(child1, child2);
      }
      if (dist1 == 1e30f) {
        if (stackPtr == 0)
          break;
        else
          node = stack[--stackPtr];
      } else {
        node = child1;
        if (dist2 != 1e30f)
          stack[stackPtr++] = child2;
      }
    }
  }

  float EvaluateSAH(BVHNode &node, int axis, float pos) {
    // determine triangle counts and bounds for this split candidate
    aabb leftBox, rightBox;
    int leftCount = 0, rightCount = 0;
    for (int i = 0; i < node.shapeCount; i++) {
      shape &shape = *shapes[shapeIdx[node.leftFirst + i]];
      if (shape.centroid.v[axis] < pos) {
        leftCount++;
        leftBox.grow(shape.bb);
      } else {
        rightCount++;
        rightBox.grow(shape.bb);
      }
    }
    float cost = leftCount * leftBox.surface_area() +
                 rightCount * rightBox.surface_area();
    return cost > 0 ? cost : 1e30f;
  }

  float FindBestSplitPlane(BVHNode &node, int &axis, float &splitPos) {
    float bestCost = 1e30f;
    for (int a = 0; a < 3; a++) {
      float boundsMin = 1e30f, boundsMax = -1e30f;
      for (int i = 0; i < node.shapeCount; i++) {
        shape &shape = *shapes[shapeIdx[node.leftFirst + i]];
        boundsMin = min(boundsMin, shape.centroid.v[a]);
        boundsMax = max(boundsMax, shape.centroid.v[a]);
      }
      if (boundsMin == boundsMax)
        continue;

      int BINS = 32;

      Bin bin[BINS];
      float scale = BINS / (boundsMax - boundsMin);
      for (uint i = 0; i < node.shapeCount; i++) {
        shape &shape = *shapes[shapeIdx[node.leftFirst + i]];
        int binIdx =
            min(BINS - 1, (int)((shape.centroid.v[a] - boundsMin) * scale));
        bin[binIdx].shapeCount++;
        bin[binIdx].bounds.grow(shape.bb);
      }

      // gather data for the 7 planes between the 8 bins
      float leftArea[BINS - 1], rightArea[BINS - 1];
      int leftCount[BINS - 1], rightCount[BINS - 1];
      aabb leftBox, rightBox;
      int leftSum = 0, rightSum = 0;
      for (int i = 0; i < BINS - 1; i++) {
        leftSum += bin[i].shapeCount;
        leftCount[i] = leftSum;
        leftBox.grow(bin[i].bounds);
        leftArea[i] = leftBox.surface_area();
        rightSum += bin[BINS - 1 - i].shapeCount;
        rightCount[BINS - 2 - i] = rightSum;
        rightBox.grow(bin[BINS - 1 - i].bounds);
        rightArea[BINS - 2 - i] = rightBox.surface_area();
      }
      // calculate SAH cost for the 7 planes
      scale = (boundsMax - boundsMin) / BINS;
      for (int i = 0; i < BINS - 1; i++) {
        float planeCost =
            leftCount[i] * leftArea[i] + rightCount[i] * rightArea[i];
        if (planeCost < bestCost)
          axis = a, splitPos = boundsMin + scale * (i + 1),
          bestCost = planeCost;
      }
    }
    return bestCost;
  }

  float CalculateNodeCost(BVHNode &node) {
    vec3 e = node.aabbMax - node.aabbMin; // extent of the node
    float surfaceArea = e.v[0] * e.v[1] + e.v[1] * e.v[2] + e.v[2] * e.v[0];
    return node.shapeCount * surfaceArea;
  }
};

#endif // bhv_h
