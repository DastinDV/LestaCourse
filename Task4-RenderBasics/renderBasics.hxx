#pragma once
#include "../Task3-Engine/Renderers/lineRenderer.hxx"
#include "../Task3-Engine/Renderers/triangleRenderer.hxx"
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"

#include <functional>
#include <memory>

class RenderBasicsGame : public core::Game {
public:
  RenderBasicsGame();
  void Render() override;
  void Update() override;
  void Init() override;
  void OnEvent(core::Event &event) override;

private:
  std::unique_ptr<core::LineRenderer> lineRenderer;
  std::unique_ptr<core::TriangleRenderer> triangleRenderer;
  std::unique_ptr<core::Canvas> canvas;
  std::vector<std::function<void()>> tests;

  int width = 640;
  int height = 480;
  void InitTestFunctions();
};

IZ_DECLSPEC core::Game *CreateGame() { return new RenderBasicsGame(); }