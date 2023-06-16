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
  int GetRadius() const;
  void SetRadius(const int radius);

  std::pair<int, int> GetTilePos() const;

private:
  int mapWidth;
  int mapHeight;

  int tilePosX;
  int tilePosY;

  int radius = 2;

  bool isReflect;
  float accumulateTime;
  bool forwardMove = true;
  Tile mirror;
  std::vector<Tile> &tiles;
};