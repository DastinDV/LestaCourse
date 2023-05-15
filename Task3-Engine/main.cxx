#include "canvas.hxx"
#include "engine.hxx"
#include "game.hxx"
#include "glRenderer.hxx"
#include "glad/glad.h"
#include "helper.hxx"
#include "lineRenderer.hxx"
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

void UpdatePoint(Agent &agent, float deltaTime, float count) {

  // auto now = std::chrono::system_clock::now().time_since_epoch();
  // auto count = std::chrono::duration_cast<std::chrono::seconds>(now).count();

  unsigned int random = hash(agent.vertex.y * 640 + agent.vertex.x +
                             hash(agent.id + (count + deltaTime) * 100000));
  core::GlVertex direction;
  direction.x = cos(degreesToRadians(agent.angle));
  direction.y = sin(degreesToRadians(agent.angle));

  core::GlVertex newPos;
  newPos = agent.vertex + direction * 0.1f * deltaTime;

  if (newPos.x < -0.9f || newPos.x >= 0.9f || newPos.y < -0.9f ||
      newPos.y >= 0.9f) {
    // std::cout << agent.id << " " << random << std::endl;
    newPos.x = std::clamp(newPos.x, -0.9f, 0.9f);
    newPos.y = std::clamp(newPos.y, -0.9f, 0.9f);
    float randomAngle = scaleToRange01(random) * 2 * 180;
    // std::cout << randomAngle << std::endl;
    agent.angle = randomAngle;
  }

  agent.vertex = newPos;
}

int main() {
  {
    using namespace core;
    bool quit;
    const char *library_name =
        SDL_GetPlatform() == "Windows"
            ? "libRenderBasics.dll"
            : "../lib/RenderBasicsGame/libRenderBasics.so";
    const char *tmp_library_file = "../lib/RenderBasicsGame/temp.so";

    void *game_library_handle{};

    Engine engine;
    engine.Initialize(640, 480);

    Canvas canvas(640, 480);

    BitFlag flags;
    flags.SetFlag(ETriangleSettings::RASTERIZED);
    flags.SetFlag(ETriangleSettings::INTERPOLATED);

    Game *consoleGame = ReloadGame(nullptr, library_name, tmp_library_file,
                                   engine, game_library_handle);

    consoleGame->Init();
    auto time_during_loading = std::filesystem::last_write_time(library_name);

    float deltaTime = 0.0;
    float timeSinceRun = 0.0;
    long last;

    GlRenderer glRenderer;
    VertexBuffer *pointBuffer = new VertexBuffer();
    glRenderer.SetBuffer(pointBuffer);

    std::vector<Agent> points;
    float *vertecies = nullptr;
    int pointsNum = 0;

    try {
      const std::string pointMovementShaderSource =
          GetShaderSource(".//Shaders//vs.txt");

      const std::string fragmentShaderSource =
          GetShaderSource(".//Shaders//fs.txt");

      int programId =
          CreateShader(pointMovementShaderSource, fragmentShaderSource);
      glUseProgram(programId);

      // int location = glGetUniformLocation(programId, "u_time");
      // assert(location != -1);

      float angle = 90;
      float offset = 0.0f;

      points.reserve(100000);

      for (int i = 0; i < 100000; i++) {
        Agent newAgent;
        // if (i % 10000 == 0)
        //   offset += 0.02f;
        newAgent.vertex = {0.0f, 0.0f, 0.0f};
        newAgent.angle = (float)GetRandomNumber(0, 360);
        newAgent.id = i;
        points.push_back(newAgent);
      }

      pointsNum = points.size();
      vertecies = new float[pointsNum * 3];

    } catch (std::runtime_error err) {
      std::cerr << err.what() << std::endl;
      return EXIT_FAILURE;
    }

    pointBuffer->Bind();
    glRenderer.SetAttribute(0, 3, EGlType::gl_float, 0, 0);

    while (!quit) {
      long now = SDL_GetTicks();

      if (now > last) {
        deltaTime = ((float)(now - last)) / 1000;
        timeSinceRun += deltaTime;
        last = now;
        // glUniform1f(location, timeSinceRun);
        // glUniform1f(angleLocation, angle);
      }

      int i = 0;
      for (auto &point : points) {
        UpdatePoint(point, deltaTime, timeSinceRun);
        vertecies[i++] = point.vertex.x;
        vertecies[i++] = point.vertex.y;
        vertecies[i++] = point.vertex.z;
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
        std::cout << "Key pressed " << event.keyBoardInfo->keyCodeName
                  << std::endl;
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

      consoleGame->OnEvent(event);

      engine.ClearScreen(timeSinceRun);

      // glRenderer.SetAttribute(0, 3, EGlType::gl_float, 0, 0);
      pointBuffer->SetData(vertecies, pointsNum);
      glRenderer.DrawPoint(vertecies, pointsNum);

      // glRenderer.DrawPoint(points1);
      // glRenderer.DrawTriangle(triangle);
      engine.SwapBuffers();
      // consoleGame->Update(deltaTime);
      // consoleGame->Render();
    }

    pointBuffer->Unbind();
    engine.CleanUp();
    delete[] vertecies;
  }

  return 0;
}