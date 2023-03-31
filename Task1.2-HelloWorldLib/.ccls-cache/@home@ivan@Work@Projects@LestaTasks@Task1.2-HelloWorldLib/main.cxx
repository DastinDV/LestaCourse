#include "lib/include/printLib.hxx"

int main(int argc, char* argv[]){


  for (int i = 0; i < argc; i++){
    std::string nextAr(argv[i]);
    bool wasPrinted = printMessage(nextAr);
    if (!wasPrinted) return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
} 
 
