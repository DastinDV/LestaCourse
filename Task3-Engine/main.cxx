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
  Uniforms uniforms_;

  void set_uniforms(const Uniforms &a_uniforms) override {
    uniforms_ = a_uniforms;
  }

  Vertex vertex_shader(const Vertex &v_in) override {
    Vertex out = v_in;
    out.x -= 320;
    out.y -= 240;

    double angle = 40;
    // double alpha = (M_PI / 2);
    double alpha = angle * (M_PI / 180); // * uniforms_.f7 * -1;
    // std::cout << alpha << std::endl;
    double x = out.x;
    double y = out.y;

    // std::cout << "BEFORE" << std::endl;
    // std::cout << "x " << out.x << " "
    //           << "y " << out.y << std::endl;

    // std::cout << "xcos " << x * std::cos(alpha) << " "
    //           << "ysin " << y * std::sin(alpha) << std::endl;

    // std::cout << std::endl;
    out.x = x * std::cos(alpha) - y * std::sin(alpha);
    out.y = x * std::sin(alpha) + y * std::cos(alpha);

    out.x += 320;
    out.y += 240;

    // std::cout << "AFTER" << std::endl;
    // std::cout << "x " << out.x << " "
    //           << "y " << out.y << std::endl;

    return out;
  }

  Color fragment_shader(const Vertex &v_in) override {
    Color out = v_in.color;
    return out;
  }

} rotateImage;
// namespace core

// struct CoolShader : gfx_program {
//   double mouse_x{};
//   double mouse_y{};
//   double radius{};

//   void set_uniforms(const Uniforms &a_uniforms) override {
//     mouse_x = a_uniforms.f0;
//     mouse_y = a_uniforms.f1;
//     radius = a_uniforms.f2;
//   }
//   Vertex vertex_shader(const Vertex &v_in) override {
//     Vertex out = v_in;

//     double x = out.x;
//     double y = out.y;

//     double dx = x - mouse_x;
//     double dy = y - mouse_y;
//     if (dx * dx + dy * dy < radius * radius) {
//       // un magnet from mouse
//       double len = std::sqrt(dx * dx + dy * dy);
//       if (len > 0) {
//         // normalize vector from vertex to mouse pos
//         double norm_dx = dx / len;
//         double norm_dy = dy / len;
//         // find position of point on radius from mouse pos in center
//         double radius_pos_x = mouse_x + norm_dx * radius;
//         double radius_pos_y = mouse_y + norm_dy * radius;
//         // find middle point
//         x = (x + radius_pos_x) / 2;
//         y = (y + radius_pos_y) / 2;
//       }
//     }

//     out.x = x;
//     out.y = y;

//     return out;
//   }

//   Color fragment_shader(const Vertex &v_in) override {
//     Color out = v_in.color;
//     return out;
//   }

// } program01;

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

    for (int i = 0; i < 1; i++) {
      vertexes.push_back({320., 140., red});
      vertexes.push_back({220., 340., green});
      vertexes.push_back({420., 340., blue});
    }

    TriangleRenderer triangleRenderer(canvas);
    Uniforms uniforms;
    gfx_program *rotateImage = new RotateImage();
    // triangleRenderer.SetGFXProgram(rotateImage);
    triangleRenderer.SetGFXProgram(rotateImage);

    BitFlag flags;
    flags.SetFlag(ETriangleSettings::RASTERIZED);
    flags.SetFlag(ETriangleSettings::INTERPOLATED);

    auto start = std::chrono::steady_clock::now();
    triangleRenderer.Draw(vertexes, green, true, flags);
    canvas.RenderToSDLWindow();
    auto end = std::chrono::steady_clock::now();

    auto diff = end - start;

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                     .count()
              << std::endl;
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

      if (event.eventType == EventType::mouse_event &&
          event.mouseInfo.has_value()) {
      }

      if (event.eventType == EventType::quit)
        quit = true;

      consoleGame->OnEvent(event);

      // triangleRenderer.Draw(vertexes, green, true, flags);
      // canvas.RenderToSDLWindow();
      // canvas.Clear({0, 0, 0});

      consoleGame->Update();
      consoleGame->Render();
    }
  }

  return 0;
}