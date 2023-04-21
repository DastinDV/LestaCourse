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