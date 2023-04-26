#include "triangleRenderer.hxx"
#include <iostream>

namespace core {
TriangleRenderer::TriangleRenderer(Canvas &canvas) : canvas(canvas) {
  lineRenderer = std::make_unique<LineRenderer>(canvas);
}

void TriangleRenderer::Rasterize(int xLeft, int xRight, int y, Color color) {
  if (xLeft == xRight)
    return;
  lineRenderer->Draw({xLeft, y}, {xRight, y}, color);
}
// Sequence buffer
void TriangleRenderer::Draw(const std::vector<Position> &vertexes,
                            const Color color, bool isFilled) {
  for (int i = 0; i < vertexes.size() / 3; i++) {

    std::array<Position, 3> nextTriangle = {
        vertexes[i * 3 + 0], vertexes[i * 3 + 1], vertexes[i * 3 + 2]};

    std::sort(nextTriangle.begin(), nextTriangle.end(),
              [](const Position &left, const Position &right) {
                return left.y < right.y;
              });

    auto line1 = lineRenderer->Draw(nextTriangle[0], nextTriangle[1], color);
    auto line2 = lineRenderer->Draw(nextTriangle[1], nextTriangle[2], color);
    auto line3 = lineRenderer->Draw(nextTriangle[2], nextTriangle[0], color);

    if (isFilled) {
      int highY = nextTriangle[0].y;
      int midY = nextTriangle[1].y;
      int lowY = nextTriangle[2].y;

      for (int y = highY + 1; y < midY; y++) {
        auto startLeft =
            find_if(line1.begin(), line1.end(),
                    [y](const Position &nextPos) { return nextPos.y == y; });
        auto startRight =
            find_if(line3.begin(), line3.end(),
                    [y](const Position &nextPos) { return nextPos.y == y; });

        std::cout << "Up " << startLeft->x << " " << startRight->x << " " << y
                  << std::endl;
        Rasterize(startLeft->x, startRight->x, y, {255, 0, 0});
      }

      for (int y = midY + 1; y < lowY; y++) {
        auto startLeft =
            find_if(line2.begin(), line2.end(),
                    [y](const Position &nextPos) { return nextPos.y == y; });
        auto startRight =
            find_if(line3.begin(), line3.end(),
                    [y](const Position &nextPos) { return nextPos.y == y; });

        std::cout << "Bottom " << startLeft->x << " " << startRight->x << " "
                  << y << std::endl;
        Rasterize(startLeft->x, startRight->x, y, {0, 0, 255});
      }
    }
  }
}

// Indexed buffer
void TriangleRenderer::Draw(const std::vector<Position> &vertexes,
                            const std::vector<int> &indexes,
                            const Color color) {

  for (int i = 0; i < indexes.size() / 3; i++) {
    std::cout << indexes[i * 3 + 0] << " " << indexes[i * 3 + 1] << " "
              << indexes[i * 3 + 2] << std::endl;
    lineRenderer->Draw(vertexes[indexes[i * 3 + 0]],
                       vertexes[indexes[i * 3 + 1]], color);
    lineRenderer->Draw(vertexes[indexes[i * 3 + 1]],
                       vertexes[indexes[i * 3 + 2]], color);
    lineRenderer->Draw(vertexes[indexes[i * 3 + 2]],
                       vertexes[indexes[i * 3 + 0]], color);
  }
}

} // namespace core