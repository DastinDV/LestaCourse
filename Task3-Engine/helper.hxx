#pragma once

#include "canvas.hxx"
#include <cmath>

int GetRandomNumber(int min, int max) {
  int num = min + rand() % (max - min + 1);

  return num;
}

unsigned int hash(unsigned int state) {
  state ^= 2747636419u;
  state *= 2654435769u;
  state ^= state >> 16;
  state *= 2654435769u;
  state ^= state >> 16;
  state *= 2654435769u;
  return state;
}

float scaleToRange01(unsigned int state) { return state / 4294967295.0; }

struct Agent {
  core::GlVertex vertex;
  float angle;
  int id;
};

float degreesToRadians(float degrees) { return degrees * M_PI / 180; }