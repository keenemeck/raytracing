#ifndef sphere_h
#define sphere_h

#include "shape.h"
#include <algorithm>

class sphere : public shape {
public:
  sphere();
  sphere(vec3 c, float r) : center(c), radius(r) {
    bb.bbmin = {center.v[0] - r, center.v[1] - r, center.v[2] - r};
    bb.bbmax = {center.v[0] + r, center.v[1] + r, center.v[2] + r};
    bb.pad();
    centroid = center;
  }

  bool ray_intersect(ray &r, hit &hit) override {
    vec3 oc = center - r.point;
    auto a = r.direction.length() * r.direction.length();
    auto h = dot(r.direction, oc);
    auto c = oc.length() * oc.length() - radius * radius;
    auto discriminant = h * h - a * c;

    if (discriminant < 0) {
      return false;
    } else {
      float t = (h - sqrt(discriminant)) / a;
      hit.mat = mat;
      hit.t = t;
      hit.location = r.p(t);
      hit.normal = (r.p(t) - center).normal();
      if (dot(r.direction, hit.normal) > 0) {
        hit.normal = -hit.normal;
        hit.front = false;
      }
      return t > 0.001;
    }
  }

  vec3 center;
  float radius;

private:
};

#endif // sphere_h
