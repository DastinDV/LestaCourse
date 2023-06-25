#include "triangleRenderer.hxx"
#include <iostream>
#include <set>
#include <unordered_map>
#include <array>
#include <algorithm>

namespace core {
TriangleRenderer::TriangleRenderer(Canvas &canvas) : canvas(canvas) {
  lineRenderer = std::make_unique<LineRenderer>(canvas);
}

void TriangleRenderer::SetGFXProgram(gfx_program *program) {
  this->program = program;
  lineRenderer->SetGFXProgram(program);
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
  lineRenderer->DrawInterpolated(Vertex{(double)xLeft, (double)y, fromColor},
                                 Vertex{(double)xRight, (double)y, toColor});
}

// Sequence buffer
void TriangleRenderer::Draw(std::vector<Vertex> &vertexes, const Color color,
                            BitFlag settings) {
  for (int i = 0; i < vertexes.size() / 3; i++) {

    Vertex &v0_ = vertexes[i * 3 + 0];
    Vertex &v1_ = vertexes[i * 3 + 1];
    Vertex &v2_ = vertexes[i * 3 + 2];

    std::array<Vertex, 3> nextTriangle;
    if (program) {
      // canvas.Clear({0, 0, 0});
      v0_ = program->vertex_shader(vertexes[i * 3 + 0]);
      v1_ = program->vertex_shader(vertexes[i * 3 + 1]);
      v2_ = program->vertex_shader(vertexes[i * 3 + 2]);

      // v0_ = v0;
      // v1_ = v1;
      // v2_ = v2;
    }

    nextTriangle[0] = v0_;
    nextTriangle[1] = v1_;
    nextTriangle[2] = v2_;

    std::sort(nextTriangle.begin(), nextTriangle.end(),
              [](const Vertex &left, const Vertex &right) {
                return left.y < right.y;
              });

    auto line1 = lineRenderer->DrawInterpolated(
        Vertex{nextTriangle[0].x, nextTriangle[0].y, nextTriangle[1].color},
        Vertex{nextTriangle[1].x, nextTriangle[1].y, nextTriangle[0].color});
    auto line2 = lineRenderer->DrawInterpolated(
        Vertex{nextTriangle[1].x, nextTriangle[1].y, nextTriangle[2].color},
        Vertex{nextTriangle[2].x, nextTriangle[2].y, nextTriangle[1].color});
    auto line3 = lineRenderer->DrawInterpolated(
        Vertex{nextTriangle[2].x, nextTriangle[2].y, nextTriangle[0].color},
        Vertex{nextTriangle[0].x, nextTriangle[0].y, nextTriangle[2].color});

    // auto line1 =
    //     lineRenderer->Draw({nextTriangle[0].x, nextTriangle[0].y},
    //                        {nextTriangle[1].x, nextTriangle[1].y}, color);
    // auto line2 =
    //     lineRenderer->Draw({nextTriangle[1].x, nextTriangle[1].y},
    //                        {nextTriangle[2].x, nextTriangle[2].y}, color);
    // auto line3 =
    //     lineRenderer->Draw({nextTriangle[2].x, nextTriangle[2].y},
    //                        {nextTriangle[0].x, nextTriangle[0].y}, color);

    std::unordered_map<int, std::set<Vertex>> yToX;

    if (settings.HasFlag(ETriangleSettings::RASTERIZED)) {
      for (auto &pixel : line1)
        yToX[pixel.y].insert(pixel);
      for (auto &pixel : line2)
        yToX[pixel.y].insert(pixel);
      for (auto &pixel : line3)
        yToX[pixel.y].insert(pixel);

      double highY = nextTriangle[0].y;
      double midY = nextTriangle[1].y;
      double lowY = nextTriangle[2].y;

      for (int y = highY + 1; y <= midY; y++) {

        auto startLeft = yToX.at(y).begin();
        auto startRight = prev(yToX.at(y).end());
        // std::cout << "Up " << startLeft->x << " " << startRight->x << " "
        //<<
        // y
        //           << std::endl;

        !settings.HasFlag(ETriangleSettings::INTERPOLATED)
            ? Rasterize(startLeft->x, startRight->x, y, color)
            : RasterizeInterpolated(startLeft->x, startRight->x, y,
                                    startRight->color, startLeft->color);
      }

      for (int y = midY + 1; y < lowY; y++) {
        auto startLeft = yToX.at(y).begin();
        auto startRight = prev(yToX.at(y).end());

        // std::cout << "Bottom " << startLeft->x << " " << startRight->x <<
        //" "
        //<< y << std::endl;

        !settings.HasFlag(ETriangleSettings::INTERPOLATED)
            ? Rasterize(startLeft->x, startRight->x, y, color)
            : RasterizeInterpolated(startLeft->x, startRight->x, y,
                                    startRight->color, startLeft->color);
      }
    }
  }
}

// Indexed buffer
void TriangleRenderer::Draw(std::vector<Vertex> &vertexes,
                            const std::vector<int> &indexes, const Color color,
                            BitFlag settings) {

  for (int i = 0; i < indexes.size() / 3; i++) {
    // std::cout << indexes[i * 3 + 0] << " " << indexes[i * 3 + 1] << " "
    //<< indexes[i * 3 + 2] << std::endl;

    Vertex &v0_ = vertexes[indexes[i * 3 + 0]];
    Vertex &v1_ = vertexes[indexes[i * 3 + 1]];
    Vertex &v2_ = vertexes[indexes[i * 3 + 2]];

    std::array<Vertex, 3> nextTriangle;
    if (program) {
      // canvas.Clear({0, 0, 0});
      v0_ = program->vertex_shader(vertexes[indexes[i * 3 + 0]]);
      v1_ = program->vertex_shader(vertexes[indexes[i * 3 + 1]]);
      v2_ = program->vertex_shader(vertexes[indexes[i * 3 + 2]]);
      // v0_ = v0;
      // v1_ = v1;
      // v2_ = v2;
    }

    nextTriangle[0] = v0_;
    nextTriangle[1] = v1_;
    nextTriangle[2] = v2_;

    // Position p0 = {nextTriangle[0].x, nextTriangle[0].y};
    // Position p1 = {nextTriangle[1].x, nextTriangle[1].y};
    // Position p2 = {nextTriangle[2].x, nextTriangle[2].y};

    std::sort(nextTriangle.begin(), nextTriangle.end(),
              [](const Vertex &left, const Vertex &right) {
                return left.y < right.y;
              });

    auto line1 = lineRenderer->DrawInterpolated(
        Vertex{nextTriangle[0].x, nextTriangle[0].y, nextTriangle[1].color},
        Vertex{nextTriangle[1].x, nextTriangle[1].y, nextTriangle[0].color});
    auto line2 = lineRenderer->DrawInterpolated(
        Vertex{nextTriangle[1].x, nextTriangle[1].y, nextTriangle[2].color},
        Vertex{nextTriangle[2].x, nextTriangle[2].y, nextTriangle[1].color});
    auto line3 = lineRenderer->DrawInterpolated(
        Vertex{nextTriangle[2].x, nextTriangle[2].y, nextTriangle[0].color},
        Vertex{nextTriangle[0].x, nextTriangle[0].y, nextTriangle[2].color});

    // lineRenderer->Draw(p0, p1, color);
    // lineRenderer->Draw(p1, p2, color);
    // lineRenderer->Draw(p2, p0, color);

    std::unordered_map<int, std::set<Vertex>> yToX;

    if (settings.HasFlag(ETriangleSettings::RASTERIZED)) {
      for (auto &pixel : line1)
        yToX[pixel.y].insert(pixel);
      for (auto &pixel : line2)
        yToX[pixel.y].insert(pixel);
      for (auto &pixel : line3)
        yToX[pixel.y].insert(pixel);

      double highY = nextTriangle[0].y;
      double midY = nextTriangle[1].y;
      double lowY = nextTriangle[2].y;

      for (int y = highY + 1; y <= midY; y++) {

        auto startLeft = yToX.at(y).begin();
        auto startRight = prev(yToX.at(y).end());
        // std::cout << "Up " << startLeft->x << " " << startRight->x << " "
        //<<
        // y
        //           << std::endl;

        !settings.HasFlag(ETriangleSettings::INTERPOLATED)
            ? Rasterize(startLeft->x, startRight->x, y, color)
            : RasterizeInterpolated(startLeft->x, startRight->x, y,
                                    startRight->color, startLeft->color);
      }

      for (int y = midY + 1; y < lowY; y++) {
        auto startLeft = yToX.at(y).begin();
        auto startRight = prev(yToX.at(y).end());

        // std::cout << "Bottom " << startLeft->x << " " << startRight->x <<
        //" "
        //<< y << std::endl;

        !settings.HasFlag(ETriangleSettings::INTERPOLATED)
            ? Rasterize(startLeft->x, startRight->x, y, color)
            : RasterizeInterpolated(startLeft->x, startRight->x, y,
                                    startRight->color, startLeft->color);
      }
    }
  }
}

} // namespace core