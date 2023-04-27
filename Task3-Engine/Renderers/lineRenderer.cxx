#include "lineRenderer.hxx"
#include <cmath>
#include <iostream>

namespace core {

LineRenderer::LineRenderer(Canvas &canvas) : canvas(canvas) {}

Color LineRenderer::Interpolate(Color from, Color to, int curr, int count) {
  if (from == to)
    return from;
  double passed = static_cast<double>(curr) / static_cast<double>(count);
  double remain =
      std::abs(static_cast<double>(count - curr) / static_cast<double>(count));
  // std::cout << "Interpolated: " << passed << " " << remain << std::endl;
  return {from.r + passed + to.r * remain, from.g * passed + to.g * remain,
          from.b * passed + to.b * remain};
}

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

std::vector<Vertex> LineRenderer::DrawInterpolated(Position from, Position to,
                                                   Color colorFrom,
                                                   Color colorTo) {
  size_t pixelsCount = 0;

  std::abs(from.y - to.y) >= std::abs(from.x - to.x)
      ? pixelsCount = static_cast<size_t>(std::round(std::abs(from.y - to.y)))
      : pixelsCount = static_cast<size_t>(std::round(std::abs(from.x - to.x)));

  std::vector<Vertex> linePixels;
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
      Color newColor =
          Interpolate(colorFrom, colorTo, std::abs(i), pixelsCount);
      canvas.SetPixel(x, y, newColor);
      linePixels.push_back({{x, y}, newColor});
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
      Color newColor =
          Interpolate(colorFrom, colorTo, std::abs(i), pixelsCount);
      canvas.SetPixel(x, y, newColor);
      linePixels.push_back({{x, y}, newColor});
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