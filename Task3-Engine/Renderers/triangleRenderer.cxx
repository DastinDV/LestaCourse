#include "triangleRenderer.hxx"

namespace core {
TriangleRenderer::TriangleRenderer(Canvas &canvas) : canvas(canvas) {
  lineRenderer = std::make_unique<LineRenderer>(canvas);
}

void TriangleRenderer::Draw(std::vector<Position> vertexes, Color color) {
  for (int i = 0; i < vertexes.size() / 3; i++) {
    lineRenderer->Draw(vertexes[i * 3 + 0], vertexes[i * 3 + 1], color);
    lineRenderer->Draw(vertexes[i * 3 + 1], vertexes[i * 3 + 2], color);
    lineRenderer->Draw(vertexes[i * 3 + 2], vertexes[i * 3 + 0], color);
  }
}

} // namespace core