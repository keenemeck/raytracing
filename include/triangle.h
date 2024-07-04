#ifndef triangle_h
#define triangle_h

#include "limits.h"
#include "shape.h"
#include <algorithm>

class triangle : public shape {
public:
  triangle();
  triangle(vec3 a, vec3 b, vec3 c) : v{a, b, c} {
    vec3 p1 = min(min(a, b), c);
    vec3 p2 = max(max(a, b), c);
    bb.bbmin = p1;
    bb.bbmax = p2;
    centroid = (a + b + c) / 3.0;
  }

  bool ray_intersect(ray &r, hit &hit) override {
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    vec3 edge1 = v[1] - v[0];
    vec3 edge2 = v[2] - v[0];
    vec3 ray_cross_e2 = cross(r.direction, edge2);
    float det = dot(edge1, ray_cross_e2);

    if (det > -epsilon && det < epsilon) {
      return false; // This ray is parallel to this triangle.
    }

    float inv_det = 1.0 / det;
    vec3 s = r.point - v[0];
    float u = inv_det * dot(s, ray_cross_e2);

    if (u < 0 || u > 1) {
      return false;
    }

    vec3 s_cross_e1 = cross(s, edge1);
    float v = inv_det * dot(r.direction, s_cross_e1);

    if (v < 0 || u + v > 1) {
      return false;
    }

    float t = inv_det * dot(edge2, s_cross_e1);

    if (t > epsilon) { // ray intersection
      hit.mat = mat;
      hit.t = t;
      hit.location = r.point + r.direction * t;

      // Compute the normal
      vec3 u_vec = this->v[1] - this->v[0];
      vec3 v_vec = this->v[2] - this->v[0];
      hit.normal = cross(u_vec, v_vec).normalize();

      if (dot(r.direction, hit.normal) > 0) {
        hit.normal = -hit.normal;
      }

      return t > 0.0001;
    } else { // This means that there is a line intersection but not a ray
             // intersection.
      return false;
    }
  }
  vec3 v[3];

private:
};

#endif // triangle_h
