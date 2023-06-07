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

  targetAR = static_cast<float>(targetScreenWidth) /
             static_cast<float>(targetScreenHeight);
  auto screen = core::GetScreenSize();
  screenWidth = screen.first;
  screenHeight = screen.second;
  screenSize[0] = screenWidth;
  screenSize[1] = screenHeight;

  CreateTiles();
  PushToBuffers();

  ResizeScreen();
}

void MirrorGame::Render() {
  roadShader.Use();
  roadBuffer.Bind();
  glRenderer.SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  glRenderer.DrawTriangle(roadBuffer.GetElementsCount());
  roadShader.SetVec4fvUniform(RedColor, "u_color");

  mirrorsBuffer.Bind();
  glRenderer.SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  glRenderer.DrawTriangle(mirrorsBuffer.GetElementsCount());
  roadShader.SetVec4fvUniform(GreenColor, "u_color");

  exitShader.Use();
  exitBuffer.Bind();
  glRenderer.SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  glRenderer.DrawTriangle(exitBuffer.GetElementsCount());
  exitShader.SetUniform1f(timeSinceRun, "u_time");
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
}

MirrorGame::~MirrorGame() {}

void MirrorGame::ResizeScreen() {
  aspectRatio =
      static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
  // targetScreenHeight = targetScreenWidth * aspectRatio;
  std::cout << "Window size = " << screenWidth << " " << screenHeight
            << std::endl;
  std::cout << "Aspect ratio = " << aspectRatio << std::endl;

  roadShader.Use();

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
    roadShader.SetMatrix4fvUniform(result, "u_translate");
    roadShader.SetMatrix4fvUniform(proj, "u_projection");

    exitShader.Use();
    exitShader.SetMatrix4fvUniform(result, "u_translate");
    exitShader.SetMatrix4fvUniform(proj, "u_projection");
    exitShader.SetVec2fvUniform(screenSize, "u_windowSize");
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
    roadShader.SetMatrix4fvUniform(proj, "u_projection");
    roadShader.SetMatrix4fvUniform(result, "u_translate");

    exitShader.Use();
    exitShader.SetMatrix4fvUniform(result, "u_translate");
    exitShader.SetMatrix4fvUniform(proj, "u_projection");
    exitShader.SetVec2fvUniform(screenSize, "u_windowSize");
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
    }
    x = 0;
    y += tileSize;
  }
}

void MirrorGame::PushToBuffers() {

  int roadI = 0, mirrorI = 0, exitI = 0;
  roadVertecies = new float[roadTileCount * 6 * 3];
  mirrorsVertecies = new float[mirrorsTileCount * 6 * 3];
  exitVertecies = new float[6 * 3];

  for (int i = 0; i < tiles.size(); i++) {
    if (tiles[i].tileType == ETileType::EMPTY)
      continue;

    else if (tiles[i].tileType == ETileType::ROAD) {
      for (auto pos : tiles[i].one.asBuf()) {
        roadVertecies[roadI++] = pos;
      }
      for (auto pos : tiles[i].two.asBuf()) {
        roadVertecies[roadI++] = pos;
      }
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

  // Road tiles positions
  roadBuffer.Bind();
  roadBuffer.SetData(roadVertecies, roadTileCount * 2 * 9 * sizeof(float));
  roadBuffer.SetElementsCount(roadTileCount * 6);
  glRenderer.SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  roadBuffer.Unbind();

  // // Mirrors tiles positions
  mirrorsBuffer.Bind();
  mirrorsBuffer.SetData(mirrorsVertecies,
                        mirrorsTileCount * 2 * 9 * sizeof(float));
  mirrorsBuffer.SetElementsCount(mirrorsTileCount * 6);
  glRenderer.SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  mirrorsBuffer.Unbind();

  // // Exit tiles positions
  exitBuffer.Bind();
  exitBuffer.SetData(exitVertecies, exitTileCount * 2 * 9 * sizeof(float));
  exitBuffer.SetElementsCount(exitTileCount * 6);
  glRenderer.SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  exitBuffer.Unbind();
}
