#pragma once
#include "../OpenGL/vertexBuffer.hxx"
#include "../Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"

#include <array>
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