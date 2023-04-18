#include "emulator.hxx"
#include "engine.hxx"
#include "game.hxx"

//#include "consoleGame.hxx"

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

    EventEmulatorI *sonyEmulator = new SonyPSEventEmulator();
    Engine engine;
    engine.Initialize();
    engine.SetKeyBoardEmulator(sonyEmulator);

    Game *consoleGame = ReloadGame(nullptr, library_name, tmp_library_file,
                                   engine, game_library_handle);

    auto time_during_loading = std::filesystem::last_write_time(library_name);
    while (!quit) {
      event event;
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
      if (event == event::quit)
        quit = true;

      std::cout << engine.GetKeyEventInfo().eventType << " "
                << engine.GetKeyEventInfo().keyCode << std::endl;

      consoleGame->Render();
      consoleGame->Update();
    }
  }

  return 0;
}