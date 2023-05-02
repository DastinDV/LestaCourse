#pragma once
#include "../canvas.hxx"

namespace core {
class LineRenderer {
public:
  LineRenderer(Canvas &canvas);

  std::vector<Vertex> Draw(Position from, Position to, Color color);
  std::vector<Vertex> DrawInterpolated(Vertex from, Vertex to);
  Color Interpolate(Color from, Color to, int curr, int count);

  void SetGFXProgram(gfx_program *program);

private:
  Canvas &canvas;

  void CreateBresenhamsLine(Position &from, Position &to,
                            std::vector<Vertex> &pixels);

  gfx_program *program = nullptr;
};
} // namespace core