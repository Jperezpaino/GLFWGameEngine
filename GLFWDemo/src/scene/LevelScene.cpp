#include <glad/glad.h>

#include "LevelScene.h"

#include "KeyListener.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

namespace {

  float lerp(
      float start,
      float end,
      float progress) {
    return start + ((end - start) * progress);
  }

}

LevelScene::LevelScene() {
  std::cout << "Inside level scene" << std::endl;
}

void LevelScene::init() {
  std::cout << "Inicializando LevelScene..." << std::endl;
  Window::get().setClearColor(0.25f, 0.25f, 1.00f);
}

void LevelScene::update(
    float deltaTime) {
  if (!m_changingScene && KeyListener::isKeyPressed(GLFW_KEY_SPACE)) {
    m_changingScene = true;
    std::cout << "Volviendo a la escena del cuadrado..." << std::endl;
  }

  if (!m_changingScene) {
    return;
  }

  m_timeToChangeScene -= deltaTime;

  const float transitionDuration = 1.0f;
  const float transitionProgress = std::clamp(1.0f - (m_timeToChangeScene / transitionDuration), 0.0f, 1.0f);
  const float red = lerp(0.25f, 0.02f, transitionProgress);
  const float green = lerp(0.25f, 0.02f, transitionProgress);
  const float blue = lerp(1.00f, 0.06f, transitionProgress);
  Window::get().setClearColor(red, green, blue);

  if (m_timeToChangeScene <= 0.0f) {
    Window::get().changeScene(0);
  }
}

void LevelScene::render() {
  glBegin(GL_TRIANGLES);

  glColor3f(0.9f, 0.1f, 0.2f);
  glVertex2f(0.0f, 0.45f);

  glColor3f(0.1f, 0.6f, 1.0f);
  glVertex2f(-0.45f, -0.35f);

  glColor3f(0.2f, 0.9f, 0.3f);
  glVertex2f(0.45f, -0.35f);

  glEnd();
  glColor3f(1.0f, 1.0f, 1.0f);
}
