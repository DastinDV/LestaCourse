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
  float *result = core::Translate(defaultTransform);
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

  pos[0] += ((direction[0] * 64.0));
  pos[1] += ((-direction[1] * 64.0));

  std::cout << "Player position = " << pos[0] << " " << pos[1] << std::endl;
  float *result = core::Translate(pos);
  std::cout << "matrix position = " << result[3 * 4 + 0] << " "
            << result[3 * 4 + 1] << " " << result[3 * 4 + 2] << std::endl;

  result[3 * 4 + 0] /= 640.0;
  result[3 * 4 + 1] /= 480.0;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      std::cout << result[i * 4 + j] << std::endl;
    }
  }
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

void Player::SetTransform(std::vector<float> pos) { this->pos = pos; }
