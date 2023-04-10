#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_version.h>

std::ostream &operator<<(std::ostream &out, const SDL_version &v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

int main(int, char **) {

  SDL_Window *window; // Declare a pointer

  SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

  // Create an application window with the following settings:
  window = SDL_CreateWindow("An SDL2 window", // window title
                            640,              // width, in pixels
                            480,              // height, in pixels
                            SDL_WINDOW_OPENGL // flags - see below
  );

  // Check that the window was successfully created
  if (window == NULL) {
    // In the case that the window could not be made...
    printf("Could not create window: %s\n", SDL_GetError());
    return 1;
  }

  // The window is open: could enter program loop here (see SDL_PollEvent())

  SDL_Delay(3000); // Pause execution for 3000 milliseconds, for example

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();

  SDL_version compiled = {0, 0, 0};
  SDL_version linked = {0, 0, 0};

  SDL_VERSION(&compiled)
  SDL_GetVersion(&linked);

  std::cout << "compiled: " << compiled << '\n';
  std::cout << "linked: " << linked << std::endl;

  bool is_good = std::cout.good();

  int result = is_good ? EXIT_SUCCESS : EXIT_FAILURE;
  return result;
}
