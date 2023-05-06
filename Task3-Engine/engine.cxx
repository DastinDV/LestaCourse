#include <cmath>
#include <iostream>
#include <unordered_map>

#include "./glad/include/glad/glad.h"
#include "canvas.hxx"
#include "engine.hxx"
#include <SDL3/SDL.h>

namespace core {

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GLContext openGLContext = nullptr;

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

  int GlMajorVer = 3;
  int GlMinorVer = 2;

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

  window = SDL_CreateWindow("title", 640, 480, ::SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call SDL_CreateWindow: " << err_message << endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GlMajorVer);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GlMinorVer);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, GlMinorVer);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  openGLContext = SDL_GL_CreateContext(window);
  if (openGLContext == nullptr) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call SDL_GL_CreateContext: " << err_message << endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  if (!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress)) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call gladLoadGLES2Loader: " << err_message << endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);

  if (renderer == nullptr) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call SDL_CreateRenderer: " << err_message << endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  GetOpenGLVersionInfo();

  return EXIT_SUCCESS;
}

void Engine::ClearScreen(float deltaTime) {
  auto current_color = std::sin(0.5f * deltaTime);
  std::cout << current_color << std::endl;
  glClearColor(current_color, 1 - current_color, current_color, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::GetOpenGLVersionInfo() {
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
}

void Engine::SwapBuffers() { SDL_GL_SwapWindow(window); }

KeyboardInfo KeyBinding(SDL_Keycode from, KeyboardEventType keyboardEvent) {
  KeyCode keyCode = sdlToEngineKeyBinding.count(from)
                        ? sdlToEngineKeyBinding.at(from)
                        : KeyCode::not_bind;
  return {SDL_GetKeyName(from), keyCode, keyboardEvent};
}

int Engine::ProcessEvent(Event &event) {
  SDL_Event sdlEvent;
  while (SDL_PollEvent(&sdlEvent)) {
    if (sdlEvent.type == SDL_EVENT_KEY_DOWN) {
      event.eventType = EventType::keyboard_event;
      event.keyBoardInfo =
          KeyBinding(sdlEvent.key.keysym.sym, KeyboardEventType::key_pressed);
    }
    if (sdlEvent.type == SDL_EVENT_KEY_UP) {
      event.eventType = EventType::keyboard_event;
      event.keyBoardInfo =
          KeyBinding(sdlEvent.key.keysym.sym, KeyboardEventType::key_released);
    }
    if (sdlEvent.type == SDL_EVENT_MOUSE_MOTION) {
      event.eventType = EventType::mouse_event;
      event.mouseInfo = {sdlEvent.motion.x, sdlEvent.motion.y};
    }
    if (sdlEvent.type == SDL_EVENT_QUIT) {
      event.eventType = EventType::quit;
    }
  }
  return 1;
}

int Engine::CleanUp() {
  SDL_GL_DeleteContext(openGLContext);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
} // namespace core