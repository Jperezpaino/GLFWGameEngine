#pragma once

#include <GLFW/glfw3.h>

class MouseListener {

  public:

    MouseListener(const MouseListener&) = delete;
    MouseListener& operator = (const MouseListener&) = delete;
    MouseListener(MouseListener&&) = delete;
    MouseListener& operator = (MouseListener&&) = delete;

    static MouseListener& get();
    static void mousePositionCallback(GLFWwindow* window, double xPosition, double yPosition);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    static bool isMouseButtonDown(int button);
    static void update();
    static float getXPosition();
    static float getYPosition();
    static float getYDisplacement();
    static float getXDisplacement();
    static float getScrollXPosition();
    static float getScrollYPosition();
    static bool isDragging();

  private:

    static constexpr int BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST + 1;
    bool m_mouseButtonPressed[BUTTON_COUNT] = {};
    double m_xPosition = 0.0;
    double m_yPosition = 0.0;
    double m_lastXPosition = 0.0;
    double m_lastYPosition = 0.0;
    double m_scrollXPosition = 0.0;
    double m_scrollYPosition = 0.0;
    bool m_dragging = false;

    MouseListener() = default;

};
