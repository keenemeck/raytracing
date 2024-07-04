#ifndef vec3_h
#define vec3_h

#include "math.h"
#include <algorithm>
#include <iostream>

class vec3 {
public:
  vec3() : v{0, 0, 0} {}
  vec3(float x, float y, float z) : v{x, y, z} {}

  float length() { return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }

  vec3 normal() {
    float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    return {v[0] / length, v[1] / length, v[2] / length};
  }

  vec3 &normalize() {
    float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    return *this /= length;
  }

  vec3 operator-() const { return {-v[0], -v[1], -v[2]}; }

  vec3 &operator+=(const vec3 &rhs) {
    v[0] += rhs.v[0];
    v[1] += rhs.v[1];
    v[2] += rhs.v[2];
    return *this;
  }

  vec3 &operator-=(const vec3 &rhs) { return *this += -rhs; }

  vec3 &operator*=(float t) {
    v[0] *= t;
    v[1] *= t;
    v[2] *= t;
    return *this;
  }

  vec3 &operator/=(float t) { return *this *= 1 / t; }

  friend std::ostream &operator<<(std::ostream &os, const vec3 &vec) {
    os << vec.v[0] << " " << vec.v[1] << " " << vec.v[2];
    return os;
  }

  float v[3];
};

vec3 operator+(const vec3 &v1, const vec3 &v2) {
  return {v1.v[0] + v2.v[0], v1.v[1] + v2.v[1], v1.v[2] + v2.v[2]};
}
vec3 operator+(const vec3 &vec, float t) {
  return {vec.v[0] + t, vec.v[1] + t, vec.v[2] + t};
}

vec3 operator-(const vec3 &v1, const vec3 &v2) {
  return {v1.v[0] - v2.v[0], v1.v[1] - v2.v[1], v1.v[2] - v2.v[2]};
}
vec3 operator-(const vec3 &vec, float t) {
  return {vec.v[0] - t, vec.v[1] - t, vec.v[2] - t};
}

vec3 operator*(const vec3 &v1, const vec3 &v2) {
  return {v1.v[0] * v2.v[0], v1.v[1] * v2.v[1], v1.v[2] * v2.v[2]};
}
vec3 operator/(const vec3 &v1, const vec3 &v2) {
  return {v1.v[0] / v2.v[0], v1.v[1] / v2.v[1], v1.v[2] / v2.v[2]};
}

vec3 operator*(const vec3 &vec, float t) {
  return {vec.v[0] * t, vec.v[1] * t, vec.v[2] * t};
}

vec3 operator/(const vec3 &vec, float t) {
  return {vec.v[0] / t, vec.v[1] / t, vec.v[2] / t};
}
vec3 operator/(float t, const vec3 &vec) {
  return {t / vec.v[0], t / vec.v[1], t / vec.v[2]};
}

float dot(const vec3 &v1, const vec3 &v2) {
  return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1] + v1.v[2] * v2.v[2];
}

vec3 cross(const vec3 &v1, const vec3 &v2) {
  return {v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1],
          v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2],
          v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0]};
}

vec3 min(const vec3 &v1, const vec3 &v2) {
  return {std::fminf(v1.v[0], v2.v[0]), std::fminf(v1.v[1], v2.v[1]),
          std::fminf(v1.v[2], v2.v[2])};
}

vec3 max(const vec3 &v1, const vec3 &v2) {
  return {std::fmaxf(v1.v[0], v2.v[0]), std::fmaxf(v1.v[1], v2.v[1]),
          std::fmaxf(v1.v[2], v2.v[2])};
}

#endif // vec3_h
