#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <queue>

#include "canvas.hxx"
#include "engine.hxx"
#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>

namespace core {


#ifdef _WIN32
struct UserDataForEvent {
  int screenWidth;
  int screenHeight;
} userDataForEvent;

std::queue<UserDataForEvent> screenSizeEventQueue;

static int SDLCALL filterEvent(void *userdata, SDL_Event * event) {
    if (event->type == SDL_EVENT_WINDOW_RESIZED) {  
      std::queue<UserDataForEvent>* data = (std::queue<UserDataForEvent>*)userdata;
      SDL_Window* window = SDL_GetWindowFromID(event->window.windowID);
      int w, h;
      SDL_GetWindowSize(window, &w, &h);
      data->push({w, h});     
      //return 0 if you don't want to handle this event twice
      return 0;
    }
    //important to allow all events, or your SDL_PollEvent doesn't get any event
    return 1;
}
#endif
// class SoundBufferImpl;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GLContext openGLContext = nullptr;
SDL_AudioDeviceID audio_device;
SDL_AudioSpec audio_device_spec;
std::vector<SoundBufferImpl *> sounds;
static std::mutex audio_mutex;

// std::mutex Engine::audio_mutex;

static std::string_view get_sound_format_name(uint16_t format_value) {
  static const std::map<int, std::string_view> format = {
      {AUDIO_U8, "AUDIO_U8"},         {AUDIO_S8, "AUDIO_S8"},
      {AUDIO_S16LSB, "AUDIO_S16LSB"}, {AUDIO_S16MSB, "AUDIO_S16MSB"},
      {AUDIO_S32LSB, "AUDIO_S32LSB"}, {AUDIO_S32MSB, "AUDIO_S32MSB"},
      {AUDIO_F32LSB, "AUDIO_F32LSB"}, {AUDIO_F32MSB, "AUDIO_F32MSB"},
  };

  auto it = format.find(format_value);
  return it->second;
}

static std::size_t get_sound_format_size(uint16_t format_value) {
  static const std::map<int, std::size_t> format = {
      {AUDIO_U8, 1},     {AUDIO_S8, 1},     {AUDIO_S16LSB, 2},
      {AUDIO_S16MSB, 2}, {AUDIO_S32LSB, 4}, {AUDIO_S32MSB, 4},
      {AUDIO_F32LSB, 4}, {AUDIO_F32MSB, 4},
  };

  auto it = format.find(format_value);
  return it->second;
}

const std::unordered_map<SDL_Keycode, KeyCode> sdlToEngineKeyBinding{
    {SDLK_w, KeyCode::w},         {SDLK_s, KeyCode::s},
    {SDLK_a, KeyCode::a},         {SDLK_d, KeyCode::d},
    {SDLK_q, KeyCode::q},         {SDLK_e, KeyCode::e},
    {SDLK_r, KeyCode::r},         {SDLK_z, KeyCode::z},
    {SDLK_x, KeyCode::x},         {SDLK_LCTRL, KeyCode::lctrl},
    {SDLK_RCTRL, KeyCode::rctrl}, {SDLK_SPACE, KeyCode::space}};

static std::ostream &operator<<(std::ostream &out, const SDL_version &v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

SoundBuffer::~SoundBuffer() {}

#pragma pack(push, 1)
class SoundBufferImpl final : public SoundBuffer {
public:
  SoundBufferImpl(std::string_view path, SDL_AudioDeviceID device_,
                  SDL_AudioSpec device_audio_spec);

  ~SoundBufferImpl() final;

  void play(const properties prop) final {
    std::lock_guard<std::mutex> lock(audio_mutex);
    // here we can change properties
    // of sound and dont collade with multithreaded playing
    // current_index = 0;
    is_playing = true;
    is_looped = (prop == properties::looped);
  }

  void stop() final {
    std::lock_guard<std::mutex> lock(audio_mutex);
    current_index = 0;
    is_playing = false;
  }

  std::unique_ptr<uint8_t[]> tmp_buf;
  uint8_t *buffer;
  uint32_t length;
  uint32_t current_index = 0;
  SDL_AudioDeviceID device;
  bool is_playing = false;
  bool is_looped = false;
};
#pragma pack(pop)

SoundBufferImpl::SoundBufferImpl(std::string_view path,
                                 SDL_AudioDeviceID device_,
                                 SDL_AudioSpec device_audio_spec)
    : buffer(nullptr), length(0), device(device_) {
  SDL_RWops *file = SDL_RWFromFile(path.data(), "rb");
  if (file == nullptr) {
    throw std::runtime_error(std::string("can't open audio file: ") +
                             path.data());
  }

  // freq, format, channels, and samples - used by SDL_LoadWAV_RW
  SDL_AudioSpec file_audio_spec;

  if (nullptr == SDL_LoadWAV_RW(file, 1, &file_audio_spec, &buffer, &length)) {
    throw std::runtime_error(std::string("can't load wav: ") + path.data());
  }

  std::cout << "--------------------------------------------\n";
  std::cout << "audio format for: " << path << '\n'
            << "format: " << get_sound_format_name(file_audio_spec.format)
            << '\n'
            << "sample_size: " << get_sound_format_size(file_audio_spec.format)
            << '\n'
            << "channels: " << static_cast<uint32_t>(file_audio_spec.channels)
            << '\n'
            << "frequency: " << file_audio_spec.freq << '\n'
            << "length: " << length << '\n'
            << "time: "
            << static_cast<double>(length) /
                   (file_audio_spec.channels *
                    static_cast<uint32_t>(file_audio_spec.freq) *
                    get_sound_format_size(file_audio_spec.format))
            << "sec" << std::endl;
  std::cout << "--------------------------------------------\n";

  if (file_audio_spec.channels != device_audio_spec.channels ||
      file_audio_spec.format != device_audio_spec.format ||
      file_audio_spec.freq != device_audio_spec.freq) {
    Uint8 *output_bytes;
    int output_length;

    int convert_status = SDL_ConvertAudioSamples(
        file_audio_spec.format, file_audio_spec.channels, file_audio_spec.freq,
        buffer, static_cast<int>(length), device_audio_spec.format,
        device_audio_spec.channels, device_audio_spec.freq, &output_bytes,
        &output_length);
    if (0 != convert_status) {
      std::stringstream message;
      message << "failed to convert WAV byte stream: " << SDL_GetError();
      throw std::runtime_error(message.str());
    }

    SDL_free(buffer);
    buffer = output_bytes;
    length = static_cast<uint32_t>(output_length);
  } else {
    // no need to convert buffer, use as is
  }
}

SoundBufferImpl::~SoundBufferImpl() {
  if (!tmp_buf) {
    SDL_free(buffer);
  }
  buffer = nullptr;
  length = 0;
}

Engine::Engine() {}

int Engine::Initialize(int screenWidth, int screenHeight) {
  using namespace std;

  SDL_version compiled = {0, 0, 0};
  SDL_version linked = {0, 0, 0};

  int GlMajorVer = 3;
  int GlMinorVer = 2;

  SDL_VERSION(&compiled)
  SDL_GetVersion(&linked);

  if (SDL_COMPILEDVERSION !=
      SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
    cerr << "warning: SDL2 compiled and linked version mismatch: " << compiled
         << " " << linked << endl;
  }

  const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
  if (init_result != 0) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call SDL_Init: " << err_message << endl;
    return EXIT_FAILURE;
  }

  window = SDL_CreateWindow("title", screenWidth, screenHeight,
                            ::SDL_WINDOW_OPENGL | ::SDL_WINDOW_RESIZABLE);

#ifdef _WIN32
  // weird thing for windows.
  SDL_SetEventFilter(filterEvent, &screenSizeEventQueue);
#endif

  if (window == nullptr) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call SDL_CreateWindow: " << err_message << endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GlMajorVer);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GlMinorVer);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, GlMinorVer);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  openGLContext = SDL_GL_CreateContext(window);
  if (openGLContext == nullptr) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call SDL_GL_CreateContext: " << err_message << endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  if (!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress)) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call gladLoadGLES2Loader: " << err_message << endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);

  if (renderer == nullptr) {
    const char *err_message = SDL_GetError();
    cerr << "error: failed call SDL_CreateRenderer: " << err_message << endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // initialize audio
  audio_device_spec.freq = 48000;
  audio_device_spec.format = AUDIO_S16LSB;
  audio_device_spec.channels = 2;
  audio_device_spec.samples = 4096; // must be power of 2
  audio_device_spec.callback = Engine::audio_callback;
  audio_device_spec.userdata = this;

  const int num_audio_drivers = SDL_GetNumAudioDrivers();
  for (int i = 0; i < num_audio_drivers; ++i) {
    std::cout << "audio_driver #:" << i << " " << SDL_GetAudioDriver(i) << '\n';
  }
  std::cout << std::flush;

  const char *default_audio_device_name = nullptr;

  // SDL_FALSE - mean get only OUTPUT audio devices
  const int num_audio_devices = SDL_GetNumAudioDevices(SDL_FALSE);
  if (num_audio_devices > 0) {
    default_audio_device_name =
        SDL_GetAudioDeviceName(num_audio_devices - 1, SDL_FALSE);
    for (int i = 0; i < num_audio_devices; ++i) {
      std::cout << "audio device #" << i << ": "
                << SDL_GetAudioDeviceName(i, SDL_FALSE) << '\n';
    }
  }
  std::cout << std::flush;

  audio_device =
      SDL_OpenAudioDevice(default_audio_device_name, 0, &audio_device_spec,
                          nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);

  if (audio_device == 0) {
    std::cerr << "failed open audio device: " << SDL_GetError();
    throw std::runtime_error("audio failed");
  } else {
    std::cout << "--------------------------------------------\n";
    std::cout << "audio device selected: " << default_audio_device_name << '\n'
              << "freq: " << audio_device_spec.freq << '\n'
              << "format: " << get_sound_format_name(audio_device_spec.format)
              << '\n'
              << "channels: "
              << static_cast<uint32_t>(audio_device_spec.channels) << '\n'
              << "samples: " << audio_device_spec.samples << '\n'
              << std::flush;

    // unpause device
    SDL_PlayAudioDevice(audio_device);
  }

  GetOpenGLVersionInfo();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  ImGui_ImplSDL3_InitForOpenGL(window, openGLContext);
  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsDark();

  return EXIT_SUCCESS;
}

void Engine::ClearScreen(float deltaTime) {
  auto current_color = std::sin(0.5f * deltaTime);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::ResizeViewPort(int w, int h) const {
  if (window) {
    glViewport(0, 0, w, h);
  }
}

SoundBuffer *CreateSoundBuffer(std::string_view path) {
  SoundBufferImpl *s =
      new SoundBufferImpl(path, audio_device, audio_device_spec);
  {
    // push_backsound_buffer_impl
    std::lock_guard<std::mutex> lock(audio_mutex);
    sounds.push_back(s);
  }
  return s;
}

void DestroySoundBuffer(SoundBuffer *sound) { delete sound; }

void Engine::GetOpenGLVersionInfo() {
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
}

void Engine::audio_callback(void *userdata, uint8_t *stream, int stream_size) {

  std::lock_guard<std::mutex> lock(audio_mutex);

  // no sound default
  std::fill_n(stream, stream_size, '\0');

  for (SoundBufferImpl *snd : sounds) {
    if (snd->is_playing) {
      uint32_t rest = snd->length - snd->current_index;
      uint8_t *current_buff = &snd->buffer[snd->current_index];

      if (rest <= static_cast<uint32_t>(stream_size)) {
        // copy rest to buffer
        SDL_MixAudioFormat(stream, current_buff, audio_device_spec.format, rest,
                           SDL_MIX_MAXVOLUME);
        snd->current_index += rest;
      } else {
        SDL_MixAudioFormat(stream, current_buff, audio_device_spec.format,
                           static_cast<uint32_t>(stream_size),
                           SDL_MIX_MAXVOLUME);
        snd->current_index += static_cast<uint32_t>(stream_size);
      }

      if (snd->current_index == snd->length) {
        if (snd->is_looped) {
          // start from begining
          snd->current_index = 0;
        } else {
          snd->is_playing = false;
        }
      }
    }
  }
}

void Engine::SwapBuffers() { SDL_GL_SwapWindow(window); }

KeyboardInfo KeyBinding(SDL_Keycode from, KeyboardEventType keyboardEvent) {
  KeyCode keyCode = sdlToEngineKeyBinding.count(from)
                        ? sdlToEngineKeyBinding.at(from)
                        : KeyCode::not_bind;
  return {SDL_GetKeyName(from), keyCode, keyboardEvent};
}

int Engine::ProcessEvent(Event &event) {

  // int a;
  // SDL_SetEventFilter(filterEvent, userDataForEvent);

  //userDataForEvent = &event;

  SDL_Event sdlEvent;

#ifdef _WIN32
// This code to deal with screen resize events for windows.
  while (!screenSizeEventQueue.empty()){
      auto item = screenSizeEventQueue.front();
      std::cout << "from queue " << item.screenWidth << " " << item.screenHeight << std::endl;
      screenSizeEventQueue.pop();
      event.eventType = EventType::window_event;
      event.windowInfo = {SDL_GetWindowID(window), WindowEventType::resized, item.screenWidth,
                          item.screenHeight};
      return 1;
  }
#endif

  while (SDL_PollEvent(&sdlEvent)) {
    ImGui_ImplSDL3_ProcessEvent(&sdlEvent);
    if (sdlEvent.type == SDL_EVENT_KEY_DOWN) {
      event.eventType = EventType::keyboard_event;
      event.keyBoardInfo =
          KeyBinding(sdlEvent.key.keysym.sym, KeyboardEventType::key_pressed);
    }
    if (sdlEvent.type == SDL_EVENT_KEY_UP) {
      event.eventType = EventType::keyboard_event;
      event.keyBoardInfo =
          KeyBinding(sdlEvent.key.keysym.sym, KeyboardEventType::key_released);
    }
    if (sdlEvent.type == SDL_EVENT_MOUSE_MOTION) {
      event.eventType = EventType::mouse_event;
      event.mouseInfo = {sdlEvent.motion.x, sdlEvent.motion.y,
                         MouseEventType::mouse_motion};
    }
    if (sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
        event.eventType = EventType::mouse_event;
        event.mouseInfo = {sdlEvent.button.x, sdlEvent.button.y,
                           MouseEventType::button_down};
      }
    }
    if (sdlEvent.type == SDL_EVENT_WINDOW_RESIZED) {
      event.eventType = EventType::window_event;
      int w, h;
      SDL_GetWindowSize(window, &w, &h);
      event.windowInfo = {SDL_GetWindowID(window), WindowEventType::resized, w,
                          h};
    }
    if (sdlEvent.type == SDL_EVENT_WINDOW_MAXIMIZED) {
      event.eventType = EventType::window_event;
      int w, h;
      SDL_GetWindowSize(window, &w, &h);
      event.windowInfo = {SDL_GetWindowID(window), WindowEventType::maximized,
                          w, h};
      return 1;
    }
    if (sdlEvent.type == SDL_EVENT_QUIT) {
      event.eventType = EventType::quit;
    }
  }
  return 1;
}

int Engine::CleanUp() {
  SDL_GL_DeleteContext(openGLContext);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}

glm::mat4 trans = glm::mat4(1.0f);
glm::mat4 proj = glm::mat4(1.0f);

// Translate vertex to a new coordinate
float *Translate(std::vector<float> &translation) {
  float *matrix = nullptr;

  trans = glm::mat4(1.0f);
  trans = glm::translate(
      trans, glm::vec3(translation[0], translation[1], translation[2]));

  matrix = glm::value_ptr(trans);
  return matrix;
}

// Rotate and scale verteces
float *Rotate(float angle) {
  float *matrix = nullptr;
  trans = glm::rotate(trans, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
  // rot = glm::scale(rot, glm::vec3(0.5, 0.5, 0.5));

  matrix = glm::value_ptr(trans);
  return matrix;
}

float *Scale(float scaleFactor) {
  float *matrix = nullptr;
  trans = glm::scale(trans, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

  matrix = glm::value_ptr(trans);
  return matrix;
}

// Ortographic projection with (0,0) in upper-left.
float *OrthoProj(float left, float right, float bottom, float top, float near,
                 float far) {
  float *matrix = nullptr;
  proj = glm::ortho(left, right, bottom, top, near, far);

  matrix = glm::value_ptr(proj);
  return matrix;
}

std::vector<float> Unproject(std::vector<float> win, std::vector<float> model,
                             float xCorrection, float yCorrection, int width,
                             int height) {
  glm::vec3 g_win(win[0], win[1], win[2]);
  proj = glm::ortho(0.0, xCorrection * 640.0, 0.0, yCorrection * 480.0, 0.0,
                    100.0);
  trans = glm::mat4(1.0f);
  trans = glm::translate(trans, glm::vec3(model[0], model[1], model[2]));
  glm::vec4 viewport(0.0f, 0.0f, (float)width, (float)height);
  glm::vec3 result = glm::unProject(g_win, trans, proj, viewport);

  std::cout << "unproject " << result[0] << " " << result[1] << " " << result[2]
            << std::endl;

  return {result[0], result[1], result[2]};
}

std::pair<int, int> GetScreenSize() {
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  return std::pair<int, int>(w, h);
}

} // namespace core