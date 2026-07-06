#include "Window.h"
#include <iostream>

int main() {
    try {
        Window* window = Window::get();
        window->run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}