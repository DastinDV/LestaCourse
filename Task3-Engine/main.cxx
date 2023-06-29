#include "canvas.hxx"
#include "engine.hxx"
#include "game.hxx"
#include "glRenderer.hxx"
#include "glad/glad.h"
#include "helper.hxx"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_win32.h"
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Simple helper function to load an image into a OpenGL texture with common
// settings
bool LoadTextureFromFile(const char *filename, GLuint *out_texture,
                         int *out_width, int *out_height) {
  // Load from file
  int image_width = 0;
  int image_height = 0;
  unsigned char *image_data =
      stbi_load(filename, &image_width, &image_height, NULL, 4);
  if (image_data == NULL)
    return false;

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE); // This is required on WebGL for non
                                     // power-of-two textures
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);

  *out_texture = image_texture;
  *out_width = image_width;
  *out_height = image_height;

  return true;
}

void CenterNextElement(float width) {
  ImGui::SetCursorPosX((ImGui::GetWindowWidth() - width) / 2);
}

bool ButtonCenteredOnLine(const char *label, ImVec2 buttonSize,
                          float alignment = 0.5f) {
  ImGuiStyle &style = ImGui::GetStyle();

  float size = buttonSize.x;
  float avail = ImGui::GetContentRegionAvail().x;

  float off = (avail - size) * alignment;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

  return ImGui::Button(label, buttonSize);
}

int main() {
  {
    using namespace core;
    bool quit = false;

    using namespace std::string_literals;
    const char *library_name =
        SDL_GetPlatform() == "Windows"s
            ? "../lib/MirrorGame/FinalProject-shared.dll"
            : "../lib/MirrorGame/libFinalProject-shared.so";
    const char *tmp_library_file = SDL_GetPlatform() == "Windows"s
                                       ? "../lib/MirrorGame/temp.dll"
                                       : "../lib/MirrorGame/temp.so";

    void *game_library_handle{};

    const int screenWidth = 640;
    const int screenHeight = 480;

    Engine engine;
    engine.Initialize(screenWidth, screenHeight);

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

    ImGuiIO &io = ImGui::GetIO();

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    int my_image_width = 0;
    int my_image_height = 0;
    GLuint my_image_texture = 0;
    bool ret = LoadTextureFromFile(
        "../../FinalProject/Assets/Images/MirrorLogo1.jpg", &my_image_texture,
        &my_image_width, &my_image_height);
    IM_ASSERT(ret);

    auto screenSize = engine.GetScreenSize();
    float mainMenuSizeX = screenSize.first;
    float mainMenuSizeY = screenSize.second;

    bool isMainMenu = true;
    bool isGame = false;
    float aspectRatio = mainMenuSizeX / mainMenuSizeY;

    SoundBuffer *menuMusic =
        CreateSoundBuffer("../../FinalProject/Assets/Music/BackTheme1.wav");
    SoundBuffer *gameMusic =
        CreateSoundBuffer("../../FinalProject/Assets/Music/BackTheme.wav");
    assert(menuMusic != nullptr);
    assert(gameMusic != nullptr);

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
          engine.ResizeViewPort(event.windowInfo->width,
                                event.windowInfo->height);
          mainMenuSizeX = event.windowInfo->width;
          mainMenuSizeY = event.windowInfo->height;
          aspectRatio = mainMenuSizeX / mainMenuSizeY;
        }
      }

      if (event.eventType == core::EventType::mouse_event &&
          event.mouseInfo.has_value()) {
        if (event.mouseInfo->type == core::MouseEventType::button_down) {
        }
      }

      if (event.eventType == EventType::quit)
        quit = true;

      engine.ClearScreen(timeSinceRun);

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL3_NewFrame();
      ImGui::NewFrame();

      // ImGui::ShowDemoWindow();

      consoleGame->OnEvent(event, deltaTime);
      if (isMainMenu) {
        menuMusic->play(SoundBuffer::properties::looped);
        {
          ImGui::SetNextWindowSize({mainMenuSizeX, mainMenuSizeY});
          ImGui::SetNextWindowPos({0.0f, 0.0f});
          ImGui::Begin("OpenGL Texture Text");
          CenterNextElement(mainMenuSizeX - (mainMenuSizeX * 0.2f));
          ImGui::Image((void *)(intptr_t)my_image_texture,
                       ImVec2(mainMenuSizeX - (mainMenuSizeX * 0.2f),
                              mainMenuSizeY / 2.0));

          ImGui::Dummy(
              ImVec2{0.0, static_cast<float>(mainMenuSizeY / 2.0 / 10.0)});
          if (ButtonCenteredOnLine(
                  "Play",
                  ImVec2{200.f * aspectRatio,
                         static_cast<float>(mainMenuSizeY / 2.0 / 5.0)}))
            isMainMenu = false;
          ImGui::Dummy(
              ImVec2{0.0, static_cast<float>(mainMenuSizeY / 2.0 / 10.0)});
          if (ButtonCenteredOnLine(
                  "Exit",
                  ImVec2{200.f * aspectRatio,
                         static_cast<float>(mainMenuSizeY / 2.0 / 5.0)})) {
            engine.CleanUp();
            delete consoleGame;
            quit = true;
          }
          ImGui::End();
        }
      } else {
        menuMusic->stop();
        gameMusic->play(SoundBuffer::properties::looped);
        consoleGame->Update(deltaTime);
        consoleGame->Render();
      }

      // Rendering
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      engine.SwapBuffers();
    }

    engine.CleanUp();
    delete consoleGame;
  }

  return 0;
}