#include "mirror.hxx"
#include <iostream>
#include <math.h>

Mirror::Mirror(Tile &mirror) : mirror(mirror) {}

void Mirror::Update(float dt) {
  if (isReflect) {
    accumulateTime += dt * (forwardMove ? 1 : -1);
    int xMirrorPos = mirror.j;
    int yMirrorPos = mirror.i;

    int radius = 3;

    float alpha = (std::sin(accumulateTime * 0.5f) + 1) / 2;
    std::cout << alpha << std::endl;

    if (alpha > 0.999 || alpha < 0.001) {
      forwardMove = !forwardMove;
      isReflect = false;
    }
    //  float alpha = manhatten_distance();
    //   Left to right reflection
    for (int i = yMirrorPos - radius; i < yMirrorPos + radius + 1; i++) {
      for (int j = xMirrorPos - radius; j < xMirrorPos; j++) {
        auto fromTile = tiles[i * mapWidth + j];
        auto toTile = tiles[i * mapWidth + j + radius + 1];

        core::GLTriangle tr1 = blend(fromTile.one, toTile.two, alpha);
        core::GLTriangle tr2 = blend(fromTile.two, toTile.one, alpha);
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

        if (fromTile.buf) {
          fromTile.buf->Bind();
          fromTile.buf->SetSubData(newSquare, 2 * 9 * sizeof(float));
          fromTile.buf->Unbind();
        }
      }
    }

    // right to left reflection
    for (int i = yMirrorPos - radius; i < yMirrorPos + radius + 1; i++) {
      for (int j = xMirrorPos + radius; j > xMirrorPos; j--) {
        auto fromTile = tiles[i * mapWidth + j];
        auto toTile = tiles[i * mapWidth + j - radius - 1];

        core::GLTriangle tr1 = blend(fromTile.one, toTile.two, alpha);
        core::GLTriangle tr2 = blend(fromTile.two, toTile.one, alpha);
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

        if (fromTile.buf) {
          fromTile.buf->Bind();
          fromTile.buf->SetSubData(newSquare, 2 * 9 * sizeof(float));
          fromTile.buf->Unbind();
        }
      }
    }
  }
}

void Mirror::Reflect(std::vector<Tile> &tiles) {
  this->tiles = tiles;
  isReflect = true;
}

void Mirror::SetMapSizeInTiles(int mapWidth, int mapHeight) {
  this->mapWidth = mapWidth;
  this->mapHeight = mapHeight;
}
