#include "mirrorGame.hxx"
#include <iostream>
#include <string>

MirrorGame::MirrorGame() { std::cout << "FinalGame created!" << std::endl; }

void MirrorGame::Init() {
  const std::string path = "./../../Assets/map1.txt";

  if (map.LoadMap(path))
    throw std::runtime_error("Couldn't load map");

  std::cout << "FinalGame Init!" << std::endl;
}

void MirrorGame::Render() {}

void MirrorGame::Update(float deltaTime) {}

void MirrorGame::OnEvent(core::Event &event, float deltaTime) {}

MirrorGame::~MirrorGame() {}
