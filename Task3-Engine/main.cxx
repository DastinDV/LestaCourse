#include "./Renderers/lineRenderer.hxx"
#include "./Renderers/triangleRenderer.hxx"
#include "canvas.hxx"
#include "engine.hxx"
#include "game.hxx"

#include <SDL3/SDL.h>

#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <thread>

namespace core {
struct RotateImage : gfx_program {
  uniforms uniforms_;

  void set_uniforms(const uniforms &a_uniforms) override {
    uniforms_ = a_uniforms;
  }

  Vertex vertex_shader(const Vertex &v_in) override {
    Vertex out = v_in;
    out.pos.x -= 320;
    out.pos.y -= 240;

    double angle = 1;
    // double alpha = (M_PI / 2);
    double alpha = angle * (M_PI / 180); // * uniforms_.f7 * -1;
    std::cout << alpha << std::endl;
    double x = out.pos.x;
    double y = out.pos.y;

    std::cout << "BEFORE" << std::endl;
    std::cout << "x " << out.pos.x << " "
              << "y " << out.pos.y << std::endl;

    std::cout << "xcos " << x * std::cos(alpha) << " "
              << "ysin " << y * std::sin(alpha) << std::endl;

    std::cout << std::endl;
    out.pos.x = std::ceil(x * std::cos(alpha) - y * std::sin(alpha));
    out.pos.y = std::ceil(x * std::sin(alpha) + y * std::cos(alpha));

    out.pos.x += 320;
    out.pos.y += 240;

    std::cout << "AFTER" << std::endl;
    std::cout << "x " << out.pos.x << " "
              << "y " << out.pos.y << std::endl;

    return out;
  }

  Color fragment_shader(const Vertex &v_in) override {
    Color out = v_in.color;
    return out;
  }

} rotateImage;
} // namespace core

int main() {
  {
    using namespace core;
    bool quit;
    const char *library_name =
        SDL_GetPlatform() == "Windows"
            ? "libRenderBasics.dll"
            : "../lib/RenderBasicsGame/libRenderBasics.so";
    const char *tmp_library_file = "../lib/RenderBasicsGame/temp.so";

    void *game_library_handle{};

    Engine engine;
    engine.Initialize();

    Canvas canvas(640, 480);
    std::vector<Vertex> vertexes;
    Position v1 = {320, 140};
    Position v2 = {220, 340};
    Position v3 = {420, 340};

    vertexes.push_back({v1, red});
    vertexes.push_back({v2, green});
    vertexes.push_back({v3, blue});

    TriangleRenderer triangleRenderer(canvas);
    gfx_program *rotateImage = new RotateImage();
    triangleRenderer.SetGFXProgram(rotateImage);

    BitFlag flags;
    flags.SetFlag(ETriangleSettings::RASTERIZED);
    flags.SetFlag(ETriangleSettings::INTERPOLATED);

    triangleRenderer.Draw(vertexes, green, true, flags);
    canvas.RenderToSDLWindow();

    Game *consoleGame = ReloadGame(nullptr, library_name, tmp_library_file,
                                   engine, game_library_handle);

    consoleGame->Init();
    auto time_during_loading = std::filesystem::last_write_time(library_name);

    while (!quit) {
      Event event;
      engine.ProcessEvent(event);

      auto time_current_file_time =
          std::filesystem::last_write_time(library_name);
      if (time_current_file_time != time_during_loading) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        consoleGame = ReloadGame(consoleGame, library_name, tmp_library_file,
                                 engine, game_library_handle);
        time_during_loading = time_current_file_time;

        if (consoleGame == nullptr) {
          std::cerr << "next attempt to reload game..." << std::endl;
          continue;
        }
      }

      if (event.eventType == EventType::keyboard_event &&
          event.keyBoardInfo.has_value()) {
        std::cout << "Key pressed " << event.keyBoardInfo->keyCodeName
                  << std::endl;
      }

      if (event.eventType == EventType::quit)
        quit = true;

      consoleGame->OnEvent(event);

      triangleRenderer.Draw(vertexes, green, true, flags);
      canvas.RenderToSDLWindow();
      //  consoleGame->Update();
      //  consoleGame->Render();
    }
  }

  return 0;
}