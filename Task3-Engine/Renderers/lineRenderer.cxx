#include "lineRenderer.hxx"
#include <cmath>
#include <iostream>
//#include <set>
//#include <unordered_map>

namespace core {

LineRenderer::LineRenderer(Canvas &canvas) : canvas(canvas) {}

void LineRenderer::SetGFXProgram(gfx_program *program) {
  this->program = program;
}

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
  if (from == to)
    return;
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
      pixels.push_back({(double)x, (double)y, {0, 0, 0}});
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
      pixels.push_back({(double)x, (double)y, {0, 0, 0}});
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

  size_t pixelsCount = 0;

  std::abs(from.y - to.y) >= std::abs(from.x - to.x)
      ? pixelsCount = static_cast<size_t>(std::round(std::abs(from.y - to.y)))
      : pixelsCount = static_cast<size_t>(std::round(std::abs(from.x - to.x)));

  std::vector<Vertex> linePixels;

  linePixels.reserve(pixelsCount);
  // std::unordered_map<int, std::set<int>> linePixels;
  CreateBresenhamsLine(from, to, linePixels);

  for (auto &pixel : linePixels) {
    pixel.color = color;
    pixel.color = program->fragment_shader(pixel);
    canvas.SetPixel(pixel.x, pixel.y, pixel.color);
  }

  return linePixels;
}

std::vector<Vertex> LineRenderer::DrawInterpolated(Vertex from, Vertex to) {
  size_t pixelsCount = 0;

  std::abs(from.y - to.y) >= std::abs(from.x - to.x)
      ? pixelsCount = static_cast<size_t>(std::round(std::abs(from.y - to.y)))
      : pixelsCount = static_cast<size_t>(std::round(std::abs(from.x - to.x)));

  std::vector<Vertex> linePixels;
  linePixels.reserve(pixelsCount);

  Position from_ = {from.x, from.y};
  Position to_ = {to.x, to.y};

  CreateBresenhamsLine(from_, to_, linePixels);

  for (int i = 0; i < linePixels.size(); i++) {
    linePixels[i].color =
        Interpolate(from.color, to.color, std::abs(i), pixelsCount);
    linePixels[i].color = program->fragment_shader(linePixels[i]);

    canvas.SetPixel(linePixels[i].x, linePixels[i].y, linePixels[i].color);
  }

  return linePixels;
}
} // namespace core