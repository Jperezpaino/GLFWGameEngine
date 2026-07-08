#include "KeyListener.h"

KeyListener& KeyListener::get() {
  static KeyListener instance;
  return instance;
}

void KeyListener::keyCallback(
    GLFWwindow* window,
    int keyCode,
    int scancode,
    int action,
    int mods) {
  (void)window;
  (void)scancode;
  (void)mods;

  if ((keyCode < 0)
   || (keyCode >= KEY_COUNT)) {
    return;
  }

  KeyListener& listener = KeyListener::get();
  if (action == GLFW_PRESS) {
    listener.m_keyPressed[keyCode] = true;
  } else if (action == GLFW_RELEASE) {
    listener.m_keyPressed[keyCode] = false;
  }
}

bool KeyListener::isKeyPressed(
    int keyCode) {
  if ((keyCode < 0)
   || (keyCode >= KEY_COUNT)) {
    return false;
  }

  KeyListener& listener = KeyListener::get();
  return listener.m_keyPressed[keyCode];
}
