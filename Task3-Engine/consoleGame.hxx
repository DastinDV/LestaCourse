#pragma once
#include "engine.hxx"
#include "game.hxx"
#include <array>

class ConsoleGame : public core::Game {
public:
  ConsoleGame();
  void Render() override;
  void Update() override;
  void Init() override;
  void OnEvent(core::event &event) override;

private:
  int16_t percentage;
  int PBWIDTH = 60;

  void PrintProgress(int percentage);
};

IZ_DECLSPEC core::Game *CreateGame() { return new ConsoleGame(); }