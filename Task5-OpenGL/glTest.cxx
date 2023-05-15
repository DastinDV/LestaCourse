#include "glTest.hxx"

#include <iostream>
#include <vector>

GLGame::GLGame() {
  glRenderer = new core::GlRenderer();
  pointBuffer = new core::VertexBuffer();
}

void GLGame::Render() { /*RenderPoints();*/
  RenderIndexTriangle();
}

void GLGame::Update(float deltaTime) { /*UpdatePoints(deltaTime);*/
}

void GLGame::Init() {
  /*InitPoints();*/
  InitIndexTriangle();
}

void GLGame::OnEvent(core::Event &event) {}

GLGame::~GLGame() {
  delete vertecies;
  delete pointBuffer;
  delete glRenderer;
}

// #################################   Points

void GLGame::InitPoints() {
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

void GLGame::RenderPoints() { glRenderer->DrawPoint(pointsNum); }

void GLGame::UpdatePoints(float deltaTime) {
  int i = 0;
  for (auto &point : points) {
    UpdatePoint(point, deltaTime, timeSinceRun);
    vertecies[i++] = point.vertex.x;
    vertecies[i++] = point.vertex.y;
    vertecies[i++] = point.vertex.z;
    // std::cout << "Update points " << std::endl;
  }
  pointBuffer->SetData(vertecies, pointsNum * 3);
  timeSinceRun += deltaTime;
}

// ################################    Triangles

void GLGame::InitIndexTriangle() {
  int vertexCount = 0;
  int indexCount = 0;
  vertecies = core::ParseVerticies("./verticies.txt", ',', vertexCount);
  indeces = core::ParseIndexies("./indeces.txt", ',', indexCount);

  triangleVAO = new core::VAO();
  triangleBuffer = new core::VertexBuffer();
  triangleVAO->SetVertexBuffer(triangleBuffer);

  triangleVAO->Bind();

  triangleBuffer->SetData(vertecies, vertexCount * 3 * sizeof(float));
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 0, 0);
}

void GLGame::RenderIndexTriangle() { glRenderer->DrawTriangle(vertexCount); }

void GLGame::UpdateIndexTriangle(float deltaTime) {}