#pragma once
#include "emulator.hxx"
#include <SDL3/SDL.h>

#ifndef IZ_DECLSPEC
#define IZ_DECLSPEC
#endif

namespace core {

enum class event { key_pressed, key_released, quit };

struct KeyEventInfo {
  std::string eventType;
  std::string keyCode;
};

class IZ_DECLSPEC Engine {
public:
  Engine();

  int Initialize();
  int ProcessEvent(event &event);

  void SetKeyBoardEmulator(EventEmulatorI *currentEmulator);
  KeyEventInfo &GetKeyEventInfo();

private:
  EventEmulatorI *eventEmulator;
  KeyEventInfo eventInfo;
};
} // namespace core