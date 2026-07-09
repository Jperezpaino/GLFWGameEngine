#include "LevelScene.h"

#include "Window.h"

#include <iostream>

LevelScene::LevelScene() {
  std::cout << "Inside level scene" << std::endl;
  Window::get().setClearColor(1.0f, 1.0f, 1.0f);
}

void LevelScene::update(
    float) {
}
