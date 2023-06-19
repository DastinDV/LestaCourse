#include "map.hxx"
#include <fstream>
#include <iostream>
#include <sstream>

int Map::LoadMap(const std::string &filePath) {
  try {

    if (map)
      delete[] map;

    std::ifstream infile(filePath);

    std::string line;
    std::string buf;
    int lineCounter = 0;
    int i = 0;
    if (!infile.is_open())
      throw std::runtime_error("Map file couldn't be opened! ");

    while (std::getline(infile, line)) {
      std::istringstream iss(line);
      if (lineCounter == 0) {
        iss >> buf >> mapWidth;
      } else if (lineCounter == 1) {
        iss >> buf >> mapHeight;
        map = new int[mapHeight * mapWidth];
      } else if (lineCounter == 2) {
        iss >> buf >> playerPosX;
      } else if (lineCounter == 3) {
        iss >> buf >> playerPosY;
      } else {
        std::string nextTileID;
        while (getline(iss, nextTileID, ' ')) {
          map[i++] = atoi(nextTileID.c_str());
        }
      }
      lineCounter++;
    }
  } catch (const std::system_error &e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const std::runtime_error err) {
    std::cout << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int *Map::GetMap() { return this->map; }

int Map::GetMapWidth() const { return this->mapWidth; }

int Map::GetMapHeight() const { return this->mapHeight; }

int Map::GetPlayerTilePosX() const { return this->playerPosX; }

int Map::GetPlayerTilePosY() const { return this->playerPosY; }

Map::~Map() { delete[] map; }
