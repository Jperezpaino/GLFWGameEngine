#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

#include <string>

class Window
{
public:
    Window();
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    void run();

private:
    void init();
    void loop();
    void processInput();
    void render();

    int m_width = 1920;
    int m_height = 1080;
    std::string m_title = "Mario";
    GLFWwindow* m_glfwWindow = nullptr;
    bool m_glfwInitialized = false;
};

#endif // WINDOW_H
