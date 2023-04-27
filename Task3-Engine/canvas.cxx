#include "canvas.hxx"
#include <SDL3/SDL.h>
#include <iostream>

namespace core {

extern SDL_Window *window;
extern SDL_Renderer *renderer;

bool operator==(const Color &l, const Color &r) {
  return l.r == r.r && l.g == r.g && l.b == r.b;
}

void Canvas::SetPixel(size_t x, size_t y, Color color) {
  pixels[y * width + x] = color;
}

const std::vector<Color> &Canvas::GetPixels() const { return this->pixels; }

Canvas::Canvas(size_t w, size_t h) : width{w}, height{h} {
  pixels.resize(width * height);
}

void Canvas::Clear(Color color) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      pixels[y * width + x] = color;
    }
  }
}

void Canvas::RenderToSDLWindow() {
  if (SDL_RenderClear(renderer) != 0)
    std::cout << "Error in clearing" << std::endl;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      auto color = pixels[y * width + x];
      SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
      if (SDL_RenderPoint(renderer, x, y) != 0) {
        // std::cout << "x " << x << " y " << y << std::endl;
        // std::cout << "Error in drawing pixel" << std::endl;
      }
    }
  }
  SDL_RenderPresent(renderer);
}
} // namespace core
