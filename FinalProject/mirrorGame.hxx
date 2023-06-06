#pragma once
#include "../OpenGL/shader.hxx"
#include "../Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"

#include "map.hxx"

core::VertexBuffer buffer;
core::Shader colorShader;

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
  core::GlRenderer glRenderer;
  Map map;

  float targetScreenWidth = 640.0f;
  float targetScreenHeight = 480.0f;
  float nearPlain = 0.0f;
  float farPlain = 100.0f;

  float targetAR;

  int screenWidth;
  int screenHeight;
  float aspectRatio;
};

IZ_DECLSPEC core::Game *CreateGame() { return new MirrorGame(); }