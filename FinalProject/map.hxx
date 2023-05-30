#include <string>

class Map {

public:
  int LoadMap(const std::string &filePath);
  ~Map();

private:
  int mapWidth;
  int mapHeight;
  const int tileWidth = 32;
  const int tileHeight = 32;
  int *map = nullptr;
};