#include "../OpenGL/vertexBuffer.hxx"
#include "../canvas.hxx"

#include <vector>

namespace core {

struct GlVertex {
  float x;
  float y;
  float z;
};

unsigned int CompileShader(unsigned int type, const std::string &source);
int CreateShader(const std::string &vertexShader,
                 const std::string &fragmentShader);

class GlRenderer {
public:
  GlRenderer();
  void DrawPoint(std::vector<GlVertex> &pos);
  void DrawTriangle(std::vector<GlVertex> &vertecies);

  VertexBuffer *pointBuffer;
  VertexBuffer *triangleBuffer;
};
} // namespace core