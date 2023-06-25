#include "mirror.hxx"
#include <iostream>
#include <math.h>
#include <algorithm>

Mirror::Mirror(Tile &mirror, std::vector<Tile> &tiles, Player *player)
    : mirror(mirror), tiles(tiles), player(player) {
  this->tilePosX = mirror.j;
  this->tilePosY = mirror.i;
  this->mirrorType = mirror.tileType;
}

void Mirror::Update(float dt, std::vector<Mirror *> &mirrors) {
  if (isReflect) {
    accumulateTime += dt * (forwardMove ? 1 : -1);
    int xMirrorPos = tilePosX;
    int yMirrorPos = tilePosY;

    // float alpha = (std::sin(accumulateTime * 0.5f) + 1) / 2 + 1;
    float alpha = 1;
    if (mirrorType == ETileType::VERTICAL) {
      if (alpha > 0.999 || alpha < 0.001) {
        bool wasMoved = false;
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

            if (tiles[i * mapWidth + j].tileType == ETileType::EXIT ||
                tiles[i * mapWidth + xMirrorPos + radius - k].tileType ==
                    ETileType::EXIT)
              continue;

            if (tiles[i * mapWidth + j].tileType == ETileType::VERTICAL) {
              auto it = std::find_if(mirrors.begin(), mirrors.end(),
                                     [&fromTilePos](const Mirror *nextMirror) {
                                       return nextMirror->GetTilePos().first ==
                                                  fromTilePos.first &&
                                              nextMirror->GetTilePos().second ==
                                                  fromTilePos.second;
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
                std::cout << "Updated mirror pos to " << fromTilePos.first
                          << " " << fromTilePos.second << std::endl;
                auto currentMirror = *it;
                currentMirror->SetTilePos(fromTilePos.first,
                                          fromTilePos.second);
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
              wasMoved = true;
            }
          }
        }

        if (!wasMoved) {
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
      }
    }

    if (mirrorType == ETileType::HORIZONTAL) {
      bool wasMoved = false;
      forwardMove = !forwardMove;
      isReflect = false;
      for (int i = yMirrorPos - radius, k = 0; i < yMirrorPos; i++, k++) {
        for (int j = xMirrorPos - radius; j < xMirrorPos + radius + 1; j++) {

          auto fromTile = tiles[i * mapWidth + j];
          auto toTile = tiles[(yMirrorPos + radius - k) * mapWidth + j];

          auto fromTilePos = std::make_pair(fromTile.j, fromTile.i);
          auto toTilePos = std::make_pair(toTile.j, toTile.i);

          if (fromTile.tileType == ETileType::EXIT ||
              toTile.tileType == ETileType::EXIT)
            continue;

          if (fromTile.tileType == ETileType::HORIZONTAL ||
              fromTile.tileType == ETileType::VERTICAL) {
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

          if (toTile.tileType == ETileType::HORIZONTAL ||
              toTile.tileType == ETileType::VERTICAL) {
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
                    tiles[(yMirrorPos + radius - k) * mapWidth + j].tileType);

          if (fromTile.i == player->GetYTilePos() &&
              fromTile.j == player->GetXTilePos()) {
            float yDiff =
                std::abs(tiles[i * mapWidth + j].i -
                         tiles[(yMirrorPos + radius - k) * mapWidth + j].i);
            player->Move(tiles, {0, yDiff});
            wasMoved = true;
          }
        }
      }

      if (!wasMoved) {
        for (int i = yMirrorPos + radius, k = 0; i > yMirrorPos; i--, k++) {
          for (int j = xMirrorPos - radius; j < xMirrorPos + radius + 1; j++) {
            if (tiles[i * mapWidth + j].i == player->GetYTilePos() &&
                tiles[i * mapWidth + j].j == player->GetXTilePos()) {
              float yDiff =
                  std::abs(tiles[i * mapWidth + j].i -
                           tiles[(yMirrorPos - radius + k) * mapWidth + j].i);
              player->Move(tiles, {0, -yDiff});
            }
          }
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

void Mirror::SetRadius(const int radius) {
  this->radius = std::clamp(radius, 1, 3);
}

std::pair<int, int> Mirror::GetTilePos() const {
  return std::pair<int, int>(this->tilePosX, this->tilePosY);
}

void Mirror::SetTilePos(int posX, int posY) {
  this->tilePosX = posX;
  this->tilePosY = posY;
}
