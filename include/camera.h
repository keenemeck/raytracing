#ifndef camera_h
#define camera_h

#include "aabb.h"
#include "bvh.h"
#include "rand.h"
#include "ray.h"
#include "shape.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

vec3 reflect(vec3 &direction, vec3 &normal) {
  return (direction - normal * 2 * dot(direction, normal));
}

vec3 refract(const vec3 &direction, const vec3 &normal, float ir) {
  float cos_theta = fminf(dot(-direction, normal), 1.0);
  vec3 r_out_perp = (normal * cos_theta + direction) * ir;
  vec3 r_out_parallel =
      normal * -sqrt(fabs(1.0 - r_out_perp.length() * r_out_perp.length()));
  return r_out_perp + r_out_parallel;
}

// TODO: move this bruh
vec3 ray_color(ray &r, vector<shape *> &shapes, int bounces_left, bvh &BVH) {

  if (bounces_left < 0) {
    return {0, 0, 0};
  }

  hit h;
  hit closest_hit;
  // TODO: add float.h and use DBL_MAX idk
  float DBL_MAX = 100000;
  float t = DBL_MAX;

  bool found = false;
  /*for (shape *s : shapes) {
    if (s->ray_intersect(r, h) && h.t < t) {
      t = h.t;
      closest_hit = h;
      found = true;
    }
  }*/

  BVH.IntersectBVH(r, 0, closest_hit);

  // if no intersection occurs
  if (closest_hit.t > 10000) {
    return {0, 0, 0};
  }

  // normal shading
  vec3 n = (closest_hit.normal + 1) * 127.5;
  // return {n.v[0], n.v[1], n.v[2]};

  // TODO: glass
  if (closest_hit.mat.dielectric) {
    float ir =
        closest_hit.front ? closest_hit.mat.ir : 1.0 / closest_hit.mat.ir;
    ray new_ray(closest_hit.location,
                refract(r.direction, closest_hit.normal, ir));
    return ray_color(new_ray, shapes, bounces_left - 1, BVH) *
           (closest_hit.mat.color / 255);
  }

  if (closest_hit.mat.reflective) {
    ray new_ray(closest_hit.location,
                (reflect(r.direction, closest_hit.normal) +
                 random_unit_vector() * closest_hit.mat.fuzz)
                    .normal());
    return ray_color(new_ray, shapes, bounces_left - 1, BVH) *
           (closest_hit.mat.color / 255);
  }

  if (closest_hit.mat.emissive) {
    return closest_hit.mat.color;
  }

  ray new_ray(closest_hit.location,
              (closest_hit.normal + random_unit_vector()).normalize());

  return ray_color(new_ray, shapes, bounces_left - 1, BVH) *
         (closest_hit.mat.color / 255);
}

class camera {
public:
  float aspect_ratio;
  int image_height;
  int samples;
  int bounces;
  float fov;
  vec3 camera_origin;
  vec3 lookat;
  vec3 vup;
  vector<shape *> scene;
  bvh BVH;

  vec3 u, v, w;

  void time_estimate(int height, int s) {
    auto start = chrono::high_resolution_clock::now();

    w = (camera_origin - lookat).normalize();
    u = (cross(vup, w)).normalize();
    v = cross(w, u);

    float focal_length = (camera_origin - lookat).length();

    float theta = fov * 3.14159265 / 180;
    float h = tan(theta / 2);
    float scene_height = 2 * h * focal_length;
    float scene_width = scene_height * aspect_ratio;

    int image_width = round(100 * aspect_ratio);

    vec3 scene_u = u * scene_width;
    vec3 scene_v = -v * scene_height;

    vec3 du = scene_u / image_width;
    vec3 dv = scene_v / 100;

    clog << "Estimating time...\n";

    int tilesize = 2;
    vector<vec3> color_buffer(image_width * tilesize);

    for (int i = 0; i < 100; i += tilesize) {
      for (int j = 0; j < image_width; j += tilesize) {
        for (int itile = 0; itile < tilesize; itile++) {
          for (int jtile = 0; jtile < tilesize; jtile++) {
            vec3 color;
            for (int k = 0; k < 10; k++) {
              vec3 direction = camera_origin - (w * focal_length) -
                               scene_u / 2 - scene_v / 2 +
                               du * (j + jtile + random_float()) +
                               dv * (i + itile + random_float());
              ray r(camera_origin, direction.normalize());

              color += ray_color(r, scene, bounces, BVH);
            }

            color /= samples;
            color.v[0] = fminf(255.0, round(sqrtf(color.v[0] / 255) * 255));
            color.v[1] = fminf(255.0, round(sqrtf(color.v[1] / 255) * 255));
            color.v[2] = fminf(255.0, round(sqrtf(color.v[2] / 255) * 255));
            int index = itile * image_width + (j + jtile);
          }
        }
      }
    }

    float timescale = (s / 10.0) * (height / 100.0) * (height / 100.0);
    auto stop = chrono::high_resolution_clock::now();
    clog << "Estimated time to render: "
         << chrono::duration_cast<chrono::microseconds>(stop - start).count() *
                timescale / 60000000.0
         << " minutes.\n";
  }

  void render() {
    auto start = chrono::high_resolution_clock::now();

    w = (camera_origin - lookat).normalize();
    u = (cross(vup, w)).normalize();
    v = cross(w, u);

    float focal_length = (camera_origin - lookat).length();

    float theta = fov * 3.14159265 / 180;
    float h = tan(theta / 2);
    float scene_height = 2 * h * focal_length;
    float scene_width = scene_height * aspect_ratio;

    int image_width = round(image_height * aspect_ratio);

    vec3 scene_u = u * scene_width;
    vec3 scene_v = -v * scene_height;

    vec3 du = scene_u / image_width;
    vec3 dv = scene_v / image_height;

    cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    int tilesize = 2;
    vector<vec3> color_buffer(image_width * tilesize);

    for (int i = 0; i < image_height; i += tilesize) {
      clog << "\r" << i << " / " << image_height;
      for (int j = 0; j < image_width; j += tilesize) {
        for (int itile = 0; itile < tilesize; itile++) {
          for (int jtile = 0; jtile < tilesize; jtile++) {
            vec3 color;
            for (int k = 0; k < samples; k++) {
              vec3 direction = camera_origin - (w * focal_length) -
                               scene_u / 2 - scene_v / 2 +
                               du * (j + jtile + random_float()) +
                               dv * (i + itile + random_float());
              ray r(camera_origin, direction.normalize());

              color += ray_color(r, scene, bounces, BVH);
            }

            color /= samples;
            color.v[0] = fminf(255.0, round(sqrtf(color.v[0] / 255) * 255));
            color.v[1] = fminf(255.0, round(sqrtf(color.v[1] / 255) * 255));
            color.v[2] = fminf(255.0, round(sqrtf(color.v[2] / 255) * 255));
            int index = itile * image_width + (j + jtile);
            color_buffer[index] = color;
          }
        }
      }
      for (int i = 0; i < tilesize; ++i) {
        for (int j = 0; j < image_width; ++j) {
          cout << color_buffer[i * image_width + j] << " ";
        }
        cout << "\n";
      }
    }

    auto stop = chrono::high_resolution_clock::now();
    clog << "\nRender complete in: "
         << chrono::duration_cast<chrono::microseconds>(stop - start).count() /
                60000000.0
         << " minutes.\n";
  }
};

#endif // camera_h
