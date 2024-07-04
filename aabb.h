#ifndef aabb_h
#define aabb_h

#include "ray.h"
#include <algorithm>
#include <limits>

class aabb {
public:
  vec3 bbmin = {1e30f, 1e30f, 1e30f};
  vec3 bbmax = {-1e30f, -1e30f, -1e30f};

  aabb() {}
  aabb(vec3 point1, vec3 point2) {
    bbmin = min(point1, point2);
    bbmax = max(point1, point2);
  }

  void pad() {
    bbmin = bbmin - 0.00001;
    bbmax = bbmax + 0.00001;
  }

  float ray_intersect(ray &r) {
    float tx1 = (bbmin.v[0] - r.point.v[0]) * r.rInv.v[0],
          tx2 = (bbmax.v[0] - r.point.v[0]) * r.rInv.v[0];
    float tmin = fminf(tx1, tx2), tmax = fmaxf(tx1, tx2);
    float ty1 = (bbmin.v[1] - r.point.v[1]) * r.rInv.v[1],
          ty2 = (bbmax.v[1] - r.point.v[1]) * r.rInv.v[1];
    tmin = fmaxf(tmin, fminf(ty1, ty2)), tmax = fminf(tmax, fmaxf(ty1, ty2));
    float tz1 = (bbmin.v[2] - r.point.v[2]) * r.rInv.v[2],
          tz2 = (bbmax.v[2] - r.point.v[2]) * r.rInv.v[2];
    tmin = fmaxf(tmin, fminf(tz1, tz2)), tmax = fminf(tmax, fmaxf(tz1, tz2));

    return tmin < tmax && tmax > 0 ? tmin : 1e30f;
  }

  void grow(aabb &bb) {
    bbmin = min(bbmin, bb.bbmin);
    bbmax = max(bbmax, bb.bbmax);
  }

  float surface_area() {
    vec3 e = bbmax - bbmin;
    return e.v[0] * e.v[1] + e.v[1] * e.v[2] + e.v[2] * e.v[0];
  }
};

#endif // aabb_h
