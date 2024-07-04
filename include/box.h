#ifndef box_h
#define box_h

#include "math.h"
#include "ray.h"
#include "shape.h"
#include <algorithm>

class box : public shape {
public:
  box() {}
  box(vec3 point0, vec3 point1) : p0(point0), p1(point1) {
    bb.bbmin = {std::min(point0.v[0], point1.v[0]),
                std::min(point0.v[1], point1.v[1]),
                std::min(point0.v[2], point1.v[2])};
    bb.bbmax = {std::max(point0.v[0], point1.v[0]),
                std::max(point0.v[1], point1.v[1]),
                std::max(point0.v[2], point1.v[2])};
    bb.pad();
  }

  bool ray_intersect(ray &r, hit &hit) override {

    return false;
    // hit.t = (tmin < 0) ? tmax : tmin;
    hit.mat = mat;
    hit.location = r.p(hit.t);
    vec3 center((std::max(p0.v[0], p1.v[0]) - std::min(p0.v[0], p1.v[0])) / 2.0,
                (std::max(p0.v[1], p1.v[1]) - std::min(p0.v[1], p1.v[1])) / 2.0,
                (std::max(p0.v[2], p1.v[2]) - std::min(p0.v[2], p1.v[2])) /
                    2.0);
    hit.normal = (r.p(hit.t) - center).normalize();
    if (dot(r.direction, hit.normal) > 0) {
      hit.normal = -hit.normal;
      hit.front = false;
    }
    return true;
  }

  vec3 p0;
  vec3 p1;
};

#endif // box_h
