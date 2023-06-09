#pragma once
#include "../OpenGL/shader.hxx"
#include "../Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"
// #include "shader.hxx"
// #include "glRenderer.hxx"
// #include "engine.hxx"
// #include "game.hxx"

#include "common.hxx"
#include "map.hxx"
#include "mirror.hxx"
#include "player.hxx"

core::Shader roadShader;
core::Shader exitShader;
core::Shader mirrorShader;
core::Shader playerShader;

float RedColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
float GreenColor[] = {0.0f, 1.0f, 0.0f, 1.0f};
float BlueColor[] = {0.0f, 0.0f, 1.0f, 1.0f};
float NeonBlueColor[] = {0.0, 1.0f, 1.0f, 1.0f};
float NeonGoldColor[] = {255.0, 255.0, 1.0, 1.0f};

float u_screenSize[2] = {0.0f, 0.0f};
float u_exitPos[2] = {0.0f, 0.0f};

std::vector<float> u_defaultTransform = {0.0f, 0.0f, 0.0f};

float u_tileSize = 32.0f;

class MirrorGame : public core::Game {
public:
  MirrorGame();
  void Init() override;
  void Render() override;
  void Update(float deltaTime) override;
  void OnEvent(core::Event &event, float deltaTime) override;
  bool IsGameEnd() override;

  ~MirrorGame();

private:
  void CreateTiles();
  void CreateShaders();
  void CreatePlayer(Tile playerTile);
  void CreateMirror(Tile mirrorTile);

  void LoadLVL(int lvl);
  void HighlightTiles(Mirror *mirror);
  void PushToBuffers();
  void ResizeScreen();
  void InitUniforms();
  void InitSounds();

  void CleanUpResources();
  core::GlRenderer *glRenderer;
  Map map;
  GameState gameState;

  core::VertexBuffer testBuffer;
  float *testVertecies;

  int currentLVL = 1;

  float targetScreenWidth = 640.0f;
  float targetScreenHeight = 480.0f;
  float nearPlain = 0.0f;
  float farPlain = 100.0f;

  int screenWidth;
  int screenHeight;
  float aspectRatio;
  float targetAR;
  float factor;
  float xSizeCorrection = 1.0;
  float ySizeCorrection = 1.0;

  float tileSize = 32.0f;
  std::vector<Tile> tiles;

  int roadTileCount = 0;
  int mirrorsTileCount = 0;
  int exitTileCount = 1;

  float *exitVertecies;
  Player *player = nullptr;
  Mirror *currentMirror = nullptr;

  std::vector<Mirror *> mirrors;

  float timeSinceRun = 0.0;
  const std::string pathToMaps = "./../../FinalProject/Assets/map";

  core::SoundBuffer *reflectionSound = nullptr;
  core::SoundBuffer *winSound = nullptr;

  const int lvlCount = 6;
  bool isGameEnd = false;
};

IZ_DECLSPEC core::Game *CreateGame() { return new MirrorGame(); }