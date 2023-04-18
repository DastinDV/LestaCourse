#include <iostream>

#include "engine.hxx"

namespace core {

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

void Engine::SetKeyBoardEmulator(EventEmulatorI *currentEmulator) {
  this->eventEmulator = currentEmulator;
}

KeyEventInfo &Engine::GetKeyEventInfo() { return this->eventInfo; }

int Engine::ProcessEvent(event &event) {
  SDL_Event sdlEvent;
  while (SDL_PollEvent(&sdlEvent)) {
    if (sdlEvent.type == SDL_EVENT_KEY_DOWN) {
      event = event::key_pressed;
      std::string keyName = SDL_GetKeyName(sdlEvent.key.keysym.sym);
      eventInfo.eventType = "Pressed";
      eventInfo.keyCode = this->eventEmulator->GetKeyBindInfo(keyName);
    }
    if (sdlEvent.type == SDL_EVENT_KEY_UP) {
      event = event::key_released;
      eventInfo.eventType = "Released";
    }
    if (sdlEvent.type == SDL_EVENT_QUIT) {
      event = event::quit;
      eventInfo.eventType = "Qiut";
    }
  }
  return 1;
}

} // namespace core