#pragma once
#include "engine.hxx"

namespace core {

class Game {
public:
  virtual void Init() = 0;
  virtual void Render() = 0;
  virtual void Update() = 0;
  virtual void OnEvent(event &event) = 0;
};
// namespace core

Game *ReloadGame(Game *old, const char *library_name,
                 const char *tmp_library_name, Engine &engine,
                 void *&old_handle);
} // namespace core

extern "C" core::Game *CreateGame();