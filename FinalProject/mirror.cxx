#include "mirror.hxx"

Mirror::Mirror() {}

void Mirror::Update(float dt, float timeSinceRun) {
  if (isReflect) {
    int xMirrorPos = mirror.i;
    int yMirrorPos = mirror.j;

    int radius = 1;

    float alpha = (std::sin(timeSinceRun * 0.5f) + 1) / 2;

    for (int i = yMirrorPos - radius; i < yMirrorPos + radius + 1; i++) {
      for (int j = xMirrorPos - radius; j < xMirrorPos; j++) {
        auto fromTile = tiles[i * mapWidth + j];
        auto toTile = tiles[i * mapWidth + xMirrorPos + j];

        core::GLTriangle tr1 = blend(fromTile.one, toTile.one, alpha);
        core::GLTriangle tr2 = blend(fromTile.two, toTile.two, alpha);
      }
    }
  }
}

void Mirror::Reflect(std::vector<Tile> &tiles, Tile &mirror) {
  this->mirror = mirror;
  this->tiles = tiles;
  isReflect = true;
}

void Mirror::SetMapSizeInTiles(int mapWidth, int mapHeight) {
  this->mapWidth = mapWidth;
  this->mapHeight = mapHeight;
}
