#pragma once

#include "../Renderers/glRenderer.hxx"
#include "vertexBuffer.hxx"

#include "common.hxx"
#include "player.hxx"
#include <vector>

class Mirror {
public:
  Mirror(Tile &mirror, std::vector<Tile> &tiles, Player *player);

  void Update(float dt, std::vector<Mirror *> &mirrors);
  void Reflect(std::vector<Tile> &tiles);
  void SetMapSizeInTiles(int mapWidth, int mapHeight);
  int GetRadius() const;
  void SetRadius(const int radius);

  std::pair<int, int> GetTilePos() const;
  void SetTilePos(int posX, int posY);

private:
  int mapWidth;
  int mapHeight;

  int tilePosX;
  int tilePosY;

  int radius = 1;

  bool isReflect = false;
  float accumulateTime;
  bool forwardMove = true;
  Tile mirror;
  std::vector<Tile> &tiles;
  Player *player;
  ETileType mirrorType; // Vertical Horizontal
};