#include <iostream>

bool printMessage(std::string &message) {
  std::cout << message << std::endl;
  return std::cout.good();
}
