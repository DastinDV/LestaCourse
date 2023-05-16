#include "glTest.hxx"
#include "shader.hxx"

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
  InitShaders();
  InitVertexObjects();
}

void GLGame::OnEvent(core::Event &event) {
  if (event.eventType == core::EventType::keyboard_event &&
      event.keyBoardInfo.has_value()) {
    if (event.keyBoardInfo->type == core::KeyboardEventType::key_released &&
        event.keyBoardInfo->keyCode == core::KeyCode::space) {
      BindNextVAO();
    }
  }
}

GLGame::~GLGame() {
  delete vertecies;
  delete pointBuffer;
  delete glRenderer;
}

// #################################   Points

void GLGame::InitShaders() {
  core::Shader rgbCirclesShader;
  core::Shader colorShader;

  rgbCirclesShader.CreateShaderProgramFromFile("./Shaders/vs1.txt",
                                               "./Shaders/fs1.txt");

  colorShader.CreateShaderProgramFromFile("./Shaders/vs.txt",
                                          "./Shaders/fs.txt");

  IdToShader[rgbCirclesShader.GetProgramID()] = rgbCirclesShader;
  IdToShader[colorShader.GetProgramID()] = colorShader;

  core::VAO *triangleVAO = new core::VAO();
  core::VertexBuffer *triangleBuffer = new core::VertexBuffer();

  triangleVAO->SetVertexBuffer(triangleBuffer);

  core::VAO *coloredTriangleVAO = new core::VAO();
  core::VertexBuffer *coloredTriangleBuffer = new core::VertexBuffer();
  coloredTriangleVAO->SetVertexBuffer(coloredTriangleBuffer);

  VAOIdToShaderId[triangleVAO->GetID()] = rgbCirclesShader.GetProgramID();
  VAOIdToShaderId[coloredTriangleVAO->GetID()] = colorShader.GetProgramID();

  // VAO OBJECTS
  vertexObjects.push_back(triangleVAO);        // 0
  vertexObjects.push_back(coloredTriangleVAO); // 1
}

void GLGame::InitVertexObjects() {
  int vertexCount, vertex1Count = 0;
  int indexCount = 0;
  vertecies = core::ParseVerticies("./verticies.txt", ',', vertexCount);
  vertecies1 = core::ParseVerticies("./verticies1.txt", ',', vertex1Count);
  indeces = core::ParseIndexies("./indeces.txt", ',', indexCount);

  core::VAO *triangleVAO = vertexObjects[0];
  core::VAO *coloredTriangleVAO = vertexObjects[1];

  triangleVAO->Bind();
  triangleVAO->GetVertexBuffer()->SetData(vertecies,
                                          vertexCount * 3 * sizeof(float));
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 0, 0);

  RunShaderByVAOId(triangleVAO->GetID());
  triangleVAO->Unbind();

  coloredTriangleVAO->Bind();
  coloredTriangleVAO->GetVertexBuffer()->SetData(vertecies1, vertex1Count * 6 *
                                                                 sizeof(float));
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 6 * sizeof(float), 0);
  glRenderer->SetAttribute(1, 3, core::EGlType::gl_float, 6 * sizeof(float),
                           (void *)(3 * sizeof(float)));

  RunShaderByVAOId(coloredTriangleVAO->GetID());
  coloredTriangleVAO->Unbind();

  vertexObjects.push_back(triangleVAO);
  vertexObjects.push_back(coloredTriangleVAO);

  BindNextVAO();
}

void GLGame::RunShaderByVAOId(unsigned int id) {
  int connectedShaderId = VAOIdToShaderId.at(id);
  IdToShader.at(connectedShaderId).Use();
}

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

void GLGame::BindNextVAO() {
  vertexObjects[currentVAOIndex]->Unbind();
  currentVAOIndex++;
  currentVAOIndex %= vertexObjects.size();
  vertexObjects[currentVAOIndex]->Bind();
  RunShaderByVAOId(vertexObjects[currentVAOIndex]->GetID());
  std::cout << currentVAOIndex << std::endl;
}

// ################################    Triangles

void GLGame::RenderIndexTriangle() { glRenderer->DrawTriangle(6); }