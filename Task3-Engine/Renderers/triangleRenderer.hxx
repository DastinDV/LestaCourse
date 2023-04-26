#pragma once
#include "../canvas.hxx"
#include "lineRenderer.hxx"
#include <memory>
#include <vector>

namespace core {
class TriangleRenderer {
public:
  TriangleRenderer(Canvas &canvas);

  void Draw(const std::vector<Position> &vertexes, const Color color,
            bool isFilled);
  void Draw(const std::vector<Position> &vertexes,
            const std::vector<int> &indexes, const Color color);
  void Rasterize(int xLeft, int xRight, int y, const Color color);

private:
  std::unique_ptr<LineRenderer> lineRenderer;
  Canvas &canvas;
};
} // namespace core