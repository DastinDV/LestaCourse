#pragma once

#ifndef IZ_DECLSPEC
#define IZ_DECLSPEC
#endif

#include <optional>
#include <string_view>

namespace core {

enum class EventType { keyboard_event, quit };

enum class KeyCode { w, a, s, d, space, lctrl, rctrl, not_bind };

enum class KeyboardEventType {
  key_pressed,
  key_released,
};

struct KeyboardInfo {
  std::string_view keyCodeName;
  KeyCode keyCode;
  KeyboardEventType type;
};

struct Event {
  EventType eventType;
  std::optional<KeyboardInfo> keyBoardInfo;
};

class IZ_DECLSPEC Engine {
public:
  Engine();

  int Initialize();
  int ProcessEvent(Event &event);
};
} // namespace core