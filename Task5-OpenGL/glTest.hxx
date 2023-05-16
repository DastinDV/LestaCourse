#pragma once

#include "../Task3-Engine/OpenGL/vao.hxx"
#include "../Task3-Engine/OpenGL/vertexBuffer.hxx"
#include "../Task3-Engine/Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"
#include "../Task3-Engine/helper.hxx"

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

class GLGame : public core::Game {
public:
  GLGame();
  void Render() override;
  void Update(float deltaTime) override;
  void Init() override;
  void OnEvent(core::Event &event) override;
  ~GLGame();

private:
  void InitPoints();
  void RenderPoints();
  void UpdatePoints(float deltaTime);

  core::GlRenderer *glRenderer;
  core::VertexBuffer *pointBuffer;

  std::vector<Agent> points;
  float *vertecies = nullptr;
  unsigned int *indeces = nullptr;
  const int pointsNum = 100'000;

  void InitIndexTriangle();
  void RenderIndexTriangle();
  void UpdateIndexTriangle(float deltaTime);

  core::VAO *triangleVAO;
  core::VertexBuffer *triangleBuffer;

  core::VAO *coloredTriangleVAO;
  core::VertexBuffer *coloredTriangleBuffer;
  float *vertecies1 = nullptr;

  float timeSinceRun = 0.0;
};

IZ_DECLSPEC core::Game *CreateGame() { return new GLGame(); }