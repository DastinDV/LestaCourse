#pragma once
#include "../canvas.hxx"

namespace core {
class LineRenderer {
public:
  LineRenderer(Canvas &canvas);

  void Draw(Position from, Position to, Color color);

private:
  Canvas &canvas;
};
} // namespace core