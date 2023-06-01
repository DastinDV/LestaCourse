#pragma once

#ifndef IZ_DECLSPEC
#define IZ_DECLSPEC
#endif

#include <optional>
#include <string_view>
#include <vector>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

namespace core {

enum class EventType { keyboard_event, mouse_event, window_event, quit };

// ########## Keyboard
enum class KeyCode { w, a, s, d, q, e, z, x, space, lctrl, rctrl, not_bind };

enum class KeyboardEventType {
  key_pressed,
  key_released,
};

struct KeyboardInfo {
  std::string_view keyCodeName;
  KeyCode keyCode;
  KeyboardEventType type;
};

// ########## Window
enum class WindowEventType { resized, maximized };

struct WindowInfo {
  unsigned int windowId;
  WindowEventType type;
};
// ########## Mouse
struct MouseInfo {
  double xPos;
  double yPos;
};

struct Event {
  EventType eventType;
  std::optional<KeyboardInfo> keyBoardInfo;
  std::optional<MouseInfo> mouseInfo;
  std::optional<WindowInfo> windowInfo;
};

float *Translate(std::vector<float> &translation);
float *Rotate(float angle);
float *Scale(float scaleFactor);
float *OrthoProj(float left, float right, float bottom, float top, float near,
                 float far);

class IZ_DECLSPEC Engine {
public:
  Engine();

  int Initialize(int screenWidth, int screenHeight);
  int ProcessEvent(Event &event);
  int CleanUp();

  // ###### OpenGL ######
  void ClearScreen(float deltaTime);
  void SwapBuffers();
  void ResizeViewPort() const;
  // ###### OpenGL ######

private:
  void GetOpenGLVersionInfo();
};
} // namespace core