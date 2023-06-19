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

  CreateShaders();
  InitUniforms();

  ResizeScreen();
}

void MirrorGame::Render() {

  for (int i = 0; i < tiles.size(); i++) {
    if (tiles[i].tileType == ETileType::ROAD) {
      roadShader.Use();

      tiles[i].buf->Bind();
      glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float),
                               0);
      float u_tilePos[2];
      u_tilePos[0] = tiles[i].tilePos[0];
      u_tilePos[1] = tiles[i].tilePos[1];
      roadShader.SetVec2fvUniform(u_tilePos, "u_tileCoordinate");
      roadShader.SetUniform1f(timeSinceRun, "u_time");
      if (tiles[i].isFlicker)
        roadShader.SetUniform1i(1, "u_isFlicker");
      else
        roadShader.SetUniform1i(0, "u_isFlicker");

      glRenderer->DrawTriangle(tiles[i].buf->GetElementsCount());
      tiles[i].buf->Unbind();
    }
    if (tiles[i].tileType == ETileType::VERTICAL) {
      mirrorShader.Use();
      tiles[i].buf->Bind();
      glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float),
                               0);
      float u_tilePos[2];
      u_tilePos[0] = tiles[i].tilePos[0];
      u_tilePos[1] = tiles[i].tilePos[1];
      mirrorShader.SetVec2fvUniform(u_tilePos, "u_tileCoordinate");
      mirrorShader.SetUniform1f(timeSinceRun, "u_time");
      glRenderer->DrawTriangle(tiles[i].buf->GetElementsCount());
      tiles[i].buf->Unbind();
    }
    if (tiles[i].tileType == ETileType::EXIT) {
      exitShader.Use();
      tiles[i].buf->Bind();
      glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float),
                               0);
      float u_tilePos[2];
      u_tilePos[0] = tiles[i].tilePos[0];
      u_tilePos[1] = tiles[i].tilePos[1];
      exitShader.SetVec2fvUniform(u_tilePos, "u_tileCoordinate");
      exitShader.SetUniform1f(timeSinceRun, "u_time");
      glRenderer->DrawTriangle(tiles[i].buf->GetElementsCount());
      tiles[i].buf->Unbind();
    }
  }

  player->Render(glRenderer);
}

void MirrorGame::Update(float deltaTime) {
  timeSinceRun += deltaTime;
  mirrorShader.Use();
  if (currentMirror) {
    currentMirror->Update(deltaTime, mirrors);
  }
}

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
      if (event.keyBoardInfo->keyCode == core::KeyCode::q) {
        if (currentMirror)
          currentMirror->Reflect(tiles);
      }
    }
  }

  if (event.eventType == core::EventType::mouse_event &&
      event.mouseInfo.has_value()) {
    if (event.mouseInfo->type == core::MouseEventType::button_down) {
      std::cout << "Clicked pos = " << event.mouseInfo->xPos << " "
                << event.mouseInfo->yPos << std::endl;

      auto tilePos = GetTilePosByClickPos(
          event.mouseInfo->xPos, event.mouseInfo->yPos, 32.0, 32.0, screenWidth,
          screenHeight, xSizeCorrection, ySizeCorrection);

      std::cout << "Clicked tilePos = " << tilePos.first << " "
                << tilePos.second << std::endl;

      std::cout << std::endl;
      auto it = std::find_if(
          mirrors.begin(), mirrors.end(), [&tilePos](const Mirror *nextMirror) {
            return nextMirror->GetTilePos().first == tilePos.first &&
                   nextMirror->GetTilePos().second == tilePos.second;
          });

      if (it != mirrors.end()) {
        std::cout << "Changed mirror " << std::endl;
        currentMirror = *it;
        HighlightTiles(currentMirror);
      }
    }
  }
}

MirrorGame::~MirrorGame() {
  std::cout << "Destroy game " << std::endl;

  delete[] exitVertecies;
  delete player;
  for (auto &item : mirrors) {
    delete item;
  }
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
    factor = aspectRatio / targetAR;
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
    for (int i = 0; i < tiles.size(); i++) {
      if (tiles[i].tileType == ETileType::VERTICAL) {
        mirrorShader.SetMatrix4fvUniform(result, "u_translate");
        mirrorShader.SetMatrix4fvUniform(proj, "u_projection");
        mirrorShader.SetVec2fvUniform(u_screenSize, "u_windowSize");
        float u_mirrorPos[2];
        u_mirrorPos[0] = tiles[i].tilePos[0];
        u_mirrorPos[0] = tiles[i].tilePos[1];
        mirrorShader.SetVec2fvUniform(u_mirrorPos, "u_tileCoordinate");
      }
    }

    exitShader.Use();
    exitShader.SetMatrix4fvUniform(result, "u_translate");
    exitShader.SetMatrix4fvUniform(proj, "u_projection");
    exitShader.SetVec2fvUniform(u_screenSize, "u_windowSize");
    exitShader.SetVec2fvUniform(u_exitPos, "u_tileCoordinate");

    playerShader.Use();
    playerShader.SetMatrix4fvUniform(result, "u_translate");
    playerShader.SetMatrix4fvUniform(proj, "u_projection");
    player->SetXSizeCorrection(factor);
    player->AdjustWorldCoordinates();

    xSizeCorrection = factor;
    std::cout << std::endl;
  } else {
    factor = targetAR / aspectRatio;
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
    for (int i = 0; i < tiles.size(); i++) {
      if (tiles[i].tileType == ETileType::VERTICAL) {
        mirrorShader.SetMatrix4fvUniform(result, "u_translate");
        mirrorShader.SetMatrix4fvUniform(proj, "u_projection");
        mirrorShader.SetVec2fvUniform(u_screenSize, "u_windowSize");
        float u_mirrorPos[2];
        u_mirrorPos[0] = tiles[i].tilePos[0];
        u_mirrorPos[0] = tiles[i].tilePos[1];
        mirrorShader.SetVec2fvUniform(u_mirrorPos, "u_tileCoordinate");
      }
    }

    exitShader.Use();
    exitShader.SetMatrix4fvUniform(result, "u_translate");
    exitShader.SetMatrix4fvUniform(proj, "u_projection");
    exitShader.SetVec2fvUniform(u_screenSize, "u_windowSize");
    exitShader.SetVec2fvUniform(u_exitPos, "u_tileCoordinate");

    playerShader.Use();
    playerShader.SetMatrix4fvUniform(result, "u_translate");
    playerShader.SetMatrix4fvUniform(proj, "u_projection");
    player->SetYSizeCorrection(factor);
    player->AdjustWorldCoordinates();

    ySizeCorrection = factor;
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
      newTile.i = i;
      newTile.j = j;
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
  player->SetWorldTransform({playerVertecies[0], playerVertecies[1], 0.0});
}

void MirrorGame::CreateMirror(Tile mirrorTile) {
  Mirror *mirror = new Mirror(mirrorTile, tiles, player);
  mirror->SetMapSizeInTiles(map.GetMapWidth(), map.GetMapHeight());
  mirrors.push_back(mirror);
}

void MirrorGame::HighlightTiles(Mirror *mirror) {
  int xMirrorPos = mirror->GetTilePos().first;
  int yMirrorPos = mirror->GetTilePos().second;

  int radius = mirror->GetRadius();

  std::for_each(tiles.begin(), tiles.end(),
                [](Tile &nextTile) { nextTile.isFlicker = false; });

  for (int i = yMirrorPos - radius; i < yMirrorPos + radius + 1; i++) {
    for (int j = xMirrorPos - radius, k = 0; j < xMirrorPos; j++, k++) {
      if (tiles[i * map.GetMapWidth() + j].tileType == ETileType::ROAD) {
        tiles[i * map.GetMapWidth() + j].isFlicker = true;
      }
    }
  }
  for (int i = yMirrorPos - radius; i < yMirrorPos + radius + 1; i++) {
    for (int j = xMirrorPos + radius, k = 0; j > xMirrorPos; j--, k++) {
      if (tiles[i * map.GetMapWidth() + j].tileType == ETileType::ROAD) {
        tiles[i * map.GetMapWidth() + j].isFlicker = true;
      }
    }
  }
}

void MirrorGame::PushToBuffers() {

  int roadI = 0, mirrorI = 0, exitI = 0;
  exitVertecies = new float[6 * 3];

  for (int i = 0; i < tiles.size(); i++) {
    if (tiles[i].tileType == ETileType::EMPTY) {
      float *emptyVertecies = new float[18];
      int k = 0;
      for (auto pos : tiles[i].one.asBuf()) {
        emptyVertecies[k++] = pos;
      }
      for (auto pos : tiles[i].two.asBuf()) {
        emptyVertecies[k++] = pos;
      }

      tiles[i].buf = new core::VertexBuffer();
      tiles[i].buf->Bind();
      tiles[i].buf->SetData(emptyVertecies, 1 * 2 * 9 * sizeof(float));
      tiles[i].buf->SetElementsCount(1 * 6);
      glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float),
                               0);
      tiles[i].vertecies = emptyVertecies;
      tiles[i].buf->Unbind();

      std::vector<float> pos;
      pos.push_back(emptyVertecies[6]);
      pos.push_back(emptyVertecies[7]);
      tiles[i].tilePos = pos;
    } else if (tiles[i].tileType == ETileType::ROAD) {
      float *tileRoadVertecies = new float[18];
      int k = 0;
      for (auto pos : tiles[i].one.asBuf()) {
        tileRoadVertecies[k++] = pos;
      }
      for (auto pos : tiles[i].two.asBuf()) {
        tileRoadVertecies[k++] = pos;
      }

      tiles[i].buf = new core::VertexBuffer();
      tiles[i].buf->Bind();
      tiles[i].buf->SetData(tileRoadVertecies, 1 * 2 * 9 * sizeof(float));
      tiles[i].buf->SetElementsCount(1 * 6);
      glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float),
                               0);
      tiles[i].vertecies = tileRoadVertecies;
      tiles[i].buf->Unbind();

      std::vector<float> pos;
      pos.push_back(tileRoadVertecies[6]);
      pos.push_back(tileRoadVertecies[7]);
      tiles[i].tilePos = pos;

    } else if (tiles[i].tileType == ETileType::VERTICAL ||
               tiles[i].tileType == ETileType::HORIZONTAL ||
               tiles[i].tileType == ETileType::CROSS) {
      float *mirrorsVertecies = new float[18];
      int k = 0;
      for (auto pos : tiles[i].one.asBuf()) {
        mirrorsVertecies[k++] = pos;
      }
      for (auto pos : tiles[i].two.asBuf()) {
        mirrorsVertecies[k++] = pos;
      }

      tiles[i].buf = new core::VertexBuffer();
      tiles[i].buf->Bind();
      tiles[i].buf->SetData(mirrorsVertecies, 1 * 2 * 9 * sizeof(float));
      tiles[i].buf->SetElementsCount(1 * 6);
      glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float),
                               0);
      tiles[i].buf->Unbind();
      tiles[i].vertecies = mirrorsVertecies;

      std::vector<float> pos;
      pos.push_back(mirrorsVertecies[6]);
      pos.push_back(mirrorsVertecies[7]);
      tiles[i].tilePos = pos;

      CreateMirror(tiles[i]);
    } else if (tiles[i].tileType == ETileType::EXIT) {
      for (auto pos : tiles[i].one.asBuf()) {
        exitVertecies[exitI++] = pos;
      }
      for (auto pos : tiles[i].two.asBuf()) {
        exitVertecies[exitI++] = pos;
      }

      tiles[i].buf = new core::VertexBuffer();
      tiles[i].buf->Bind();
      tiles[i].buf->SetData(exitVertecies,
                            exitTileCount * 2 * 9 * sizeof(float));
      tiles[i].buf->SetElementsCount(exitTileCount * 6);
      glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float),
                               0);

      tiles[i].buf->Unbind();
      tiles[i].vertecies = exitVertecies;

      std::vector<float> pos;
      pos.push_back(exitVertecies[6]);
      pos.push_back(exitVertecies[7]);

      // for uniform to scale shader for an exit tile.
      tiles[i].tilePos = pos;
      tiles[i].tilePos = pos;
    }
  }

  std::cout << "exit coordinate in world = " << u_exitPos[0] << " "
            << u_exitPos[1] << std::endl;
}

void MirrorGame::CreateShaders() {
  roadShader.CreateShaderProgramFromFile(
      "./../../FinalProject/Assets/Shaders/tileVS.txt",
      "./../../FinalProject/Assets/Shaders/roadFS.txt");

  exitShader.CreateShaderProgramFromFile(
      "./../../FinalProject/Assets/Shaders/tileVS.txt",
      "./../../FinalProject/Assets/Shaders/exitFS.txt");

  mirrorShader.CreateShaderProgramFromFile(
      "./../../FinalProject/Assets/Shaders/tileVS.txt",
      "./../../FinalProject/Assets/Shaders/mirrorFS.txt");
}

void MirrorGame::InitUniforms() {

  float *transform = core::Translate(u_defaultTransform);

  roadShader.Use();
  roadShader.SetUniform1f(this->tileSize, "u_tileSize");
  roadShader.SetMatrix4fvUniform(transform, "u_transform");
  roadShader.SetVec4fvUniform(NeonBlueColor, "u_borderColor");
  roadShader.SetUniform1i(0, "u_isFlicker");

  exitShader.Use();
  exitShader.SetUniform1f(this->tileSize, "u_tileSize");
  exitShader.SetVec2fvUniform(u_exitPos, "u_tileCoordinate");
  exitShader.SetMatrix4fvUniform(transform, "u_transform");

  mirrorShader.Use();
  for (int i = 0; i < tiles.size(); i++) {
    if (tiles[i].tileType == ETileType::VERTICAL) {
      mirrorShader.SetUniform1f(this->tileSize, "u_tileSize");
      float u_mirrorPos[2];
      u_mirrorPos[0] = tiles[i].tilePos[0];
      u_mirrorPos[0] = tiles[i].tilePos[1];
      mirrorShader.SetVec2fvUniform(u_mirrorPos, "u_tileCoordinate");
      mirrorShader.SetMatrix4fvUniform(transform, "u_transform");
    }
  }
}
