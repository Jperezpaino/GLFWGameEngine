#include "Application.h"

#include <exception>
#include <iostream>

int main() {
  try {
    Application application;
    application.run();
  } catch (const std::exception& exception) {
    std::cerr << "Error: " << exception.what() << std::endl;
    return -1;
  }

  return 0;
}
