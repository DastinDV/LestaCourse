#include "canvas.hxx"
#include "engine.hxx"
#include "game.hxx"
#include "glRenderer.hxx"
#include "glad/glad.h"
#include "helper.hxx"
#include "lineRenderer.hxx"
#include "shader.hxx"
#include "triangleRenderer.hxx"
#include "vertexBuffer.hxx"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cassert>
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
            ? "libFinalProject-shared.dll"
            : "../lib/MirrorGame/libFinalProject-shared.so";
    const char *tmp_library_file = "../lib/glTest/temp.so";

    void *game_library_handle{};

    Engine engine;
    engine.Initialize(640, 640);

    Canvas canvas(640, 480);

    BitFlag flags;
    flags.SetFlag(ETriangleSettings::RASTERIZED);
    flags.SetFlag(ETriangleSettings::INTERPOLATED);

    Game *consoleGame = ReloadGame(nullptr, library_name, tmp_library_file,
                                   engine, game_library_handle);

    try {
      consoleGame->Init();
    } catch (std::runtime_error &err) {
      std::cout << err.what() << std::endl;
      return EXIT_FAILURE;
    }

    auto time_during_loading = std::filesystem::last_write_time(library_name);

    float deltaTime = 0.0;
    float timeSinceRun = 0.0;
    long last;

    while (!quit) {
      long now = SDL_GetTicks();

      if (now > last) {
        deltaTime = ((float)(now - last)) / 1000;
        timeSinceRun += deltaTime;
        last = now;
      }

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

      Event event;
      engine.ProcessEvent(event);
      if (event.eventType == EventType::keyboard_event &&
          event.keyBoardInfo.has_value()) {
        if (event.keyBoardInfo->type == core::KeyboardEventType::key_pressed) {
          std::cout << "Key pressed " << event.keyBoardInfo->keyCodeName
                    << std::endl;
        }
      }

      if (event.eventType == EventType::window_event &&
          event.windowInfo.has_value()) {
        if (event.windowInfo->type == WindowEventType::resized ||
            event.windowInfo->type == WindowEventType::maximized) {
          engine.ResizeViewPort();
        }
      }

      if (event.eventType == EventType::mouse_event &&
          event.mouseInfo.has_value()) {
      }

      if (event.eventType == EventType::quit)
        quit = true;

      consoleGame->OnEvent(event, deltaTime);

      engine.ClearScreen(timeSinceRun);

      consoleGame->Update(deltaTime);
      consoleGame->Render();
      engine.SwapBuffers();
    }

    engine.CleanUp();
  }

  return 0;
}