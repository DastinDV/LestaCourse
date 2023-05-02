#pragma once
#include <string>
#include <vector>

namespace core {

struct Position {
  int x;
  int y;
  friend bool operator==(const Position &l, const Position &r);
};

#pragma pack(push, 1)
struct Color {
  u_int8_t r = 0;
  u_int8_t g = 0;
  u_int8_t b = 0;
  friend bool operator==(const Color &l, const Color &r);
};
#pragma pack(pop)

struct Vertex {
  double x;
  double y;
  Color color;
  friend bool operator<(const Vertex &vertex, const Vertex &other);
  double delta;
};

const Color red = {255, 0, 0};
const Color green = {0, 255, 0};
const Color blue = {0, 0, 255};

class Canvas {

public:
  Canvas(size_t width, size_t height);
  void SetPixel(size_t x, size_t y, Color &color);
  void WriteImage(const std::string &fileName);
  void RenderToSDLWindow();
  void Clear(Color color);
  void SaveImage(std::string fileName);

  const std::vector<Color> &GetPixels() const;

private:
  size_t width = 0;
  size_t height = 0;
  std::vector<Color> pixels;
};

struct Uniforms {
  double f0 = 0;
  double f1 = 0;
  double f2 = 0;
  double f3 = 0;
  double f4 = 0;
  double f5 = 0;
  double f6 = 0;
  double f7 = 0;
  Canvas *texture0 = nullptr;
};

struct gfx_program {
  virtual ~gfx_program() = default;
  virtual void set_uniforms(const Uniforms &) = 0;
  virtual Vertex vertex_shader(const Vertex &v_in) = 0;
  virtual Color fragment_shader(const Vertex &v_in) = 0;
};

} // namespace core