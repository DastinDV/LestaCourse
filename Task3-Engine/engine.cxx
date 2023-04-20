#include <iostream>
#include <unordered_map>

#include "engine.hxx"
#include <SDL3/SDL.h>

namespace core {

const std::unordered_map<SDL_Keycode, KeyCode> sdlToEngineKeyBinding{
    {SDLK_w, KeyCode::w},         {SDLK_s, KeyCode::w},
    {SDLK_a, KeyCode::w},         {SDLK_d, KeyCode::w},
    {SDLK_LCTRL, KeyCode::lctrl}, {SDLK_RCTRL, KeyCode::rctrl},
    {SDLK_SPACE, KeyCode::space}};

static std::ostream &operator<<(std::ostream &out, const SDL_version &v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

Engine::Engine() {}

int Engine::Initialize() {
  using namespace std;

  SDL_version compiled = {0, 0, 0};
  SDL_version linked = {0, 0, 0};

  SDL_VERSION(&compiled)
  SDL_GetVersion(&linked);

  if (SDL_COMPILEDVERSION !=
      SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
    cerr << "warning: SDL2 compiled and linked version mismatch: " << compiled
         << " " << linked << endl;
  }

  const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
  if (init_result != 0) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call SDL_Init: " << err_message << endl;
    return EXIT_FAILURE;
  }

  SDL_Window *const window =
      SDL_CreateWindow("title", 640, 480, ::SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call SDL_CreateWindow: " << err_message << endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int Engine::ProcessEvent(Event &event) {
  SDL_Event sdlEvent;
  while (SDL_PollEvent(&sdlEvent)) {
    if (sdlEvent.type == SDL_EVENT_KEY_DOWN) {
      event.eventType = EventType::keyboard_event;
      KeyboardInfo info{SDL_GetKeyName(sdlEvent.key.keysym.sym),
                        sdlToEngineKeyBinding.at(sdlEvent.key.keysym.sym),
                        KeyboardEventType::key_pressed};
      event.keyBoardInfo = info;
    }
    if (sdlEvent.type == SDL_EVENT_KEY_UP) {
      event.eventType = EventType::keyboard_event;
      KeyboardInfo info{SDL_GetKeyName(sdlEvent.key.keysym.sym),
                        sdlToEngineKeyBinding.at(sdlEvent.key.keysym.sym),
                        KeyboardEventType::key_released};
      event.keyBoardInfo = info;
    }
    if (sdlEvent.type == SDL_EVENT_QUIT) {
      event.eventType = EventType::quit;
    }
  }
  return 1;
}

} // namespace core