#include "MouseListener.h"

MouseListener& MouseListener::get() {
  static MouseListener instance;
  return instance;
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
