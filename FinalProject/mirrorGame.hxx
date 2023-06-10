#pragma once
#include "../OpenGL/shader.hxx"
#include "../Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"

#include "map.hxx"

core::Shader roadShader;
core::Shader exitShader;
core::Shader mirrorShader;

std::vector<core::VertexBuffer *> roadBuffers;
core::VertexBuffer mirrorsBuffer;
core::VertexBuffer exitBuffer;

float RedColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
float GreenColor[] = {0.0f, 1.0f, 0.0f, 1.0f};
float BlueColor[] = {0.0f, 0.0f, 1.0f, 1.0f};
float u_screenSize[2] = {0.0f, 0.0f};
float u_exitPos[2] = {0.0f, 0.0f};
float u_mirrorPos[2] = {0.0f, 0.0f};
std::vector<std::vector<float>> u_roadPos;

float u_tileSize = 32.0f;

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
  void InitUniforms();

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

  std::vector<float *> roadVertecies;
  float *exitVertecies;
  float *mirrorsVertecies;

  float timeSinceRun = 0.0;
};

IZ_DECLSPEC core::Game *CreateGame() { return new MirrorGame(); }