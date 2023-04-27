#pragma once
#include "../canvas.hxx"

namespace core {
class LineRenderer {
public:
  LineRenderer(Canvas &canvas);

  std::vector<Position> Draw(Position from, Position to, Color color);
  std::vector<Vertex> DrawInterpolated(Position from, Position to,
                                       Color colorFrom, Color colorTo);
  Color Interpolate(Color from, Color to, int curr, int count);

private:
  Canvas &canvas;
};
} // namespace core