#include "lineRenderer.hxx"
#include <cmath>
#include <iostream>
//#include <set>
//#include <unordered_map>

namespace core {

LineRenderer::LineRenderer(Canvas &canvas) : canvas(canvas) {}

Color LineRenderer::Interpolate(Color from, Color to, int curr, int count) {
  if (from == to)
    return from;
  double passed = static_cast<double>(curr) / static_cast<double>(count);
  double remain =
      std::abs(static_cast<double>(count - curr) / static_cast<double>(count));
  // std::cout << "Interpolated: " << passed << " " << remain << std::endl;
  return {from.r * passed + to.r * remain, from.g * passed + to.g * remain,
          from.b * passed + to.b * remain};
}

void LineRenderer::CreateBresenhamsLine(Position &from, Position &to,
                                        std::vector<Vertex> &pixels) {
  int x1 = from.x;
  int y1 = from.y;
  int x2 = to.x;
  int y2 = to.y;

  int dx = x2 - x1;
  int dy = y2 - y1;

  int error = 0;

  int x = x1;
  int y = y1;
  if (std::abs(dx) > std::abs(dy)) {
    int derror = std::abs(dy) * 2;

    for (int i = 0; dx > 0 ? i <= dx : i >= dx; dx >= 0 ? i++ : i--) {
      // canvas.SetPixel(x, y, color);
      pixels.push_back({{x, y}, {0, 0, 0}});
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
      // canvas.SetPixel(x, y, color);
      pixels.push_back({{x, y}, {0, 0, 0}});
      y = y1 + i;
      error += derror;
      if (2 * error > std::abs(dy)) {
        x += (dx > 0 ? 1 : -1);
        error -= std::abs(dy) * 2;
      }
    }
  }
}

// Bresenham's algorithm.
std::vector<Vertex> LineRenderer::Draw(Position from, Position to,
                                       Color color) {

  std::vector<Vertex> linePixels;
  // std::unordered_map<int, std::set<int>> linePixels;
  CreateBresenhamsLine(from, to, linePixels);

  for (auto &pixel : linePixels) {
    pixel.color = color;
    canvas.SetPixel(pixel.pos.x, pixel.pos.y, pixel.color);
  }

  return linePixels;
}

std::vector<Vertex> LineRenderer::DrawInterpolated(Vertex from, Vertex to) {
  size_t pixelsCount = 0;

  std::abs(from.pos.y - to.pos.y) >= std::abs(from.pos.x - to.pos.x)
      ? pixelsCount =
            static_cast<size_t>(std::round(std::abs(from.pos.y - to.pos.y)))
      : pixelsCount =
            static_cast<size_t>(std::round(std::abs(from.pos.x - to.pos.x)));

  std::vector<Vertex> linePixels;

  CreateBresenhamsLine(from.pos, to.pos, linePixels);

  for (int i = 0; i < linePixels.size(); i++) {
    Color newColor =
        Interpolate(from.color, to.color, std::abs(i), pixelsCount);
    linePixels[i].color = newColor;
    canvas.SetPixel(linePixels[i].pos.x, linePixels[i].pos.y,
                    linePixels[i].color);
  }

  return linePixels;
}
} // namespace core