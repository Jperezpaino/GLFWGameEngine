#include "MouseListener.h"

MouseListener& MouseListener::get() {
  static MouseListener instance;
  return instance;
}

void MouseListener::mousePositionCallback(
    GLFWwindow* window,
    double xPosition,
    double yPosition) {
  (void)window;

  MouseListener& listener = MouseListener::get();

  listener.m_lastXPosition = listener.m_xPosition;
  listener.m_lastYPosition = listener.m_yPosition;

  listener.m_xPosition = xPosition;
  listener.m_yPosition = yPosition;
}

void MouseListener::mouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods) {
  (void)window;
  (void)mods;

  if ((button < 0)
   || (button >= BUTTON_COUNT)) {
    return;
  }

  MouseListener& listener = MouseListener::get();
  if (action == GLFW_PRESS) {
    listener.m_mouseButtonPressed[button] = true;
  } else if (action == GLFW_RELEASE) {
    listener.m_mouseButtonPressed[button] = false;
  }
}

bool MouseListener::isMouseButtonDown(
    int button) {
  if ((button < 0)
   || (button >= BUTTON_COUNT)) {
    return false;
  }

  MouseListener& listener = MouseListener::get();
  return listener.m_mouseButtonPressed[button];
}

void MouseListener::update() {
  MouseListener& listener = MouseListener::get();

  listener.m_lastXPosition = listener.m_xPosition;
  listener.m_lastYPosition = listener.m_yPosition;
}

float MouseListener::getXPosition() {
  MouseListener& listener = MouseListener::get();
  return static_cast<float>(listener.m_xPosition);
}

float MouseListener::getYPosition() {
  MouseListener& listener = MouseListener::get();
  return static_cast<float>(listener.m_yPosition);
}

float MouseListener::getXDisplacement() {
  MouseListener& listener = MouseListener::get();
  return static_cast<float>(listener.m_xPosition - listener.m_lastXPosition);
}

float MouseListener::getYDisplacement() {
  MouseListener& listener = MouseListener::get();
  return static_cast<float>(listener.m_yPosition - listener.m_lastYPosition);
}
