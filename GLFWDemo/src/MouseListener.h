#pragma once

#include <GLFW/glfw3.h>

class MouseListener {

  public:

    MouseListener(const MouseListener&) = delete;
    MouseListener& operator = (const MouseListener&) = delete;
    MouseListener(MouseListener&&) = delete;
    MouseListener& operator = (MouseListener&&) = delete;

    static MouseListener& get();
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static bool isMouseButtonDown(int button);

  private:

    static constexpr int BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST + 1;
    bool m_mouseButtonPressed[BUTTON_COUNT] = {};

    MouseListener() = default;

};
