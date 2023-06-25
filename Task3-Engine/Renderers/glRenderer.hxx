#pragma once
#include "../canvas.hxx"
#include "vertexBuffer.hxx"

#include <vector>

#ifndef IZ_DECLSPEC
#define IZ_DECLSPEC
#endif

namespace core {

struct IZ_DECLSPEC GlVertex {
  float x;
  float y;
  float z;

  float r;
  float g;
  float b;

  friend GlVertex operator+(const GlVertex &l, const GlVertex &r);
  friend GlVertex operator*(const GlVertex &l, float val);
};

struct IZ_DECLSPEC GLTriangle {
  GLTriangle() {
    v[0] = GlVertex();
    v[1] = GlVertex();
    v[2] = GlVertex();
  }

  std::vector<float> asBuf() {
    std::vector<float> elements;
    elements.push_back(v[0].x);
    elements.push_back(v[0].y);
    elements.push_back(v[0].z);
    elements.push_back(v[1].x);
    elements.push_back(v[1].y);
    elements.push_back(v[1].z);
    elements.push_back(v[2].x);
    elements.push_back(v[2].y);
    elements.push_back(v[2].z);
    return elements;
  }

  void fillGlVertecies(float *data, int offset = 0) {
    v[0].x = data[0 + offset];
    v[0].y = data[1 + offset];
    v[0].z = data[2 + offset];

    v[1].x = data[3 + offset];
    v[1].y = data[4 + offset];
    v[1].z = data[5 + offset];

    v[2].x = data[6 + offset];
    v[2].y = data[7 + offset];
    v[2].z = data[8 + offset];
  }

  GlVertex v[3];
};

unsigned int IZ_DECLSPEC CompileShader(unsigned int type, const std::string &source);
int IZ_DECLSPEC CreateShader(const std::string &vertexShader,
                 const std::string &fragmentShader);

std::string IZ_DECLSPEC GetShaderSource(std::string path);
float * ParseVerticies(std::string path, char separator, int &count);
unsigned int * ParseIndexies(std::string path, char separator, int &count);

enum EGlType { gl_float };

class IZ_DECLSPEC GlRenderer {
public:
  GlRenderer();
  void DrawPoint(const int size);
  void DrawTriangle(const int size);

  void SetAttribute(int id, int size, EGlType type, int stride, void *offset);
  void SetBuffer(VertexBuffer *buffer);

  VertexBuffer *currentBuffer;
};
} // namespace core