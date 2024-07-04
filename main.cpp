#include "camera.h"
#include "quad.h"
#include "sphere.h"
#include "utils.h"
#include <string>
#include <vector>

using namespace std;

int main() {

  quad q1({0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {0.5, 0.5, 0.5});
  quad q2({-0.5, 0.5, 0.5}, {-0.5, 0.5, -0.5}, {-0.5, -0.5, -0.5});
  quad q3({-0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, {0.5, 0.5, -0.5});
  quad q4({0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}, {-0.5, -0.5, 0.5});
  quad q5({0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5}, {-0.5, 0.5, -0.5});
  quad q6({0.3, 0.4999, -0.3}, {0.3, 0.4999, 0.3}, {-0.3, 0.4999, 0.3});
  quad q7({0.4999, -0.3, -0.3}, {0.4999, 0.3, -0.3}, {0.4999, 0.3, 0.3});
  quad q8({-0.4999, 0.3, 0.3}, {-0.4999, 0.3, -0.3}, {-0.4999, -0.3, -0.3});

  material wLight;
  wLight.emissive = true;
  wLight.color = {255 * 7, 255 * 7, 255 * 7};

  material rLight;
  rLight.emissive = true;
  rLight.color = {166 * 7, 13 * 7, 13 * 7};

  material gLight;
  gLight.emissive = true;
  gLight.color = {31 * 7, 115 * 7, 38 * 7};

  material bLight;
  bLight.emissive = true;
  bLight.color = {13 * 7, 13 * 7, 166 * 7};

  material wDiffuse;
  wDiffuse.color = {255, 255, 255};

  material wReflect;
  wReflect.color = {255, 255, 255};
  wReflect.reflective = true;

  material bGlass;
  bGlass.color = {0, 90, 136};
  bGlass.dielectric = true;
  bGlass.ir = 1.5;

  material bDiffuse;
  bDiffuse.color = {0, 90, 136};

  material bReflect;
  bReflect.color = {0, 90, 136};
  bReflect.reflective = true;
  bReflect.fuzz = 0.5;

  q1.mat.color = {166, 13, 13};
  q2.mat.color = {31, 115, 38};
  q3.mat.color = {186, 186, 186};
  q4.mat.color = {186, 186, 186};
  q5.mat.color = {186, 186, 186};
  q6.mat = wLight;
  q7.mat = rLight;
  q8.mat = bLight;

  sphere bs({0.25, -0.375, 0.25}, 0.125);
  bs.mat = bReflect;

  q3.mat.color = {255, 255, 255};

  vector<shape *> shapes = {&q6, &q7, &q8};

  float scale;
  vector<float> offsets(3);
  string filename = "dragon.obj";

  if (filename == "bunny.obj") {
    scale = 3.21132;
    offsets = {0.0168405, -0.188686, 0.001537};
  } else if (filename == "dragon.obj") {
    scale = 3.6605;
    offsets = {0.0058789, -0.189323, 0.0046034};
  } else if (filename == "icosahedron.obj") {
    scale = 0.293893;
    offsets = {0, 0, 0};
  }

  load_obj(filename, &shapes, &wDiffuse, scale, offsets);

  camera cam;

  cam.image_height = 800;
  cam.samples = 2000;
  cam.bounces = 20;

  switch (1) {
  // cornell
  case 1:
    cam.aspect_ratio = 1.0;
    cam.fov = 13;
    cam.camera_origin = {0, 0, 2};
    cam.lookat = {0, 0, -1};
    cam.vup = {0, 1, 0};
    cam.scene = shapes;
    cam.BVH = bvh(shapes);
    break;
  }

  cam.time_estimate(cam.image_height, cam.samples);
  cam.render();

  return 0;
}
