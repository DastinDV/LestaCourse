#pragma once
#include "../Renderers/glRenderer.hxx"
#include "../Task3-Engine/engine.hxx"

enum class ETileType { EMPTY, ROAD, EXIT, VERTICAL, HORIZONTAL, CROSS };

struct Tile {
  core::GLTriangle one;
  core::GLTriangle two;

  ETileType tileType;
  bool isWalkable = false;
};