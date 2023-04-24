#pragma once
#include <string>
#include <vector>

namespace core {

struct Position {
  int x;
  int y;
};

struct Color {
  u_int8_t r = 0;
  u_int8_t g = 0;
  u_int8_t b = 0;
  friend bool operator==(const Color &l, const Color &r);
};

class Canvas {

public:
  Canvas(size_t width, size_t height);
  void SetPixel(size_t x, size_t y, Color color);
  void WriteImage(const std::string &fileName);
  void RenderToSDLWindow();

  const std::vector<Color> &GetPixels() const;

private:
  size_t width = 0;
  size_t height = 0;
  std::vector<Color> pixels;
};
} // namespace core