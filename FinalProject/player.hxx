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
  void SetTransform(std::vector<float> pos);

private:
  core::VertexBuffer buf;
  std::vector<float> pos = {0.0, 0.0, 0.0};

  int tilePosX;
  int tilePosY;

  int mapWidth;
  int mapHeight;

  float tileSize = 32.0;
  float *playerVertecies;
  std::vector<float> direction;
  core::Shader shader;
};