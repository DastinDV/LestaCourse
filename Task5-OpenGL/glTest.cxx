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
  if (currentVAOIndex == 2)
    UpdateMorphing(deltaTime);
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
  core::Shader morphShader;

  rgbCirclesShader.CreateShaderProgramFromFile("./Shaders/vs1.txt",
                                               "./Shaders/fs1.txt");

  colorShader.CreateShaderProgramFromFile("./Shaders/vs.txt",
                                          "./Shaders/fs.txt");

  morphShader.CreateShaderProgramFromFile("./Shaders/morpVS.txt",
                                          "./Shaders/morpFS.txt");

  IdToShader[rgbCirclesShader.GetProgramID()] = rgbCirclesShader;
  IdToShader[colorShader.GetProgramID()] = colorShader;
  IdToShader[morphShader.GetProgramID()] = morphShader;

  //////////// triangle ////////////
  core::VAO *triangleVAO = new core::VAO();
  core::VertexBuffer *triangleBuffer = new core::VertexBuffer();

  triangleVAO->SetVertexBuffer(triangleBuffer);

  //////////// coloredTriangle ////////////
  core::VAO *coloredTriangleVAO = new core::VAO();
  core::VertexBuffer *coloredTriangleBuffer = new core::VertexBuffer();
  coloredTriangleVAO->SetVertexBuffer(coloredTriangleBuffer);

  //////////// morphing ////////////
  core::VAO *morphingVAO = new core::VAO();
  core::VertexBuffer *morphingBuffer = new core::VertexBuffer();

  morphingVAO->SetVertexBuffer(morphingBuffer);

  VAOIdToShaderId[triangleVAO->GetID()] = rgbCirclesShader.GetProgramID();
  VAOIdToShaderId[coloredTriangleVAO->GetID()] = colorShader.GetProgramID();
  VAOIdToShaderId[morphingVAO->GetID()] = morphShader.GetProgramID();

  // VAO OBJECTS
  vertexObjects.push_back(triangleVAO);        // 0
  vertexObjects.push_back(coloredTriangleVAO); // 1
  vertexObjects.push_back(morphingVAO);        // 2
}

void GLGame::InitVertexObjects() {
  int vertBufLen, vertBuf1Len = 0, vertBuf2Len = 0;
  int indexCount = 0;
  vertecies = core::ParseVerticies("./verticies.txt", ',', vertBufLen);
  vertecies1 = core::ParseVerticies("./verticies1.txt", ',', vertBuf1Len);
  vertecies2 = generateStarVertices(12, 0.5);
  vertBuf2Len = 12 * 3;

  indeces = core::ParseIndexies("./indeces.txt", ',', indexCount);

  core::VAO *triangleVAO = vertexObjects[0];
  core::VAO *coloredTriangleVAO = vertexObjects[1];
  core::VAO *morphingVAO = vertexObjects[2];

  //////////// triangle ////////////
  triangleVAO->Bind();
  triangleVAO->GetVertexBuffer()->SetData(vertecies,
                                          vertBufLen * sizeof(float));
  triangleVAO->GetVertexBuffer()->SetElementsCount(vertBufLen / 3);
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 0, 0);

  RunShaderByVAOId(triangleVAO->GetID());
  triangleVAO->Unbind();

  //////////// coloredTriangle ////////////
  coloredTriangleVAO->Bind();
  coloredTriangleVAO->GetVertexBuffer()->SetData(vertecies1, vertBuf1Len * 6 *
                                                                 sizeof(float));
  coloredTriangleVAO->GetVertexBuffer()->SetElementsCount(vertBuf1Len / 3);
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 6 * sizeof(float), 0);
  glRenderer->SetAttribute(1, 3, core::EGlType::gl_float, 6 * sizeof(float),
                           (void *)(3 * sizeof(float)));

  RunShaderByVAOId(coloredTriangleVAO->GetID());
  coloredTriangleVAO->Unbind();

  //////////// morphing ////////////
  morphingVAO->Bind();
  morphingVAO->GetVertexBuffer()->SetData(vertecies2,
                                          vertBuf2Len * sizeof(float));
  morphingVAO->GetVertexBuffer()->SetElementsCount(vertBuf2Len);
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 0, 0);

  RunShaderByVAOId(morphingVAO->GetID());
  morphingVAO->Unbind();

  // vertexObjects.push_back(triangleVAO);
  // vertexObjects.push_back(coloredTriangleVAO);
  // vertexObjects.push_back(coloredTriangleVAO);

  BindNextVAO();
}

void GLGame::RunShaderByVAOId(unsigned int id) {
  int connectedShaderId = VAOIdToShaderId.at(id);
  IdToShader.at(connectedShaderId).Use();
}

void GLGame::UpdateMorphing(float deltaTime) {
  // clang-format off
  float targetMorphing[] = {
    -0.5,0.5,0.0,   // 0 
    0.5,0.5,0.0,    // 1
    0.5,-0.5,0.0,   // 2
    -0.5,0.5,0.0,   // 0
    0.5,-0.5,0.0,   // 2
    -0.5,-0.5,0.0   // 3
  };

  std::vector<core::GLTriangle> star;
  std::vector<core::GLTriangle> square;

  int triangleCount = vertexObjects[2]->GetVertexBuffer()->GetElementsCount() / 9;

  for (int n = 0; n < triangleCount; n++) {
    core::GLTriangle nextTriangle;
    for (int i = 0; i < 3; i++){
    nextTriangle.v[i] = core::GlVertex{vertecies2[n*9 + (i * 3)], vertecies2[n*9 + (i * 3 + 1)], vertecies2[n*9 + (i * 3 + 2)]};
    }
    star.push_back(nextTriangle);
  }

  for (int n = 0; n < 2; n++){
    core::GLTriangle nextTriangle;
    for (int i = 0; i < 3; i++){
    nextTriangle.v[i] = core::GlVertex{targetMorphing[n*9 + (i * 3)], targetMorphing[n*9 + (i * 3 + 1)], targetMorphing[n*9 + (i * 3 + 2)]};
    }
    square.push_back(nextTriangle);
  }

  float alpha = (std::sin(timeSinceRun * 0.5f) + 1) / 2;
  float newStar[triangleCount * 9];
  int index = 0;
  for (int i = 0; i <= triangleCount / 2; i+=2){
    core::GLTriangle tr1 = blend(star[i], square[0], alpha);
    core::GLTriangle tr2 = blend(star[i+1], square[1], alpha);

    auto elements = tr1.asBuf();
    auto elements2 = tr2.asBuf();

    for (int j = 0; j < elements.size(); j++){
        newStar[index++] = elements[j];
    }
    for (int j = 0; j < elements2.size(); j++){
        newStar[index++] = elements2[j];
    }
  }

  // float newStar[18];
  // auto elements = tr1.asBuf();
  // auto elements2 = tr2.asBuf();

  // for (int i = 0; i < elements.size(); i++){
  //     newStar[i] = elements[i];
  // }
  // for (int i = 0; i < elements2.size(); i++){
  //     newStar[9 + i] = elements2[i];
  // }

  vertexObjects[2]->GetVertexBuffer()->SetSubData(newStar, triangleCount * 9 * sizeof(float));

  timeSinceRun += deltaTime;
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

void GLGame::RenderIndexTriangle() {
  glRenderer->DrawTriangle(
      vertexObjects[currentVAOIndex]->GetVertexBuffer()->GetElementsCount());
}