#ifndef BVH_H
#define BVH_H

#include "shape.h"
#include "vec3.h"
#include <vector>

using namespace std;

class bvhNode {
public:
  bvhNode() {}
  vec3 aabbMin, aabbMax;
  int leftNode, firstShapeIdx, shapeCount;
  bool isLeaf() { return shapeCount > 0; }
};

class bvh {
public:
  bvh() {}
  bvh(vector<shape *> &s) {
    shapes = s;
    clog << shapes.size() << " is the size\n";
    bvhNodes = new bvhNode[shapes.size() * 2 - 1];
    shapeIdx = new int[shapes.size()];
    for (int i = 0; i < shapes.size(); i++) {
      shapeIdx[i] = i;
    }
    bvhNode &root = bvhNodes[rootNodeIdx];
    root.leftNode = 0;
    root.firstShapeIdx = 0, root.shapeCount = shapes.size();
    UpdateNodeBounds(rootNodeIdx);
    // subdivide recursively
    Subdivide(rootNodeIdx);
  }

  void UpdateNodeBounds(int nodeIdx) {
    bvhNode &node = bvhNodes[nodeIdx];
    float big = 999999;
    node.aabbMin = {big, big, big};
    node.aabbMax = {-big, -big, -big};
    for (int first = node.firstShapeIdx, i = 0; i < node.shapeCount; i++) {
      int idx = shapeIdx[first + i];
      shape *shape = shapes[idx];
      node.aabbMin = min(node.aabbMin, shape->bb.bbmin);
      node.aabbMin = min(node.aabbMin, shape->bb.bbmax);
      node.aabbMax = max(node.aabbMax, shape->bb.bbmin);
      node.aabbMax = max(node.aabbMax, shape->bb.bbmax);
    }
  }

  void Subdivide(int nodeIdx) {
    // terminate recursion
    bvhNode &node = bvhNodes[nodeIdx];
    if (node.shapeCount <= 2) {
      return;
    }

    // determine split axis and position
    int axis;
    float splitPos;
    float splitCost = FindBestSplitPlane(node, axis, splitPos);
    float nosplitCost = CalculateNodeCost(node);
    if (splitCost >= nosplitCost) {
      return;
    }

    // in-place partition
    int i = node.firstShapeIdx;
    int j = i + node.shapeCount - 1;
    while (i <= j) {
      if (shapes[shapeIdx[i]]->centroid.v[axis] < splitPos) {
        i++;
      } else {
        swap(shapeIdx[i], shapeIdx[j--]);
      }
    }

    // abort split if one of the sides is empty
    int leftCount = i - node.firstShapeIdx;
    if (leftCount == 0 || leftCount == node.shapeCount) {
      return;
    }
    // create child nodes
    int leftChildIdx = nodesUsed++;
    int rightChildIdx = nodesUsed++;
    bvhNodes[leftChildIdx].firstShapeIdx = node.firstShapeIdx;
    bvhNodes[leftChildIdx].shapeCount = leftCount;
    bvhNodes[rightChildIdx].firstShapeIdx = i;
    bvhNodes[rightChildIdx].shapeCount = node.shapeCount - leftCount;
    node.leftNode = leftChildIdx;
    node.shapeCount = 0;
    UpdateNodeBounds(leftChildIdx);
    UpdateNodeBounds(rightChildIdx);
    // recurse
    Subdivide(leftChildIdx);
    Subdivide(rightChildIdx);
  }

  bool IntersectBVH(ray &ray, int nodeIdx, hit &h) {
    bvhNode &node = bvhNodes[nodeIdx];
    aabb bb(node.aabbMin, node.aabbMax);
    if (!bb.ray_intersect(ray)) {
      return false;
    }
    if (node.isLeaf()) {
      bool hitSomething = false;
      hit tempHit;
      for (int i = 0; i < node.shapeCount; i++) {
        if (shapes[shapeIdx[node.firstShapeIdx + i]]->ray_intersect(ray,
                                                                    tempHit)) {
          hitSomething = true;
          if (tempHit.t < h.t) {
            h = tempHit;
          }
        }
      }
      return hitSomething;
    } else {
      bool leftHit = IntersectBVH(ray, node.leftNode, h);
      bool rightHit = IntersectBVH(ray, node.leftNode + 1, h);

      return leftHit || rightHit;
    }
  }

  float EvaluateSAH(bvhNode &node, int axis, float pos) {
    aabb leftBox, rightBox;
    int leftCount = 0, rightCount = 0;
    for (int i = 0; i < node.shapeCount; i++) {
      shape *shape = shapes[shapeIdx[node.leftNode + i]];
      if (shape->centroid.v[axis] < pos) {
        leftCount++;
        leftBox.grow(shape->bb);
      } else {
        rightCount++;
        rightBox.grow(shape->bb);
      }
    }
    float cost = leftCount * leftBox.surface_area() +
                 rightCount * rightBox.surface_area();
    return cost > 0 ? cost : 1e30f;
  }

  float FindBestSplitPlane(bvhNode &node, int &axis, float &splitPos) {
    float bestCost = 1e30f;
    for (int a = 0; a < 3; a++) {
      float boundsMin = node.aabbMin.v[a];
      float boundsMax = node.aabbMax.v[a];
      if (boundsMin == boundsMax) {
        continue;
      }
      float scale = (boundsMax - boundsMin) / 100;
      for (int i = 1; i < 100; i++) {
        float candidatePos = boundsMin + i * scale;
        float cost = EvaluateSAH(node, a, candidatePos);
        if (cost < bestCost) {
          splitPos = candidatePos, axis = a, bestCost = cost;
        }
      }
    }
    return bestCost;
  }

  float CalculateNodeCost(bvhNode &node) {
    vec3 e = node.aabbMax - node.aabbMin; // extent of the node
    float surfaceArea = e.v[0] * e.v[1] + e.v[1] * e.v[2] + e.v[2] * e.v[0];
    return node.shapeCount * surfaceArea;
  }

  vector<shape *> shapes;
  bvhNode *bvhNodes;
  int *shapeIdx;
  int rootNodeIdx = 0, nodesUsed = 1;
};

#endif // bhv_h
