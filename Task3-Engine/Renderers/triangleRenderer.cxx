#include "triangleRenderer.hxx"
#include <iostream>
#include <set>
#include <unordered_map>

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
  lineRenderer->DrawInterpolated(Vertex{{xLeft, y}, fromColor},
                                 Vertex{{xRight, y}, toColor});
}
// Sequence buffer
void TriangleRenderer::Draw(const std::vector<Vertex> &vertexes,
                            const Color color, bool isFilled,
                            BitFlag settings) {
  for (int i = 0; i < vertexes.size() / 3; i++) {

    std::array<Vertex, 3> nextTriangle = {
        vertexes[i * 3 + 0], vertexes[i * 3 + 1], vertexes[i * 3 + 2]};

    std::sort(nextTriangle.begin(), nextTriangle.end(),
              [](const Vertex &left, const Vertex &right) {
                return left.pos.y < right.pos.y;
              });

    auto line1 = lineRenderer->DrawInterpolated(
        Vertex{nextTriangle[0].pos, nextTriangle[1].color},
        Vertex{nextTriangle[1].pos, nextTriangle[0].color});
    auto line2 = lineRenderer->DrawInterpolated(
        Vertex{nextTriangle[1].pos, nextTriangle[2].color},
        Vertex{nextTriangle[2].pos, nextTriangle[1].color});
    auto line3 = lineRenderer->DrawInterpolated(
        Vertex{nextTriangle[2].pos, nextTriangle[0].color},
        Vertex{nextTriangle[0].pos, nextTriangle[2].color});

    std::unordered_map<int, std::set<Vertex>> yToX;

    for (const Vertex &pixel : line1)
      yToX[pixel.pos.y].insert(pixel);
    for (auto &pixel : line2)
      yToX[pixel.pos.y].insert(pixel);
    for (auto &pixel : line3)
      yToX[pixel.pos.y].insert(pixel);
    // auto line1 =
    //     lineRenderer->Draw(nextTriangle[0].pos, nextTriangle[1].pos,
    //     color);
    // auto line2 =
    //     lineRenderer->Draw(nextTriangle[1].pos, nextTriangle[2].pos,
    //     color);
    // auto line3 =
    //     lineRenderer->Draw(nextTriangle[2].pos, nextTriangle[0].pos,
    //     color);

    if (settings.HasFlag(ETriangleSettings::RASTERIZED)) {
      int highY = nextTriangle[0].pos.y;
      int midY = nextTriangle[1].pos.y;
      int lowY = nextTriangle[2].pos.y;

      for (int y = highY + 1; y <= midY; y++) {

        auto startLeft = yToX.at(y).begin();
        auto startRight = prev(yToX.at(y).end());
        // std::cout << "Up " << startLeft->x << " " << startRight->x << " " <<
        // y
        //           << std::endl;

        !settings.HasFlag(ETriangleSettings::INTERPOLATED)
            ? Rasterize(startLeft->pos.x, startRight->pos.x, y, {255, 0, 0})
            : RasterizeInterpolated(startLeft->pos.x, startRight->pos.x, y,
                                    startRight->color, startLeft->color);
      }

      for (int y = midY + 1; y < lowY; y++) {
        auto startLeft = yToX.at(y).begin();
        auto startRight = prev(yToX.at(y).end());

        // std::cout << "Bottom " << startLeft->x << " " << startRight->x << " "
        //<< y << std::endl;

        !settings.HasFlag(ETriangleSettings::INTERPOLATED)
            ? Rasterize(startLeft->pos.x, startRight->pos.x, y, {0, 0, 255})
            : RasterizeInterpolated(startLeft->pos.x, startRight->pos.x, y,
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