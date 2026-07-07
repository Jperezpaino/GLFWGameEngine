#include "KeyListener.h"

KeyListener& KeyListener::get() {
  static KeyListener instance;
  return instance;
}

void KeyListener::keyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods) {
  (void)window;
  (void)scancode;
  (void)mods;

  if ((key < 0)
   || (key >= KEY_COUNT)) {
    return;
  }

  KeyListener& listener = KeyListener::get();

  if (action == GLFW_PRESS) {
    listener.m_keyPressed[key] = true;
  } else if (action == GLFW_RELEASE) {
    listener.m_keyPressed[key] = false;
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
