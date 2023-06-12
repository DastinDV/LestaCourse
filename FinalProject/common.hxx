#pragma once
#include "../OpenGL/vertexBuffer.hxx"
#include "../Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"

enum class ETileType { EMPTY, ROAD, EXIT, VERTICAL, HORIZONTAL, CROSS };

struct Tile {
  core::GLTriangle one;
  core::GLTriangle two;

  int i = 0;
  int j = 0;
  ETileType tileType;
  core::VertexBuffer *buf;
  std::vector<float> tilePos;

  bool isWalkable = false;
};