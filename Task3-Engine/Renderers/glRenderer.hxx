#include "../canvas.hxx"
#include "vertexBuffer.hxx"

#include <vector>

namespace core {

struct GlVertex {
  float x;
  float y;
  float z;

  float r;
  float g;
  float b;

  friend GlVertex operator+(const GlVertex &l, const GlVertex &r);
  friend GlVertex operator*(const GlVertex &l, float val);
};

struct GLTriangle {
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

  GlVertex v[3];
};

unsigned int CompileShader(unsigned int type, const std::string &source);
int CreateShader(const std::string &vertexShader,
                 const std::string &fragmentShader);

std::string GetShaderSource(std::string path);
float *ParseVerticies(std::string path, char separator, int &count);
unsigned int *ParseIndexies(std::string path, char separator, int &count);

enum EGlType { gl_float };

class GlRenderer {
public:
  GlRenderer();
  void DrawPoint(const int size);
  void DrawTriangle(const int size);

  void SetAttribute(int id, int size, EGlType type, int stride, void *offset);
  void SetBuffer(VertexBuffer *buffer);

  VertexBuffer *currentBuffer;
};
} // namespace core