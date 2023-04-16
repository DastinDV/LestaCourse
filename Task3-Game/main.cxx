#include "../Task3-Engine/engine.hxx"
#include <iostream>

int main(int /*argc*/, char * /*argv*/[]) {
  bool quit = false;

  {
    using namespace engine_sdl;
    EventEmulatorI *sonyEmulator = new SonyPSEventEmulator();
    Engine engine;
    engine.Initialize();
    engine.SetKeyBoardEmulator(sonyEmulator);

    while (!quit) {
      event event;
      engine.ProcessEvent(event);
      if (event == event::quit)
        quit = true;

      std::cout << engine.GetKeyEventInfo().eventType << " "
                << engine.GetKeyEventInfo().keyCode << std::endl;
    }
  }
}