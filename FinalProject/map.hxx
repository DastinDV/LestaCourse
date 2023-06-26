#include <string>

class Map {

public:
  int LoadMap(const std::string &filePath);
  int *GetMap();
  int GetMapWidth() const;
  int GetMapHeight() const;
  int GetPlayerTilePosX() const;
  int GetPlayerTilePosY() const;

  ~Map();

private:
  int mapWidth;
  int mapHeight;
  //const int tileWidth = 32;
  //const int tileHeight = 32;

  int playerPosX = 0;
  int playerPosY = 0;
  int *map = nullptr;
};