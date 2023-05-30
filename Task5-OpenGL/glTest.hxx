#pragma once

#include "../Task3-Engine/OpenGL/shader.hxx"
#include "../Task3-Engine/OpenGL/vao.hxx"
#include "../Task3-Engine/OpenGL/vertexBuffer.hxx"
#include "../Task3-Engine/Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"
#include "../Task3-Engine/helper.hxx"

#include <iostream>
#include <unordered_map>
#include <unordered_set>

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

core::GlVertex blend_vertex(const core::GlVertex &vl, const core::GlVertex &vr,
                            const float a) {
  core::GlVertex r;
  r.x = (1.0f - a) * vl.x + a * vr.x;
  r.y = (1.0f - a) * vl.y + a * vr.y;
  r.z = 0;
  return r;
}

core::GLTriangle blend(const core::GLTriangle &tl, const core::GLTriangle &tr,
                       const float a) {
  core::GLTriangle r;
  r.v[0] = blend_vertex(tl.v[0], tr.v[0], a);
  r.v[1] = blend_vertex(tl.v[1], tr.v[1], a);
  r.v[2] = blend_vertex(tl.v[2], tr.v[2], a);
  return r;
}

float *generateStarVertices(int n, float radius) {
  float *positions = new float[n * 3];
  int triangleCount = n / 3;
  float start_angle = M_PI_2;
  float step = 2 * M_PI / n;

  int k = 0;
  while (k < triangleCount) {
    for (int i = 0; i < 3; i++) {
      float x = radius * cos(start_angle + (2 * M_PI * i / 3));
      float y = radius * sin(start_angle + (2 * M_PI * i / 3));
      positions[k * 9 + (i * 3)] = x;
      positions[k * 9 + (i * 3 + 1)] = y;
      positions[k * 9 + (i * 3 + 2)] = 0.0;
    }
    start_angle += step;
    k++;
  }

  return positions;
}

class GLGame : public core::Game {
public:
  GLGame();
  void Render() override;
  void Update(float deltaTime) override;
  void Init() override;
  void OnEvent(core::Event &event, float deltaTime) override;
  ~GLGame();

private:
  void InitShaders();
  void InitVertexObjects();
  void BindNextVAO();
  void RunShaderByVAOId(unsigned int id);

  void UpdateMorphing(float deltaTime);
  void UpdateMovement(float deltaTime);

  void InitPoints();
  void RenderPoints();
  void UpdatePoints(float deltaTime);

  std::unordered_map<unsigned int, core::Shader> IdToShader;
  std::unordered_map<unsigned int, unsigned int> VAOIdToShaderId;
  std::vector<core::VAO *> vertexObjects;
  int currentVAOIndex = 0;

  core::GlRenderer *glRenderer;
  core::VertexBuffer *pointBuffer;

  float *vertecies = nullptr;
  float *vertecies1 = nullptr;
  float *vertecies2 = nullptr;
  float *vertecies3 = nullptr;
  float *vertecies4 = nullptr;

  std::vector<Agent> points;
  std::vector<float> currentBoxPos;
  float currentAngle = 0.0f;
  float currentScale = 1.0f;

  unsigned int *indeces = nullptr;
  const int pointsNum = 100'000;

  void RenderIndexTriangle();

  float timeSinceRun = 0.0;
};

IZ_DECLSPEC core::Game *CreateGame() { return new GLGame(); }