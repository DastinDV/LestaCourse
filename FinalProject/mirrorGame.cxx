#include "mirrorGame.hxx"
#include "../OpenGL/shader.hxx"
#include "../OpenGL/vertexBuffer.hxx"

#include <algorithm>
#include <iostream>
#include <string>

MirrorGame::MirrorGame() { std::cout << "FinalGame created!" << std::endl; }

void MirrorGame::Init() {
  const std::string path = "./../../FinalProject/Assets/map1.txt";
  glRenderer = new core::GlRenderer();

  if (map.LoadMap(path))
    throw std::runtime_error("Couldn't load map");

  std::cout << "FinalGame Init!" << std::endl;

  targetAR = static_cast<float>(targetScreenWidth) /
             static_cast<float>(targetScreenHeight);
  auto screen = core::GetScreenSize();
  screenWidth = screen.first;
  screenHeight = screen.second;
  u_screenSize[0] = static_cast<float>(screenWidth);
  u_screenSize[1] = static_cast<float>(screenHeight);

  CreateTiles();
  PushToBuffers();
  InitUniforms();

  ResizeScreen();
}

void MirrorGame::Render() {

  roadShader.Use();
  for (int i = 0; i < roadTileCount; i++) {
    roadBuffers[i]->Bind();
    glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float),
                             0);
    float u_tilePos[2];
    u_tilePos[0] = u_roadPos[i][0];
    u_tilePos[1] = u_roadPos[i][1];
    roadShader.SetVec2fvUniform(u_tilePos, "u_tileCoordinate");
    glRenderer->DrawTriangle(roadBuffers[i]->GetElementsCount());
    roadBuffers[i]->Unbind();
  }

  mirrorShader.Use();
  mirrorsBuffer.Bind();
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  glRenderer->DrawTriangle(mirrorsBuffer.GetElementsCount());
  mirrorShader.SetUniform1f(timeSinceRun, "u_time");

  exitShader.Use();
  exitBuffer.Bind();
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  glRenderer->DrawTriangle(exitBuffer.GetElementsCount());
  exitShader.SetUniform1f(timeSinceRun, "u_time");

  player->Render(glRenderer);
}

void MirrorGame::Update(float deltaTime) { timeSinceRun += deltaTime; }

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

  if (event.eventType == core::EventType::keyboard_event &&
      event.keyBoardInfo.has_value()) {
    if (event.keyBoardInfo->type == core::KeyboardEventType::key_pressed) {
      if (event.keyBoardInfo->keyCode == core::KeyCode::d) {
        player->Move(tiles, {1, 0});
      }
      if (event.keyBoardInfo->keyCode == core::KeyCode::a) {
        player->Move(tiles, {-1, 0});
      }
      if (event.keyBoardInfo->keyCode == core::KeyCode::w) {
        player->Move(tiles, {0, -1});
      }
      if (event.keyBoardInfo->keyCode == core::KeyCode::s) {
        player->Move(tiles, {0, 1});
      }
    }
  }
}

MirrorGame::~MirrorGame() {
  std::cout << "Destroy game " << std::endl;
  for (auto &roadBuffer : roadBuffers)
    delete roadBuffer;

  for (auto &vertecies : roadVertecies)
    delete[] vertecies;

  delete[] exitVertecies;
  delete[] mirrorsVertecies;
}

void MirrorGame::ResizeScreen() {
  aspectRatio =
      static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
  // targetScreenHeight = targetScreenWidth * aspectRatio;
  u_screenSize[0] = static_cast<float>(screenWidth);
  u_screenSize[1] = static_cast<float>(screenHeight);
  std::cout << "Window size = " << screenWidth << " " << screenHeight
            << std::endl;
  std::cout << "Aspect ratio = " << aspectRatio << std::endl;

  roadShader.Use();

  if (aspectRatio >= targetAR) {
    float factor = aspectRatio / targetAR;
    std::cout << "factor = " << factor << std::endl;
    std::cout << "left = " << factor * targetScreenWidth - targetScreenWidth
              << std::endl;
    std::cout << "right = " << factor * targetScreenWidth << " " << screenWidth
              << std::endl;
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

    roadShader.SetMatrix4fvUniform(result, "u_translate");
    roadShader.SetMatrix4fvUniform(proj, "u_projection");
    roadShader.SetVec2fvUniform(u_screenSize, "u_windowSize");

    mirrorShader.Use();
    mirrorShader.SetMatrix4fvUniform(result, "u_translate");
    mirrorShader.SetMatrix4fvUniform(proj, "u_projection");
    mirrorShader.SetVec2fvUniform(u_screenSize, "u_windowSize");
    mirrorShader.SetVec2fvUniform(u_mirrorPos, "u_tileCoordinate");

    exitShader.Use();
    exitShader.SetMatrix4fvUniform(result, "u_translate");
    exitShader.SetMatrix4fvUniform(proj, "u_projection");
    exitShader.SetVec2fvUniform(u_screenSize, "u_windowSize");
    exitShader.SetVec2fvUniform(u_exitPos, "u_tileCoordinate");

    playerShader.Use();
    playerShader.SetMatrix4fvUniform(result, "u_translate");
    playerShader.SetMatrix4fvUniform(proj, "u_projection");
    // playerShader.SetVec2fvUniform(u_screenSize, "u_windowSize");
    // playerShader.SetVec2fvUniform(u_mirrorPos, "u_tileCoordinate");
    std::cout << std::endl;
  } else {
    float factor = targetAR / aspectRatio;
    std::cout << "factor =" << factor << std::endl;
    std::cout << "bottom = " << factor * targetScreenHeight << " "
              << screenWidth << std::endl;
    float *proj =
        core::OrthoProj(0.0f, targetScreenWidth, factor * targetScreenHeight,
                        factor * 0.0f, nearPlain, farPlain);
    std::vector<float> translate = {
        0.0f,
        (-((factor * targetScreenHeight) - (targetScreenHeight)) /
         (factor * targetScreenHeight)),
        0.0f};

    std::cout << "translate " << translate[0] << " " << translate[1] << " "
              << translate[2] << std::endl;
    float *result = core::Translate(translate);
    roadShader.SetMatrix4fvUniform(proj, "u_projection");
    roadShader.SetMatrix4fvUniform(result, "u_translate");
    roadShader.SetVec2fvUniform(u_screenSize, "u_windowSize");

    mirrorShader.Use();
    mirrorShader.SetMatrix4fvUniform(result, "u_translate");
    mirrorShader.SetMatrix4fvUniform(proj, "u_projection");
    mirrorShader.SetVec2fvUniform(u_screenSize, "u_windowSize");
    mirrorShader.SetVec2fvUniform(u_mirrorPos, "u_tileCoordinate");

    exitShader.Use();
    exitShader.SetMatrix4fvUniform(result, "u_translate");
    exitShader.SetMatrix4fvUniform(proj, "u_projection");
    exitShader.SetVec2fvUniform(u_screenSize, "u_windowSize");
    exitShader.SetVec2fvUniform(u_exitPos, "u_tileCoordinate");

    playerShader.Use();
    playerShader.SetMatrix4fvUniform(result, "u_translate");
    playerShader.SetMatrix4fvUniform(proj, "u_projection");
    std::cout << std::endl;
  }
}

void MirrorGame::CreateTiles() {

  int mapWidth = map.GetMapWidth();
  int mapHeight = map.GetMapHeight();

  int *a_tiles = map.GetMap();

  float x = 0;
  float y = 0;
  for (int i = 0; i < mapHeight; i++) {
    for (int j = 0; j < mapWidth; j++) {
      Tile newTile;
      newTile.tileType = static_cast<ETileType>(a_tiles[i * mapWidth + j]);

      if (newTile.tileType == ETileType::ROAD)
        roadTileCount++;
      if (newTile.tileType == ETileType::VERTICAL ||
          newTile.tileType == ETileType::HORIZONTAL ||
          newTile.tileType == ETileType::CROSS)
        mirrorsTileCount++;
      // Left-Up part of Tile
      //   --------
      //  |....... |
      //  |.....   |
      //  |..      |
      //   --------
      newTile.one.v[0].x = x;
      newTile.one.v[0].y = y;

      newTile.one.v[1].x = x + tileSize;
      newTile.one.v[1].y = y;

      newTile.one.v[2].x = x;
      newTile.one.v[2].y = y + tileSize;

      // Right-Down part of Tile
      //   --------
      //  |     ...|
      //  |   .....|
      //  | .......|
      //   --------
      newTile.two.v[0].x = x + tileSize;
      newTile.two.v[0].y = y;

      newTile.two.v[1].x = x + tileSize;
      newTile.two.v[1].y = y + tileSize;

      newTile.two.v[2].x = x;
      newTile.two.v[2].y = y + tileSize;

      tiles.push_back(newTile);
      x += tileSize;

      if (i == map.GetPlayerTilePosY() && j == map.GetPlayerTilePosX())
        CreatePlayer(newTile);
    }
    x = 0;
    y += tileSize;
  }
}

void MirrorGame::CreatePlayer(Tile playerTile) {

  playerShader.CreateShaderProgramFromFile(
      "./../../FinalProject/Assets/Shaders/tileVS.txt",
      "./../../FinalProject/Assets/Shaders/playerFS.txt");

  player = new Player(map.GetPlayerTilePosX(), map.GetPlayerTilePosY());
  player->SetShader(playerShader);

  float *playerVertecies = player->GetVertecies();

  int k = 0;
  for (auto pos : playerTile.one.asBuf()) {
    playerVertecies[k++] = pos;
  }
  for (auto pos : playerTile.two.asBuf()) {
    playerVertecies[k++] = pos;
  }

  player->Init();
  player->SetMapSizeInTiles(map.GetMapWidth(), map.GetMapHeight());
  // player->SetTransform({playerVertecies[0], playerVertecies[1], 0.0});
}

void MirrorGame::PushToBuffers() {

  int roadI = 0, mirrorI = 0, exitI = 0;
  mirrorsVertecies = new float[mirrorsTileCount * 6 * 3];
  exitVertecies = new float[6 * 3];

  for (int i = 0; i < tiles.size(); i++) {
    if (tiles[i].tileType == ETileType::EMPTY)
      continue;

    else if (tiles[i].tileType == ETileType::ROAD) {
      float *tileRoadVertecies = new float[18];
      int k = 0;
      for (auto pos : tiles[i].one.asBuf()) {
        tileRoadVertecies[k++] = pos;
      }
      for (auto pos : tiles[i].two.asBuf()) {
        tileRoadVertecies[k++] = pos;
      }
      roadVertecies.push_back(tileRoadVertecies);
    } else if (tiles[i].tileType == ETileType::VERTICAL ||
               tiles[i].tileType == ETileType::HORIZONTAL ||
               tiles[i].tileType == ETileType::CROSS) {
      for (auto pos : tiles[i].one.asBuf()) {
        mirrorsVertecies[mirrorI++] = pos;
      }
      for (auto pos : tiles[i].two.asBuf()) {
        mirrorsVertecies[mirrorI++] = pos;
      }
    } else if (tiles[i].tileType == ETileType::EXIT) {
      for (auto pos : tiles[i].one.asBuf()) {
        exitVertecies[exitI++] = pos;
      }
      for (auto pos : tiles[i].two.asBuf()) {
        exitVertecies[exitI++] = pos;
      }
    }
  }

  roadShader.CreateShaderProgramFromFile(
      "./../../FinalProject/Assets/Shaders/tileVS.txt",
      "./../../FinalProject/Assets/Shaders/roadFS.txt");

  exitShader.CreateShaderProgramFromFile(
      "./../../FinalProject/Assets/Shaders/tileVS.txt",
      "./../../FinalProject/Assets/Shaders/exitFS.txt");

  mirrorShader.CreateShaderProgramFromFile(
      "./../../FinalProject/Assets/Shaders/tileVS.txt",
      "./../../FinalProject/Assets/Shaders/mirrorFS.txt");

  for (int i = 0; i < roadTileCount; i++) {
    std::vector<float> pos;
    pos.push_back(roadVertecies[i][6]);
    pos.push_back(roadVertecies[i][7]);
    u_roadPos.push_back(pos);
  }

  // Road tiles positions
  for (int i = 0; i < roadTileCount; i++) {
    core::VertexBuffer *roadBuffer = new core::VertexBuffer();
    roadBuffer->Bind();
    roadBuffer->SetData(roadVertecies[i], 1 * 2 * 9 * sizeof(float));
    roadBuffer->SetElementsCount(1 * 6);
    glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float),
                             0);
    roadBuffers.push_back(roadBuffer);
    roadBuffer->Unbind();
  }

  // Mirrors tiles positions
  mirrorsBuffer.Bind();
  mirrorsBuffer.SetData(mirrorsVertecies,
                        mirrorsTileCount * 2 * 9 * sizeof(float));
  mirrorsBuffer.SetElementsCount(mirrorsTileCount * 6);
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  mirrorsBuffer.Unbind();

  u_mirrorPos[0] = mirrorsVertecies[6];
  u_mirrorPos[1] = mirrorsVertecies[7];

  // Exit tiles positions
  exitBuffer.Bind();
  exitBuffer.SetData(exitVertecies, exitTileCount * 2 * 9 * sizeof(float));
  exitBuffer.SetElementsCount(exitTileCount * 6);
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  exitBuffer.Unbind();

  // for uniform to scale shader for an exit tile.
  u_exitPos[0] = exitVertecies[6];
  u_exitPos[1] = exitVertecies[7];
  std::cout << "exit coordinate in world = " << u_exitPos[0] << " "
            << u_exitPos[1] << std::endl;
}

void MirrorGame::InitUniforms() {

  float *transform = core::Translate(u_defaultTransform);

  roadShader.Use();
  roadShader.SetUniform1f(this->tileSize, "u_tileSize");
  roadShader.SetMatrix4fvUniform(transform, "u_transform");

  exitShader.Use();
  exitShader.SetUniform1f(this->tileSize, "u_tileSize");
  exitShader.SetVec2fvUniform(u_exitPos, "u_tileCoordinate");
  exitShader.SetMatrix4fvUniform(transform, "u_transform");

  mirrorShader.Use();
  mirrorShader.SetUniform1f(this->tileSize, "u_tileSize");
  mirrorShader.SetVec2fvUniform(u_mirrorPos, "u_tileCoordinate");
  mirrorShader.SetMatrix4fvUniform(transform, "u_transform");
}
