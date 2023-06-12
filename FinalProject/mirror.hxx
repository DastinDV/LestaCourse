#pragma once

#include "../Renderers/glRenderer.hxx"
#include "common.hxx"
#include <vector>

core::GlVertex blend_vertex(const core::GlVertex &vl, const core::GlVertex &vr,
                            const float a) {
  core::GlVertex r;
  r.x = (1.0f - a) * vl.x + a * vr.x;
  r.y = (1.0f - a) * vl.y + a * vr.y;
  r.z = 0;
  return r;
}

core::GLTriangle blend(const core::GLTriangle &tl, const core::GLTriangle &tr,
                       const float a) {
  core::GLTriangle r;
  r.v[0] = blend_vertex(tl.v[0], tr.v[0], a);
  r.v[1] = blend_vertex(tl.v[1], tr.v[1], a);
  r.v[2] = blend_vertex(tl.v[2], tr.v[2], a);
  return r;
}

class Mirror {
public:
  Mirror();

  void Update(float dt, float timeSinceRun);
  void Reflect(std::vector<Tile> &tiles, Tile &mirror);
  void SetMapSizeInTiles(int mapWidth, int mapHeight);

private:
  int mapWidth;
  int mapHeight;

  bool isReflect;
  Tile mirror;
  std::vector<Tile> tiles;
};