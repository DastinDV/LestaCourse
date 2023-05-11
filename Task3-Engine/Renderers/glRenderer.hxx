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

class GlRenderer {
public:
  GlRenderer();
  void DrawPoint(std::vector<GlVertex> &pos);
  void DrawTriangle(std::vector<GlVertex> &vertecies);

  VertexBuffer *pointBuffer;
  VertexBuffer *triangleBuffer;
};
} // namespace core