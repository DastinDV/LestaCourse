#pragma once
#include "../OpenGL/shader.hxx"
#include "../Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"

#include "map.hxx"

core::VertexBuffer buffer;
core::Shader colorShader;
core::Shader roadShader;

core::VertexBuffer roadBuffer;
core::VertexBuffer mirrorsBuffer;
core::VertexBuffer exitBuffer;

float RedColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
float GreenColor[] = {0.0f, 1.0f, 0.0f, 1.0f};
float BlueColor[] = {0.0f, 0.0f, 1.0f, 1.0f};

enum class ETileType { EMPTY, ROAD, EXIT, VERTICAL, HORIZONTAL, CROSS };

struct Tile {
  core::GLTriangle one;
  core::GLTriangle two;

  ETileType tileType;
  bool isWalkable = false;
};

class MirrorGame : public core::Game {
public:
  MirrorGame();
  void Init() override;
  void Render() override;
  void Update(float deltaTime) override;
  void OnEvent(core::Event &event, float deltaTime) override;

  ~MirrorGame();

private:
  void ResizeScreen();
  void CreateTiles();
  void PushToBuffers();

  core::GlRenderer glRenderer;
  Map map;

  float targetScreenWidth = 640.0f;
  float targetScreenHeight = 480.0f;
  float nearPlain = 0.0f;
  float farPlain = 100.0f;

  int screenWidth;
  int screenHeight;
  float aspectRatio;
  float targetAR;

  float tileSize = 32.0f;
  std::vector<Tile> tiles;

  int roadTileCount = 0;
  int mirrorsTileCount = 0;
  int exitTileCount = 1;

  float *roadVertecies;
  float *exitVertecies;
  float *mirrorsVertecies;
};

IZ_DECLSPEC core::Game *CreateGame() { return new MirrorGame(); }