#pragma once
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"

#include "map.hxx"

class MirrorGame : public core::Game {
public:
  MirrorGame();
  void Init() override;
  void Render() override;
  void Update(float deltaTime) override;
  void OnEvent(core::Event &event, float deltaTime) override;
  ~MirrorGame();

private:
  Map map;
};

IZ_DECLSPEC core::Game *CreateGame() { return new MirrorGame(); }