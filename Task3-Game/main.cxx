#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string_view>

#if __has_include(<SDL.h>)
#include <SDL.h>
#else
#include <SDL3/SDL.h>
#endif

std::ostream &operator<<(std::ostream &out, const SDL_version &v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

#pragma pack(push, 4)
struct bind {
  SDL_Keycode key;
  std::string_view name;
};
#pragma pack(pop)

void printKeyInfo(SDL_Event &event) {
  /* Is it a release or a press? */
  if (event.key.type == SDL_EVENT_KEY_UP)
    printf("Release:- ");
  else
    printf("Press:- ");

  /* Print the hardware scancode first */
  printf("Scancode: 0x%02X", event.key.keysym.scancode);
  /* Print the name of the key */
  printf(", Name: %s", SDL_GetKeyName(event.key.keysym.sym));
  printf("\n");
  /* Print modifier info */
}

int processEvent(SDL_Event &event) {

  switch (event.type) {
  case SDL_EVENT_KEY_DOWN:
    printKeyInfo(event);
    break;
  case SDL_EVENT_QUIT:
    return 1;
    break;
  }

  return 0;
}

int main(int /*argc*/, char * /*argv*/[]) {
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

  SDL_Event event;
  int quit = 0;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      quit = processEvent(event);
    }
  }

  SDL_DestroyWindow(window);

  SDL_Quit();

  return EXIT_SUCCESS;
}