#include "./Renderers/lineRenderer.hxx"
#include "./Renderers/triangleRenderer.hxx"
#include "canvas.hxx"
#include "engine.hxx"
#include "game.hxx"

#include <SDL3/SDL.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

int main() {
  {
    using namespace core;

    bool quit;
    const char *library_name = SDL_GetPlatform() == "Windows"
                                   ? "libConsoleGame.dll"
                                   : "../lib/MyGame/libConsoleGame.so";
    const char *tmp_library_file = "../lib/MyGame/temp.so";

    void *game_library_handle{};

    Engine engine;
    engine.Initialize();

    Game *consoleGame = ReloadGame(nullptr, library_name, tmp_library_file,
                                   engine, game_library_handle);

    auto time_during_loading = std::filesystem::last_write_time(library_name);

    Canvas canvas(640, 480);

    LineRenderer lineRenderer(canvas);
    // lineRenderer.Draw({200, 200}, {300, 300}, {0, 255, 0});
    // lineRenderer.Draw({200, 200}, {300, 100}, {255, 0, 0});
    // lineRenderer.Draw({200, 200}, {100, 300}, {0, 0, 255});
    // lineRenderer.Draw({200, 200}, {100, 100}, {255, 255, 255});
    // lineRenderer.Draw({210, 200}, {300, 200}, {255, 0, 0});
    // lineRenderer.Draw({200, 210}, {200, 300}, {255, 0, 0});
    // lineRenderer.Draw({190, 200}, {100, 200}, {255, 0, 0});
    // lineRenderer.Draw({200, 190}, {200, 100}, {255, 0, 0});

    TriangleRenderer triangleRenderer(canvas);
    std::vector<Position> vertexies;
    Position v1 = {0, 0};
    Position v2 = {640, 480};
    Position v3 = {0, 480};
    int step = 10;
    for (int i = 0; i < 16; i++) {
      vertexies.push_back(v1);
      vertexies.push_back(v2);
      vertexies.push_back(v3);
      v1.x += 10;
      v1.y += 20;
      v2.x -= 30;
      v2.y -= 10;
      v3.x += 10;
      v3.y -= 10;
    }

    std::vector<Position> vertexies1;
    Position v11 = {0, 0};
    Position v21 = {640, 0};
    Position v31 = {640, 480};

    for (int i = 0; i < 16; i++) {
      vertexies1.push_back(v11);
      vertexies1.push_back(v21);
      vertexies1.push_back(v31);
      v11.x += 30;
      v11.y += 10;
      v21.x -= 10;
      v21.y += 10;
      v31.x -= 10;
      v31.y -= 20;
    }

    triangleRenderer.Draw(vertexies, {0, 255, 0});
    triangleRenderer.Draw(vertexies1, {255, 0, 0});
    // for (int i = 100; i < 380; i++) {
    //   lineRenderer.Draw({320, 240}, {640, i}, {0, 255, 0});
    //   lineRenderer.Draw({320, 240}, {0, i}, {0, 255, 0});
    //   //   lineRenderer.Draw({200, 200}, {i, 100}, {0, 255, 0});
    // }
    // for (int i = 100; i <= 540; i++) {
    //   lineRenderer.Draw({320, 240}, {i, 0}, {255, 0, 0});
    //   lineRenderer.Draw({320, 240}, {i, 480}, {255, 0, 0});
    // }

    canvas.RenderToSDLWindow();

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
      consoleGame->OnEvent(event);

      if (event.eventType == EventType::keyboard_event &&
          event.keyBoardInfo.has_value()) {
        std::cout << "Key pressed " << event.keyBoardInfo->keyCodeName
                  << std::endl;
      }

      if (event.eventType == EventType::quit)
        quit = true;

      // consoleGame->Render();
      consoleGame->Update();
    }
  }

  return 0;
}