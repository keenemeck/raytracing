#ifndef shape_h
#define shape_h

#include "aabb.h"
#include "hit.h"
#include "material.h"
#include "ray.h"

class shape {
public:
  virtual ~shape() {}
  virtual bool ray_intersect(ray &r, hit &hit) = 0;

  vec3 normal;
  material mat;
  aabb bb;
  vec3 centroid;

private:
};

#endif // shape_h
