#include "lineRenderer.hxx"
#include <iostream>

namespace core {

LineRenderer::LineRenderer(Canvas &canvas) : canvas(canvas) {}

// Bresenham's algorithm.
std::vector<Position> LineRenderer::Draw(Position from, Position to,
                                         Color color) {

  std::vector<Position> linePixels;
  int x1 = from.x;
  int y1 = from.y;
  int x2 = to.x;
  int y2 = to.y;

  int dx = x2 - x1;
  int dy = y2 - y1;

  // double error = 0;
  int error = 0;

  int x = x1;
  int y = y1;
  if (std::abs(dx) > std::abs(dy)) {
    int derror = std::abs(dy) * 2;

    for (int i = 0; dx > 0 ? i <= dx : i >= dx; dx >= 0 ? i++ : i--) {
      canvas.SetPixel(x, y, color);
      linePixels.push_back({x, y});
      x = x1 + i;
      error += derror;
      if (2 * error > std::abs(dx)) {
        y += (y2 > y1 ? 1 : -1);
        error -= std::abs(dx) * 2;
      }
    }
  } else {
    int derror = std::abs(dx) * 2;
    for (int i = 0; dy > 0 ? i <= dy : i >= dy; dy >= 0 ? i++ : i--) {
      canvas.SetPixel(x, y, color);
      linePixels.push_back({x, y});
      y = y1 + i;
      error += derror;
      if (2 * error > std::abs(dy)) {
        x += (dx > 0 ? 1 : -1);
        error -= std::abs(dy) * 2;
      }
    }
  }
  return linePixels;
}
} // namespace core