#pragma once
#include "../OpenGL/vertexBuffer.hxx"
#include "../Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"

#include <array>
#include <iostream>
#include <math.h>

static core::GlVertex blend_vertex(const core::GlVertex &vl,
                                   const core::GlVertex &vr, const float a) {
  core::GlVertex r;
  r.x = (1.0f - a) * vl.x + a * vr.x;
  r.y = (1.0f - a) * vl.y + a * vr.y;
  r.z = 0;
  return r;
}

static core::GLTriangle blend(const core::GLTriangle &tl,
                              const core::GLTriangle &tr, const float a) {
  core::GLTriangle r;
  r.v[0] = blend_vertex(tl.v[0], tr.v[0], a);
  r.v[1] = blend_vertex(tl.v[1], tr.v[1], a);
  r.v[2] = blend_vertex(tl.v[2], tr.v[2], a);
  return r;
}

static float manhatten_distance(float from, float to) {
  return std::sqrt(std::abs(std::pow(from, 2) - std::pow(to, 2)));
}

static std::pair<int, int> GetTilePosByClickPos(float x, float y, float w,
                                                float h, int screenWidth,
                                                int screenHeight,
                                                float xSizeCorrection,
                                                float ySizeCorrection) {
  std::vector<float> screenCoord = {x, y, 0.0};

  float xTranslate =
      ((640.0 * xSizeCorrection) - 640.0f) / (640 * xSizeCorrection);
  float yTranslate =
      ((480.0 * ySizeCorrection) - 480.0f) / (480.0 * ySizeCorrection);

  std::cout << "xTranslate " << xTranslate << std::endl;
  std::cout << "yTranslate " << yTranslate << std::endl;

  // Тайлы не имеют трансформа относительно своей позиции.
  std::vector<float> model = {0.0f, 0.0f, 0.0f};
  auto result = core::Unproject(screenCoord, model, xSizeCorrection,
                                ySizeCorrection, screenWidth, screenHeight);

  // Но подтягиваем для центровки экрана.
  result[0] -= (xTranslate * (640 * xSizeCorrection)) / 2.0;
  result[1] -= (yTranslate * (480 * ySizeCorrection)) / 2.0;

  result[0] /= (w);
  result[1] /= (h);
  return std::pair<int, int>(result[0], result[1]);
}

enum class ETileType { EMPTY, ROAD, EXIT, VERTICAL, HORIZONTAL, CROSS };

struct Tile {
  core::GLTriangle one;
  core::GLTriangle two;

  int i = 0;
  int j = 0;
  ETileType tileType;
  core::VertexBuffer *buf;

  std::vector<float> tilePos;
  float *vertecies;
  bool isWalkable = false;
  bool isFlicker = false;
};