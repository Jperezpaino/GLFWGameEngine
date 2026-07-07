#pragma once

#include <GLFW/glfw3.h>

class KeyListener {

  public:

    KeyListener(const KeyListener&) = delete;
    KeyListener& operator=(const KeyListener&) = delete;
    KeyListener(KeyListener&&) = delete;
    KeyListener& operator=(KeyListener&&) = delete;

    static KeyListener& get();
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static bool isKeyPressed(int keyCode);

  private:

    static constexpr int KEY_COUNT = GLFW_KEY_LAST + 1;
    bool m_keyPressed[KEY_COUNT] = {};

    KeyListener() = default;

};
