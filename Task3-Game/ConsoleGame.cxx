#include "ConsoleGame.hxx"
#include <chrono>
#include <iostream>
#include <thread>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
//#define PBSTR "************************************************************"

ConsoleGame::ConsoleGame() : percentage{0} {}

void ConsoleGame::Init() {}

void ConsoleGame::Render() {
  PrintProgress(percentage);
  using namespace std::chrono;
  // std::this_thread::sleep_for(milliseconds(200));
  // system("clear");
}

void ConsoleGame::Update() {
  percentage++;
  percentage %= 100;
  using namespace std::chrono;
  std::this_thread::sleep_for(milliseconds(200));
}

void ConsoleGame::OnEvent(core::event &event) {
  // std::cout << "Event" << std::endl;
}

void ConsoleGame::PrintProgress(int percentage) {
  // std::cout << PBWIDTH << std::endl;
  int val = (int)(percentage);
  int lpad = (val * PBWIDTH) / 100;
  int rpad = PBWIDTH - lpad;
  system("clear");
  printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush(stdout);
}