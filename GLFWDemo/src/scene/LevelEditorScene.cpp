#include "LevelEditorScene.h"

#include "KeyListener.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

LevelEditorScene::LevelEditorScene() {
  std::cout << "Inside level editor scene" << std::endl;
  Window::get().setClearColor(0.50f, 0.50f, 0.50f);
}

void LevelEditorScene::update(
    float deltaTime) {
  if (!m_changingScene && KeyListener::isKeyPressed(GLFW_KEY_SPACE)) {
    m_changingScene = true;
    std::cout << "Iniciando transicion a la escena de juego..." << std::endl;
  }

  if (!m_changingScene) {
    return;
  }

  m_timeToChangeScene -= deltaTime;

  const float transitionProgress = std::clamp(1.0f - m_timeToChangeScene, 0.0f, 1.0f);
  const float color = 0.18f * (1.0f - transitionProgress);
  Window::get().setClearColor(color, color, color);

  if (m_timeToChangeScene <= 0.0f) {
    Window::get().changeScene(1);
  }
}
