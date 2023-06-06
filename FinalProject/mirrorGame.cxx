#include "mirrorGame.hxx"
#include "../OpenGL/shader.hxx"
#include "../OpenGL/vertexBuffer.hxx"

#include <iostream>
#include <string>

MirrorGame::MirrorGame() { std::cout << "FinalGame created!" << std::endl; }

void MirrorGame::Init() {
  const std::string path = "./../../FinalProject/Assets/map1.txt";

  if (map.LoadMap(path))
    throw std::runtime_error("Couldn't load map");

  std::cout << "FinalGame Init!" << std::endl;

  // clang-format off
  float positions[] = {
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    640.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 480.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    640.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    640.0f, 480.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
    0.0f, 480.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  // clang-format on

  targetAR = static_cast<float>(targetScreenWidth) /
             static_cast<float>(targetScreenHeight);
  auto screen = core::GetScreenSize();
  screenWidth = screen.first;
  screenHeight = screen.second;

  colorShader.CreateShaderProgramFromFile(
      "./../../FinalProject/Assets/Shaders/vs.txt",
      "./../../FinalProject/Assets/Shaders/fs.txt");

  buffer.Bind();
  buffer.SetData(positions, 6 * 6 * sizeof(float));
  buffer.SetElementsCount(6);

  glRenderer.SetAttribute(0, 3, core::EGlType::gl_float, 6 * sizeof(float), 0);
  glRenderer.SetAttribute(1, 3, core::EGlType::gl_float, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));

  colorShader.Use();

  ResizeScreen();
}

void MirrorGame::Render() { glRenderer.DrawTriangle(6); }

void MirrorGame::Update(float deltaTime) {}

void MirrorGame::OnEvent(core::Event &event, float deltaTime) {
  if (event.eventType == core::EventType::window_event &&
      event.windowInfo.has_value()) {
    if (event.windowInfo->type == core::WindowEventType::resized ||
        event.windowInfo->type == core::WindowEventType::maximized) {
      screenWidth = event.windowInfo->width;
      screenHeight = event.windowInfo->height;
      ResizeScreen();
    }
  }
}

MirrorGame::~MirrorGame() {}

void MirrorGame::ResizeScreen() {
  aspectRatio =
      static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
  // targetScreenHeight = targetScreenWidth * aspectRatio;
  std::cout << "Window size = " << screenWidth << " " << screenHeight
            << std::endl;
  std::cout << "Aspect ratio = " << aspectRatio << std::endl;

  colorShader.Use();

  if (aspectRatio >= targetAR) {
    float factor = aspectRatio / targetAR;
    std::cout << "right = " << factor * targetScreenWidth << " " << screenWidth
              << std::endl;
    std::cout << "targetScreenHeight = " << targetScreenHeight << std::endl;
    float *proj =
        core::OrthoProj(factor * 0.0f, factor * targetScreenWidth,
                        targetScreenHeight, 0.0f, nearPlain, farPlain);
    std::vector<float> translate = {
        (((factor * targetScreenWidth) - (targetScreenWidth)) /
         (factor * targetScreenWidth)),
        0.0f, 0.0f};
    float *result = core::Translate(translate);
    std::cout << "translate " << translate[0] << " " << translate[1] << " "
              << translate[2] << std::endl;
    colorShader.SetMatrix4fvUniform(result, "u_translate");
    colorShader.SetMatrix4fvUniform(proj, "u_projection");
  } else {
    float factor = targetAR / aspectRatio;
    std::cout << "Factor " << factor << std::endl;
    float *proj =
        core::OrthoProj(0.0f, targetScreenWidth, factor * targetScreenHeight,
                        factor * 0.0f, nearPlain, farPlain);
    std::vector<float> translate = {
        0.0f,
        (-((factor * targetScreenHeight) - (targetScreenHeight)) /
         (factor * targetScreenHeight)),
        0.0f};
    float *result = core::Translate(translate);
    colorShader.SetMatrix4fvUniform(proj, "u_projection");
    colorShader.SetMatrix4fvUniform(result, "u_translate");
  }
}
