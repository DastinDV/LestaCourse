#pragma once

#include "../Renderers/glRenderer.hxx"
#include "common.hxx"
#include <vector>

class Mirror {
public:
  Mirror(Tile &mirror, std::vector<Tile> &tiles);

  void Update(float dt);
  void Reflect(std::vector<Tile> &tiles);
  void SetMapSizeInTiles(int mapWidth, int mapHeight);

private:
  int mapWidth;
  int mapHeight;

  bool isReflect;
  float accumulateTime;
  bool forwardMove = true;
  Tile mirror;
  std::vector<Tile> &tiles;
};