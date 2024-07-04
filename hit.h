#ifndef hit_h
#define hit_h

#include "material.h"
#include "vec3.h"

class hit {
public:
  float t = 99999.0;
  bool front = true; // TODO: implement this properly
  vec3 normal;
  vec3 location;
  material mat;

private:
};

#endif // !hit_h
