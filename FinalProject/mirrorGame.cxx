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
    270.0f, 140.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    370.0f, 140.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    270.0f, 340.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    370.0f, 140.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    370.0f, 340.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
    270.0f, 340.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  // clang-format on

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

  float *proj = core::OrthoProj(0.0f, 640.0f, 640.0f, 0.0f, 0.0f, 100.0f);
  colorShader.SetMatrix4fvUniform(proj, "u_projection");
}

void MirrorGame::Render() { glRenderer.DrawTriangle(6); }

void MirrorGame::Update(float deltaTime) {}

void MirrorGame::OnEvent(core::Event &event, float deltaTime) {}

MirrorGame::~MirrorGame() {}
