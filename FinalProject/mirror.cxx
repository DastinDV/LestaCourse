#include "mirror.hxx"
#include <iostream>
#include <math.h>

Mirror::Mirror(Tile &mirror, std::vector<Tile> &tiles, Player *player)
    : mirror(mirror), tiles(tiles), player(player) {
  this->tilePosX = mirror.j;
  this->tilePosY = mirror.i;
}

void Mirror::Update(float dt, std::vector<Mirror *> &mirrors) {
  if (isReflect) {
    accumulateTime += dt * (forwardMove ? 1 : -1);
    int xMirrorPos = tilePosX;
    int yMirrorPos = tilePosY;

    // float alpha = (std::sin(accumulateTime * 0.5f) + 1) / 2 + 1;
    float alpha = 1;
    if (alpha > 0.999 || alpha < 0.001) {
      forwardMove = !forwardMove;
      isReflect = false;

      for (int i = yMirrorPos - radius; i < yMirrorPos + radius + 1; i++) {
        for (int j = xMirrorPos - radius, k = 0; j < xMirrorPos; j++, k++) {
          // Возможно понадобится не заменять и координаты тайла i j
          auto fromTilePos = std::make_pair(tiles[i * mapWidth + j].j,
                                            tiles[i * mapWidth + j].i);
          auto toTilePos =
              std::make_pair(tiles[i * mapWidth + xMirrorPos + radius - k].j,
                             tiles[i * mapWidth + xMirrorPos + radius - k].i);

          if (tiles[i * mapWidth + j].tileType == ETileType::VERTICAL) {
            auto it = std::find_if(
                mirrors.begin(), mirrors.end(),
                [&fromTilePos](const Mirror *nextMirror) {
                  return nextMirror->GetTilePos().first == fromTilePos.first &&
                         nextMirror->GetTilePos().second == fromTilePos.second;
                });

            if (it != mirrors.end()) {
              std::cout << std::endl;
              std::cout << "Updated mirror pos to " << toTilePos.first << " "
                        << toTilePos.second << std::endl;
              auto currentMirror = *it;
              currentMirror->SetTilePos(toTilePos.first, toTilePos.second);
            }
          }

          if (tiles[i * mapWidth + xMirrorPos + radius - k].tileType ==
              ETileType::VERTICAL) {
            auto it = std::find_if(
                mirrors.begin(), mirrors.end(),
                [&toTilePos](const Mirror *nextMirror) {
                  return nextMirror->GetTilePos().first == toTilePos.first &&
                         nextMirror->GetTilePos().second == toTilePos.second;
                });

            if (it != mirrors.end()) {
              std::cout << std::endl;
              std::cout << "Updated mirror pos to " << fromTilePos.first << " "
                        << fromTilePos.second << std::endl;
              auto currentMirror = *it;
              currentMirror->SetTilePos(fromTilePos.first, fromTilePos.second);
            }
          }

          std::swap(tiles[i * mapWidth + j].tileType,
                    tiles[i * mapWidth + xMirrorPos + radius - k].tileType);

          if (tiles[i * mapWidth + j].i == player->GetYTilePos() &&
              tiles[i * mapWidth + j].j == player->GetXTilePos()) {
            float xDiff =
                std::abs(tiles[i * mapWidth + j].j -
                         tiles[i * mapWidth + xMirrorPos + radius - k].j);
            player->Move(tiles, {xDiff, 0});
          }
        }
      }

      for (int i = yMirrorPos - radius; i < yMirrorPos + radius + 1; i++) {
        for (int j = xMirrorPos + radius, k = 0; j > xMirrorPos; j--, k++) {
          if (tiles[i * mapWidth + j].i == player->GetYTilePos() &&
              tiles[i * mapWidth + j].j == player->GetXTilePos()) {
            float xDiff =
                std::abs(tiles[i * mapWidth + j].j -
                         tiles[i * mapWidth + xMirrorPos - radius + k].j);
            player->Move(tiles, {-xDiff, 0});
          }
        }
      }
    }

    //   Left to right reflection
    for (int i = yMirrorPos - radius; i < yMirrorPos + radius + 1; i++) {
      for (int j = xMirrorPos - radius, k = 0; j < xMirrorPos; j++, k++) {
        auto fromTile = tiles[i * mapWidth + j];
        auto toTile = tiles[i * mapWidth + xMirrorPos + radius - k];

        core::GLTriangle tr1 = blend(fromTile.one, toTile.one, alpha);
        core::GLTriangle tr2 = blend(fromTile.two, toTile.two, alpha);
        auto elements = tr1.asBuf();
        auto elements2 = tr2.asBuf();

        float newSquare[18];
        int index = 0;
        for (int j = 0; j < elements.size(); j++) {
          newSquare[index++] = elements[j];
        }
        for (int j = 0; j < elements2.size(); j++) {
          newSquare[index++] = elements2[j];
        }
      }
    }

    // right to left reflection
    for (int i = yMirrorPos - radius; i < yMirrorPos + radius + 1; i++) {
      for (int j = xMirrorPos + radius, k = 0; j > xMirrorPos; j--, k++) {
        auto fromTile = tiles[i * mapWidth + j];
        auto toTile = tiles[i * mapWidth + xMirrorPos - radius + k];

        core::GLTriangle tr1 = blend(fromTile.one, toTile.one, alpha);
        core::GLTriangle tr2 = blend(fromTile.two, toTile.two, alpha);
        auto elements = tr1.asBuf();
        auto elements2 = tr2.asBuf();

        float newSquare[18];
        int index = 0;
        for (int j = 0; j < elements.size(); j++) {
          newSquare[index++] = elements[j];
        }
        for (int j = 0; j < elements2.size(); j++) {
          newSquare[index++] = elements2[j];
        }
      }
    }
  }
}

void Mirror::Reflect(std::vector<Tile> &tiles) { isReflect = true; }

void Mirror::SetMapSizeInTiles(int mapWidth, int mapHeight) {
  this->mapWidth = mapWidth;
  this->mapHeight = mapHeight;
}

int Mirror::GetRadius() const { return radius; }

void Mirror::SetRadius(const int radius) { this->radius = radius; }

std::pair<int, int> Mirror::GetTilePos() const {
  return std::pair<int, int>(this->tilePosX, this->tilePosY);
}

void Mirror::SetTilePos(int posX, int posY) {
  this->tilePosX = posX;
  this->tilePosY = posY;
}
