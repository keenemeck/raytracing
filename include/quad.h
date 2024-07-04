#ifndef quad_h
#define quad_h

#include "ray.h"
#include "shape.h"
#include <algorithm>

class quad : public shape {
public:
  quad() {}
  quad(vec3 point0, vec3 point1, vec3 point2)
      : p0(point0), p1(point1), p2(point2) {
    u = point0 - point1;
    v = point2 - point1;
    vec3 n = cross(u, v);
    normal = n.normal();
    D = dot(normal, point1);
    w = n / dot(n, n);
    bb.bbmin = {min(min(point0, point1), point2)};
    bb.bbmax = {max(max(point0, point1), point2)};
    bb.pad();
    centroid = (bb.bbmin + bb.bbmax) / 2.0;
  }

  bool ray_intersect(ray &r, hit &hit) override {
    float denom = dot(normal, r.direction);

    float t = (D - dot(normal, r.point)) / denom;

    // Determine the hit point lies within the planar shape using its plane
    // coordinates.
    auto intersection = r.p(t);
    vec3 planar_hitpt_vector = intersection - p1;
    float a = dot(w, cross(planar_hitpt_vector, v));
    float b = dot(w, cross(u, planar_hitpt_vector));

    if (!(a >= 0 && a <= 1) || !(b >= 0 && b <= 1)) {
      return false;
    }

    hit.t = t;
    hit.mat = mat;
    hit.normal = normal;
    if (dot(r.direction, hit.normal) > 0) {
      hit.normal = -hit.normal;
      hit.front = false;
    }
    hit.location = r.p(t);

    return t >= 0.001;
  }

  vec3 p0;
  vec3 p1;
  vec3 p2;
  vec3 u, v, w;
  float D;
};

#endif // quad_h
