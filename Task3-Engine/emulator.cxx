#include "emulator.hxx"
#include <stdexcept>

namespace engine_sdl {
void EventEmulatorI::AddKeyBinding(std::string emulatedKeyName,
                                   KeyBind newKeyEvent) {
  this->keyNameToEvent[emulatedKeyName] = newKeyEvent;
}

std::string EventEmulatorI::GetKeyBindInfo(std::string keyCodeName) {
  if (!keyNameToEvent.count(keyCodeName))
    return "";

  return keyNameToEvent.at(keyCodeName).bindedKeyCodeName;
}

EventEmulatorI::~EventEmulatorI() = default;

SonyPSEventEmulator::SonyPSEventEmulator() { Init(); }
void SonyPSEventEmulator::Init() {
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_w),
                {SDL_KeyCode::SDLK_w, "Up"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_s),
                {SDL_KeyCode::SDLK_s, "Down"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_a),
                {SDL_KeyCode::SDLK_a, "Left"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_d),
                {SDL_KeyCode::SDLK_d, "Right"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_x),
                {SDL_KeyCode::SDLK_x, "Cross"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_v),
                {SDL_KeyCode::SDLK_v, "Triangle"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_c),
                {SDL_KeyCode::SDLK_c, "Circle"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_z),
                {SDL_KeyCode::SDLK_z, "Square"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_SPACE),
                {SDL_KeyCode::SDLK_SPACE, "Share"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_ESCAPE),
                {SDL_KeyCode::SDLK_ESCAPE, "PS"});
  AddKeyBinding(SDL_GetKeyName(SDL_KeyCode::SDLK_o),
                {SDL_KeyCode::SDLK_o, "Options"});
}
} // namespace engine_sdl
