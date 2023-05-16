#include "../canvas.hxx"
#include "vertexBuffer.hxx"

#include <vector>

namespace core {

struct GlVertex {
  float x;
  float y;
  float z;

  friend GlVertex operator+(const GlVertex &l, const GlVertex &r);
  friend GlVertex operator*(const GlVertex &l, float val);
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