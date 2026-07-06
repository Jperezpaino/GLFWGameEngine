#include "Window.h"

#include <iostream>
#include <stdexcept>

Window* Window::window = nullptr;

Window::Window()
    : width(1920),
      height(1080),
      title("Mario"),
      glfwWindow(nullptr)
{
}

Window* Window::get()
{
    if (window == nullptr)
    {
        window = new Window();
    }

    return window;
}

void Window::run()
{
    std::cout << "Hello GLFW " << glfwGetVersionString() << "!" << std::endl;
    init();
    loop();
}

void Window::init()
{
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    });

    if (!glfwInit())
    {
        throw std::runtime_error("Unable to initialize GLFW.");
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (glfwWindow == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create the GLFW window.");
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);
    glfwShowWindow(glfwWindow);
}

void Window::loop()
{
    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(glfwWindow);
    }
}

Window::~Window()
{
    if (glfwWindow != nullptr)
    {
        glfwDestroyWindow(glfwWindow);
        glfwWindow = nullptr;
    }

    glfwTerminate();
}
