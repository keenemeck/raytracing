#ifndef ray_h
#define ray_h

#include "vec3.h"

class ray {
public:
  ray() : point({0, 0, 0}), direction({0, 0, 0}) {}
  ray(const vec3 &p, const vec3 &dir) : point(p), direction(dir) {
    rInv = 1.0 / direction;
  }

  vec3 p(float t) {
    return {point.v[0] + direction.v[0] * t, point.v[1] + direction.v[1] * t,
            point.v[2] + direction.v[2] * t};
  }

  vec3 point, direction, rInv;

private:
};

#endif // ray_h
