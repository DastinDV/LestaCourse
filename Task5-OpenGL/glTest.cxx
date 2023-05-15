#include "glTest.hxx"

#include <iostream>
#include <vector>

GLGame::GLGame() {
  glRenderer = new core::GlRenderer();
  pointBuffer = new core::VertexBuffer();
}

void GLGame::Render() { glRenderer->DrawPoint(vertecies, pointsNum); }

void GLGame::Update(float deltaTime) {
  int i = 0;
  for (auto &point : points) {
    UpdatePoint(point, deltaTime, timeSinceRun);
    vertecies[i++] = point.vertex.x;
    vertecies[i++] = point.vertex.y;
    vertecies[i++] = point.vertex.z;
    // std::cout << "Update points " << std::endl;
  }
  pointBuffer->SetData(vertecies, pointsNum);
  timeSinceRun += deltaTime;
}

void GLGame::Init() {
  glRenderer = new core::GlRenderer();
  pointBuffer = new core::VertexBuffer();
  glRenderer->SetBuffer(pointBuffer);

  points.reserve(pointsNum);

  for (int i = 0; i < pointsNum; i++) {
    Agent newAgent;
    newAgent.vertex = {0.0f, 0.0f, 0.0f};
    newAgent.angle = (float)GetRandomNumber(0, 360);
    newAgent.id = i;
    points.push_back(newAgent);
  }
  vertecies = new float[points.size() * 3];

  pointBuffer->Bind();
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 0, 0);
}

void GLGame::OnEvent(core::Event &event) {}

GLGame::~GLGame() {
  delete vertecies;
  delete pointBuffer;
  delete glRenderer;
}
