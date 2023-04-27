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

void TriangleRenderer::RasterizeInterpolated(int xLeft, int xRight, int y,
                                             const Color fromColor,
                                             const Color toColor) {
  if (xLeft == xRight)
    return;
  lineRenderer->DrawInterpolated({xLeft, y}, {xRight, y}, fromColor, toColor);
}
// Sequence buffer
void TriangleRenderer::Draw(const std::vector<Vertex> &vertexes,
                            const Color color, bool isFilled) {
  for (int i = 0; i < vertexes.size() / 3; i++) {

    std::array<Vertex, 3> nextTriangle = {
        vertexes[i * 3 + 0], vertexes[i * 3 + 1], vertexes[i * 3 + 2]};

    std::sort(nextTriangle.begin(), nextTriangle.end(),
              [](const Vertex &left, const Vertex &right) {
                return left.pos.y < right.pos.y;
              });

    auto line1 = lineRenderer->DrawInterpolated(
        nextTriangle[0].pos, nextTriangle[1].pos, nextTriangle[1].color,
        nextTriangle[0].color);
    auto line2 = lineRenderer->DrawInterpolated(
        nextTriangle[1].pos, nextTriangle[2].pos, nextTriangle[2].color,
        nextTriangle[1].color);
    auto line3 = lineRenderer->DrawInterpolated(
        nextTriangle[2].pos, nextTriangle[0].pos, nextTriangle[0].color,
        nextTriangle[2].color);

    // auto line1 =
    //     lineRenderer->Draw(nextTriangle[0].pos, nextTriangle[1].pos, color);
    // auto line2 =
    //     lineRenderer->Draw(nextTriangle[1].pos, nextTriangle[2].pos, color);
    // auto line3 =
    //     lineRenderer->Draw(nextTriangle[2].pos, nextTriangle[0].pos, color);

    if (isFilled) {
      int highY = nextTriangle[0].pos.y;
      int midY = nextTriangle[1].pos.y;
      int lowY = nextTriangle[2].pos.y;

      for (int y = highY + 1; y < midY; y++) {
        auto startLeft =
            find_if(line1.begin(), line1.end(),
                    [y](const Vertex &nextPos) { return nextPos.pos.y == y; });
        auto startRight =
            find_if(line3.begin(), line3.end(),
                    [y](const Vertex &nextPos) { return nextPos.pos.y == y; });

        // std::cout << "Up " << startLeft->x << " " << startRight->x << " " <<
        // y
        //<< std::endl;

        // Rasterize(startLeft->pos.x, startRight->pos.x, y, {255, 0, 0});
        RasterizeInterpolated(startLeft->pos.x, startRight->pos.x, y,
                              startRight->color, startLeft->color);
      }

      for (int y = midY + 1; y < lowY; y++) {
        auto startLeft =
            find_if(line2.begin(), line2.end(),
                    [y](const Vertex &nextPos) { return nextPos.pos.y == y; });
        auto startRight =
            find_if(line3.begin(), line3.end(),
                    [y](const Vertex &nextPos) { return nextPos.pos.y == y; });

        // std::cout << "Bottom " << startLeft->x << " " << startRight->x << " "
        //<< y << std::endl;

        // Rasterize(startLeft->x, startRight->x, y, {0, 0, 255});
        RasterizeInterpolated(startLeft->pos.x, startRight->pos.x, y,
                              startRight->color, startLeft->color);
      }
    }
  }
}

// Indexed buffer
void TriangleRenderer::Draw(const std::vector<Vertex> &vertexes,
                            const std::vector<int> &indexes,
                            const Color color) {

  for (int i = 0; i < indexes.size() / 3; i++) {
    // std::cout << indexes[i * 3 + 0] << " " << indexes[i * 3 + 1] << " "
    //<< indexes[i * 3 + 2] << std::endl;
    lineRenderer->Draw(vertexes[indexes[i * 3 + 0]].pos,
                       vertexes[indexes[i * 3 + 1]].pos, color);
    lineRenderer->Draw(vertexes[indexes[i * 3 + 1]].pos,
                       vertexes[indexes[i * 3 + 2]].pos, color);
    lineRenderer->Draw(vertexes[indexes[i * 3 + 2]].pos,
                       vertexes[indexes[i * 3 + 0]].pos, color);
  }
}

} // namespace core