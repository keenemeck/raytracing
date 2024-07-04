#define TINYOBJLOADER_IMPLEMENTATION

#include "quad.h"
#include "shape.h"
#include "tiny_obj_loader.h"
#include "triangle.h"
#include "vec3.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void load_obj(string filename, vector<shape *> *scene, material *mat,
              float scale, vector<float> offsets, float radians = 0) {
  clog << "Loading obj...\n";

  tinyobj::ObjReaderConfig reader_config;
  reader_config.mtl_search_path = "./mtls/"; // Path to material files

  tinyobj::ObjReader reader;

  filename = "./objs/" + filename;

  if (!reader.ParseFromFile(filename, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    exit(1);
  }

  if (!reader.Warning().empty()) {
    std::clog << "TinyObjReader: " << reader.Warning();
  }

  auto &attrib = reader.GetAttrib();
  auto &shapes = reader.GetShapes();
  auto &materials = reader.GetMaterials();

  float maxx = -999, maxy = -999, maxz = -999, minx = 999, miny = 999,
        minz = 999;

  float sin_theta = sin(radians);
  float cos_theta = cos(radians);

  for (size_t s = 0; s < shapes.size(); ++s) {
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
      int fv = shapes[s].mesh.num_face_vertices[f];

      if (fv == 3 || 4) {
        vec3 verts[fv];
        vec3 norms[fv];
        for (int v = 0; v < fv; v++) {
          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
          tinyobj::real_t vx =
              attrib.vertices[3 * size_t(idx.vertex_index) + 0];
          tinyobj::real_t vy =
              attrib.vertices[3 * size_t(idx.vertex_index) + 1];
          tinyobj::real_t vz =
              attrib.vertices[3 * size_t(idx.vertex_index) + 2];
          verts[v] = {((cos_theta * vx + sin_theta * vz) + offsets[0]) * scale,
                      (vy + offsets[1]) * scale,
                      ((-sin_theta * vx + cos_theta * vz) + offsets[2]) *
                          scale};
          maxx = std::fmaxf(maxx, float(vx));
          maxy = std::fmaxf(maxy, float(vy));
          maxz = std::fmaxf(maxz, float(vz));
          minx = std::fminf(minx, float(vx));
          miny = std::fminf(miny, float(vy));
          minz = std::fminf(minz, float(vz));

          if (idx.normal_index >= 0) {
            tinyobj::real_t nx =
                attrib.normals[3 * size_t(idx.normal_index) + 0];
            tinyobj::real_t ny =
                attrib.normals[3 * size_t(idx.normal_index) + 1];
            tinyobj::real_t nz =
                attrib.normals[3 * size_t(idx.normal_index) + 2];
            norms[v] = {nx, ny, nz};
          }
        }
        if (fv == 3) {
          triangle *new_tri = new triangle(verts[0], verts[1], verts[2]);
          new_tri->mat = *mat;
          scene->push_back(new_tri);
        } else {
          quad *new_quad = new quad(verts[0], verts[2], verts[2]);
          new_quad->mat.color = {255, 255, 255};
          new_quad->mat = *mat;
          scene->push_back(new_quad);
        }
        index_offset += 3;
      } else {
        index_offset += fv;
      }
    }
  }

  float sc =
      1.0 / std::fmaxf(std::fmaxf(maxx - minx, maxy - miny), maxz - minz) * 0.5;
  clog << "scale: " << sc << "\n";
  clog << "offsets (center): " << -(maxx + minx) * 0.5 << ", "
       << -(maxy + miny) * 0.5 << ", " << -(maxz + minz) * 0.5 << "\n";
  clog << "offsets (floor): " << -(maxx + minx) * 0.5 << ", "
       << -0.5 / sc - miny << ", " << -(maxz + minz) * 0.5 << "\n";
}
