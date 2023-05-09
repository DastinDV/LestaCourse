#include "./Renderers/glRenderer.hxx"
#include "./Renderers/lineRenderer.hxx"
#include "./Renderers/triangleRenderer.hxx"
#include "./glad/include/glad/glad.h"
#include "canvas.hxx"
#include "engine.hxx"
#include "game.hxx"

#include <SDL3/SDL.h>

#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <thread>

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

    BitFlag flags;
    flags.SetFlag(ETriangleSettings::RASTERIZED);
    flags.SetFlag(ETriangleSettings::INTERPOLATED);

    Game *consoleGame = ReloadGame(nullptr, library_name, tmp_library_file,
                                   engine, game_library_handle);

    consoleGame->Init();
    auto time_during_loading = std::filesystem::last_write_time(library_name);

    float deltaTime = 0.0;
    float timeSinceRun = 0.0;
    long last;

    GlRenderer glRenderer;
    // clang-format off
      std::vector<GlVertex> points = {
          {0.0f, 0.0f, 0.0f},
          {-0.1f, 0.0f, 0.0f},
          {0.1f, 0.0f, 0.0f}};

      std::vector<GlVertex> points1 = {
          {0.0f, 0.1f, 0.0f},
          {0.0f, -0.1f, 0.0f}};

      std::vector<GlVertex> triangle = {
          {-1.0f, -1.0f, 0.0f},
          {1.0f, -1.0f, 0.0f},
          {0.0f, 1.0f, 0.0f}};

    const std::string vertexShaderSource = R"(
              #version 330 core

              layout (location = 0) in vec3 position;

              void main()
              {
                  gl_Position = vec4(position.x / 4.0f, position.y/ 4.0f,
                  position.z/ 4.0f, 1.0);
              }
     )";

    const std::string fragmentShaderSource = R"(
              #version 330 core

              out vec4 FragColor;

              void main()
              {
                  FragColor = vec4(1.0, 0.0, 0.0, 0.0);
              }
     )";
    // clang-format on

    int programId = CreateShader(vertexShaderSource, fragmentShaderSource);
    glUseProgram(programId);

    while (!quit) {
      long now = SDL_GetTicks();

      if (now > last) {
        deltaTime = ((float)(now - last)) / 1000;
        timeSinceRun += deltaTime;
        last = now;
      }

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

      engine.ClearScreen(timeSinceRun);

      glRenderer.DrawPoint(points);
      glRenderer.DrawPoint(points1);
      glRenderer.DrawTriangle(triangle);

      engine.SwapBuffers();
      // consoleGame->Update(deltaTime);
      // consoleGame->Render();
    }

    engine.CleanUp();
  }

  return 0;
}