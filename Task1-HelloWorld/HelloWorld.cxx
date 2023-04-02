#include <iostream>

int main(){
  std::cout << "Hello World" << std::endl;
  bool outStatus = std::cout.good();
  
  int result = outStatus ? EXIT_SUCCESS : EXIT_FAILURE;
  return result;
}
