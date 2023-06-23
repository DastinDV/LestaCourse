#pragma once

#ifndef IZ_DECLSPEC
#define IZ_DECLSPEC
#endif

#include <mutex>
#include <optional>
#include <string_view>
#include <vector>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

namespace core {

enum class EventType { keyboard_event, mouse_event, window_event, quit };

// ########## Keyboard
enum class KeyCode { w, a, s, d, q, e, r, z, x, space, lctrl, rctrl, not_bind };

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
  int width;
  int height;
};
// ########## Mouse

enum class MouseEventType { button_down, button_up, mouse_motion };

struct MouseInfo {
  float xPos;
  float yPos;
  MouseEventType type;
};

struct Event {
  EventType eventType;
  std::optional<KeyboardInfo> keyBoardInfo;
  std::optional<MouseInfo> mouseInfo;
  std::optional<WindowInfo> windowInfo;
};

// ###### Math ######
float *Translate(std::vector<float> &translation);
float *Rotate(float angle);
float *Scale(float scaleFactor);
float *OrthoProj(float left, float right, float bottom, float top, float near,
                 float far);

std::vector<float> Unproject(std::vector<float> win, std::vector<float> model,
                             float xCorrection, float yCorrection,
                             int screenWidth, int screenHeight);
// ###### Math ######

std::pair<int, int> GetScreenSize();

class IZ_DECLSPEC SoundBuffer {
public:
  enum class properties { once, looped };
  virtual ~SoundBuffer();
  virtual void play(const properties) = 0;
  virtual void stop() = 0;
};

SoundBuffer *CreateSoundBuffer(std::string_view path);
void DestroySoundBuffer(SoundBuffer *);

class SoundBufferImpl;

class IZ_DECLSPEC Engine {
public:
  Engine();

  int Initialize(int screenWidth, int screenHeight);
  int ProcessEvent(Event &event);
  int CleanUp();

  // ###### OpenGL ######
  void ClearScreen(float deltaTime);
  void SwapBuffers();
  void ResizeViewPort(int w, int h) const;
  // ###### OpenGL ######

  // virtual SoundBuffer *CreateSoundBuffer(std::string_view path);
  // virtual void DestroySoundBuffer(SoundBuffer *);

private:
  void GetOpenGLVersionInfo();

  // static std::mutex audio_mutex;
  static void audio_callback(void *, uint8_t *, int);

  friend class SoundBufferImpl; // for audio_mutex
};
} // namespace core
