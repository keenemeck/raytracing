#ifndef material_h
#define material_h

#include "vec3.h"

class material {
public:
  vec3 color;
  bool reflective = false;
  bool emissive = false;
  bool dielectric = false;
  float ir = 1.0;
  float fuzz = 0.0;

private:
};

#endif // material_h
