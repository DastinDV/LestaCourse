#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>

#include "canvas.hxx"
#include "engine.hxx"
#include "glad/glad.h"
#include <SDL3/SDL.h>

namespace core {

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GLContext openGLContext = nullptr;

const std::unordered_map<SDL_Keycode, KeyCode> sdlToEngineKeyBinding{
    {SDLK_w, KeyCode::w},         {SDLK_s, KeyCode::s},
    {SDLK_a, KeyCode::a},         {SDLK_d, KeyCode::d},
    {SDLK_q, KeyCode::q},         {SDLK_e, KeyCode::e},
    {SDLK_r, KeyCode::r},         {SDLK_z, KeyCode::z},
    {SDLK_x, KeyCode::x},         {SDLK_LCTRL, KeyCode::lctrl},
    {SDLK_RCTRL, KeyCode::rctrl}, {SDLK_SPACE, KeyCode::space}};

static std::ostream &operator<<(std::ostream &out, const SDL_version &v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

Engine::Engine() {}

int Engine::Initialize(int screenWidth, int screenHeight) {
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

  window = SDL_CreateWindow("title", screenWidth, screenHeight,
                            ::SDL_WINDOW_OPENGL | ::SDL_WINDOW_RESIZABLE);

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
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::ResizeViewPort(int w, int h) const {
  if (window) {
    glViewport(0, 0, w, h);
  }
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
      event.mouseInfo = {sdlEvent.motion.x, sdlEvent.motion.y,
                         MouseEventType::mouse_motion};
    }
    if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
        event.eventType = EventType::mouse_event;
        event.mouseInfo = {sdlEvent.button.x, sdlEvent.button.y,
                           MouseEventType::button_down};
      }
    }
    if (sdlEvent.type == SDL_EVENT_WINDOW_RESIZED) {
      event.eventType = EventType::window_event;
      int w, h;
      SDL_GetWindowSize(window, &w, &h);
      event.windowInfo = {SDL_GetWindowID(window), WindowEventType::resized, w,
                          h};
    }
    if (sdlEvent.type == SDL_EVENT_WINDOW_MAXIMIZED) {
      event.eventType = EventType::window_event;
      int w, h;
      SDL_GetWindowSize(window, &w, &h);
      event.windowInfo = {SDL_GetWindowID(window), WindowEventType::maximized,
                          w, h};
      return 1;
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

glm::mat4 trans;
glm::mat4 proj;

// Translate vertex to a new coordinate
float *Translate(std::vector<float> &translation) {
  float *matrix;

  trans = glm::mat4(1.0f);
  trans = glm::translate(
      trans, glm::vec3(translation[0], translation[1], translation[2]));

  matrix = glm::value_ptr(trans);
  return matrix;
}

// Rotate and scale verteces
float *Rotate(float angle) {
  float *matrix;
  trans = glm::rotate(trans, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
  // rot = glm::scale(rot, glm::vec3(0.5, 0.5, 0.5));

  matrix = glm::value_ptr(trans);
  return matrix;
}

float *Scale(float scaleFactor) {
  float *matrix;
  trans = glm::scale(trans, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

  matrix = glm::value_ptr(trans);
  return matrix;
}

// Ortographic projection with (0,0) in upper-left.
float *OrthoProj(float left, float right, float bottom, float top, float near,
                 float far) {
  float *matrix;
  proj = glm::ortho(left, right, bottom, top, near, far);

  matrix = glm::value_ptr(proj);
  return matrix;
}

std::vector<float> Unproject(std::vector<float> win, std::vector<float> model,
                             float xCorrection, float yCorrection, int width,
                             int height) {
  glm::vec3 g_win(win[0], win[1], win[2]);
  proj = glm::ortho(0.0, xCorrection * 640.0, 0.0, yCorrection * 480.0, 0.0,
                    100.0);
  trans = glm::mat4(1.0f);
  trans = glm::translate(trans, glm::vec3(model[0], model[1], model[2]));
  glm::vec4 viewport(0.0f, 0.0f, (float)width, (float)height);
  glm::vec3 result = glm::unProject(g_win, trans, proj, viewport);

  std::cout << "unproject " << result[0] << " " << result[1] << " " << result[2]
            << std::endl;

  return {result[0], result[1], result[2]};
}

std::pair<int, int> GetScreenSize() {
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  return std::pair<int, int>(w, h);
}

} // namespace core