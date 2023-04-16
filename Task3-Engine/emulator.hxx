
#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

namespace engine_sdl {

struct KeyBind {
  SDL_KeyCode keyCode;
  std::string bindedKeyCodeName;
};

class EventEmulatorI {
public:
  virtual void Init() = 0;
  void AddKeyBinding(std::string emulatedKeyName, KeyBind newKeyEvent);
  std::string GetKeyBindInfo(std::string keyCodeName);
  virtual ~EventEmulatorI();

private:
  std::unordered_map<std::string, KeyBind> keyNameToEvent;
};

class SonyPSEventEmulator : public EventEmulatorI {
public:
  SonyPSEventEmulator();
  void Init() override;
};

} // namespace engine_sdl