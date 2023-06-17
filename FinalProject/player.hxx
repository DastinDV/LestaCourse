#pragma once
#include "../OpenGL/vertexBuffer.hxx"
#include "../Renderers/glRenderer.hxx"

#include "common.hxx"
#include "shader.hxx"

#include <vector>

class Player {
public:
  Player(float posX, float posY);

  void Init();
  void Move(std::vector<Tile> &tiles, std::vector<float> direction);
  void Render(core::GlRenderer *glRenderer);
  float *GetVertecies();

  void SetShader(core::Shader shader);

  void SetMapSizeInTiles(int mapWidth, int mapHeight);
  void SetWorldTransform(std::vector<float> worldPos);
  void SetXSizeCorrection(float factor);
  void SetYSizeCorrection(float factor);

  void AdjustWorldCoordinates();
  int GetXTilePos();
  int GetYTilePos();

private:
  core::VertexBuffer buf;
  std::vector<float> localPos = {0.0, 0.0, 0.0};
  std::vector<float> worldPos = {0.0, 0.0, 0.0};

  int tilePosX;
  int tilePosY;

  int mapWidth;
  int mapHeight;

  float tileSizeX = 32.0;
  float tileSizeY = 32.0;

  float xSizeCorrection = 1.0;
  float ySizeCorrection = 1.0;
  float scale = 0.5;

  float *playerVertecies;
  std::vector<float> direction;
  core::Shader shader;
};