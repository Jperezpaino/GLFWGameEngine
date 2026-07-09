#include "LevelEditorScene.h"

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

LevelEditorScene::LevelEditorScene() {
  std::cout << "Inside level editor scene" << std::endl;
}

void LevelEditorScene::init() {
  std::cout << "Inicializando LevelEditorScene..." << std::endl;
  Window::get().setClearColor(1.00f, 0.25f, 0.25f);
}

void LevelEditorScene::update(
    float deltaTime) {
  if (!m_changingScene && KeyListener::isKeyPressed(GLFW_KEY_SPACE)) {
    m_changingScene = true;
    std::cout << "Volviendo a la escena del triangulo..." << std::endl;
  }

  if (!m_changingScene) {
    return;
  }

  m_timeToChangeScene -= deltaTime;

  const float transitionDuration = 1.0f;
  const float transitionProgress = std::clamp(1.0f - (m_timeToChangeScene / transitionDuration), 0.0f, 1.0f);
  const float red = lerp(1.00f, 0.02f, transitionProgress);
  const float green = lerp(0.25f, 0.02f, transitionProgress);
  const float blue = lerp(0.25f, 0.06f, transitionProgress);
  Window::get().setClearColor(red, green, blue);

  if (m_timeToChangeScene <= 0.0f) {
    Window::get().changeScene(1);
  }
}

void LevelEditorScene::render() {
  glBegin(GL_QUADS);

  glColor3f(1.0f, 0.2f, 0.2f);
  glVertex2f(-0.35f, -0.35f);

  glColor3f(0.2f, 1.0f, 0.2f);
  glVertex2f(0.35f, -0.35f);

  glColor3f(0.2f, 0.4f, 1.0f);
  glVertex2f(0.35f, 0.35f);

  glColor3f(1.0f, 1.0f, 0.2f);
  glVertex2f(-0.35f, 0.35f);

  glEnd();
  glColor3f(1.0f, 1.0f, 1.0f);
}
