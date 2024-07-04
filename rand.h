#ifndef rand_h
#define rand_h

#include <cstdlib>

inline float random_float() {
  // Returns a random real in [0,1).
  return rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max) {
  // Returns a random real in [min,max).
  return min + (max - min) * random_float();
}

vec3 random_vec() {
  return vec3(random_float(), random_float(), random_float());
}

vec3 random_vec(float min, float max) {
  return vec3(random_float(min, max), random_float(min, max),
              random_float(min, max));
}

inline vec3 random_unit_vector() {
  while (true) {
    vec3 p = random_vec(-1, 1);
    if (p.length() * p.length() < 1) {
      return p.normalize();
    }
  }
}

inline vec3 random_on_hemisphere(const vec3 &normal) {
  vec3 on_unit_sphere = random_unit_vector();
  if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
    return on_unit_sphere;
  else
    return -on_unit_sphere;
}

#endif // rand_h
