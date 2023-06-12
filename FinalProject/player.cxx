#include "player.hxx"
#include <iostream>
#include <vector>

Player::Player(float posX, float posY) : tilePosX(posX), tilePosY(posY) {
  playerVertecies = new float[18];
}

void Player::Init() {
  buf.Bind();
  buf.SetData(playerVertecies, 2 * 9 * sizeof(float));
  buf.SetElementsCount(1 * 6);
  //  нужно ли тут сетать атрибут? Или ток перед отрисовкой
  buf.Unbind();

  std::vector<float> defaultTransform = {0.0, 0.0, 0.0};
  std::vector<float> adjust = {(-16.0 / (640.0 * xSizeCorrection)),
                               (-64.0 / (480.0 * ySizeCorrection)), 0.0};
  float *result = core::Translate(defaultTransform);
  result = core::Translate(adjust);
  result = core::Scale(scale);
  // result = core::Translate(adjust);
  shader.Use();
  shader.SetMatrix4fvUniform(result, "u_transform");
}

void Player::Move(std::vector<Tile> &tiles, std::vector<float> direction) {
  std::vector<float> nextTilePos(3, 0.0f);
  nextTilePos[0] = tilePosX + direction[0];
  nextTilePos[1] = tilePosY + direction[1];

  std::cout << "NextTile: " << nextTilePos[0] << " " << nextTilePos[1]
            << std::endl;
  int x = nextTilePos[0];
  int y = nextTilePos[1];

  if (tiles[y * mapWidth + x].tileType != ETileType::ROAD)
    return;

  tilePosX = nextTilePos[0];
  tilePosY = nextTilePos[1];

  localPos[0] += ((direction[0] * tileSizeX * 2));
  localPos[1] += ((-direction[1] * tileSizeY * 2));

  worldPos[0] += (direction[0] * tileSizeX);
  worldPos[1] += (-direction[1] * tileSizeX);

  std::cout << "Player localPos = " << localPos[0] << " " << localPos[1]
            << std::endl;
  std::cout << "Player worldPos = " << worldPos[0] << " " << worldPos[1]
            << std::endl;
  float *result = core::Translate(localPos);
  std::vector<float> adjust = {(localPos[0] - 16.0), (localPos[1] - 64.0), 0.0};
  result = core::Translate(adjust);
  result = core::Scale(scale);
  std::cout << "matrix position = " << result[3 * 4 + 0] << " "
            << result[3 * 4 + 1] << " " << result[3 * 4 + 2] << std::endl;

  result[3 * 4 + 0] /= (640.0 * xSizeCorrection);
  result[3 * 4 + 1] /= (480.0 * ySizeCorrection);

  // for (int i = 0; i < 4; i++) {
  //   for (int j = 0; j < 4; j++) {
  //     std::cout << result[i * 4 + j] << std::endl;
  //   }
  // }
  shader.SetMatrix4fvUniform(result, "u_transform");
}

void Player::Render(core::GlRenderer *glRenderer) {
  shader.Use();
  buf.Bind();
  glRenderer->SetAttribute(0, 3, core::EGlType::gl_float, 3 * sizeof(float), 0);
  glRenderer->DrawTriangle(buf.GetElementsCount());
}

float *Player::GetVertecies() { return this->playerVertecies; }

void Player::SetShader(core::Shader shader) { this->shader = shader; }

void Player::SetMapSizeInTiles(int mapWidth, int mapHeight) {
  this->mapWidth = mapWidth;
  this->mapHeight = mapHeight;
}

void Player::SetWorldTransform(std::vector<float> worldPos) {
  this->worldPos = worldPos;
}

void Player::SetXSizeCorrection(float factor) {
  this->xSizeCorrection = factor;
  this->ySizeCorrection = 1.0;
}

void Player::SetYSizeCorrection(float factor) {
  this->ySizeCorrection = factor;
  this->xSizeCorrection = 1.0;
}

void Player::AdjustWorldCoordinates() {
  float *result = core::Translate(localPos);
  std::vector<float> adjust = {(localPos[0] - 16.0), (localPos[1] - 64.0), 0.0};
  result = core::Translate(adjust);
  result = core::Scale(scale);

  result[3 * 4 + 0] /= (640.0 * xSizeCorrection);
  result[3 * 4 + 1] /= (480.0 * ySizeCorrection);

  shader.SetMatrix4fvUniform(result, "u_transform");
}
