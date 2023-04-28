#pragma once
#include "../canvas.hxx"
#include "lineRenderer.hxx"
#include <memory>
#include <vector>

namespace core {

enum class ETriangleSettings {
  RASTERIZED = 1 << 0,   // 1
  INTERPOLATED = 1 << 1, // 2
};

struct BitFlag {
  void SetFlag(ETriangleSettings flag) { m_FlagValue |= (int)flag; }

  // Sets flag to false
  void UnsetFlag(ETriangleSettings flag) { m_FlagValue &= ~(int)flag; }

  // Check whether a flag is set to true
  bool HasFlag(ETriangleSettings flag) {
    return (m_FlagValue & (int)flag) == (int)flag;
  }

  uint8_t m_FlagValue = 0;
};

class TriangleRenderer {
public:
  TriangleRenderer(Canvas &canvas);

  void Draw(const std::vector<Vertex> &vertexes, const Color color,
            bool isFilled, BitFlag settings);
  void Draw(const std::vector<Vertex> &vertexes,
            const std::vector<int> &indexes, const Color color);
  void Rasterize(int xLeft, int xRight, int y, const Color color);
  void RasterizeInterpolated(int xLeft, int xRight, int y,
                             const Color fromColor, const Color toColor);

private:
  std::unique_ptr<LineRenderer> lineRenderer;
  Canvas &canvas;
  BitFlag settings;
};
} // namespace core