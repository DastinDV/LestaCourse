#pragma once
#include "../canvas.hxx"
#include "lineRenderer.hxx"
#include <memory>
#include <vector>

namespace core {
class TriangleRenderer {
public:
  TriangleRenderer(Canvas &canvas);

  void Draw(std::vector<Position> vertexes, Color color);

private:
  std::unique_ptr<LineRenderer> lineRenderer;
  Canvas &canvas;
};
} // namespace core