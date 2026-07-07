#include "Window.h"

#include <iostream>
#include <stdexcept>

Window::Window() = default;

Window::~Window() {
  if (m_glfwWindow != nullptr) {
    glfwDestroyWindow(m_glfwWindow);
    m_glfwWindow = nullptr;
  }

  if (m_glfwInitialized) {
    glfwTerminate();
    m_glfwInitialized = false;
  }
}

void Window::run() {
  std::cout << "Hello GLFW " << glfwGetVersionString() << "!" << std::endl;

  init();
  loop();
}

void Window::init() {
  glfwSetErrorCallback(
    [](int error, const char* description) {
      std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    }
  );

  if (!glfwInit()) {
    throw std::runtime_error("Unable to initialize GLFW.");
  }

  m_glfwInitialized = true;

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

  m_glfwWindow = glfwCreateWindow(
    m_width,
    m_height,
    m_title.c_str(),
    nullptr,
    nullptr
  );

  if (m_glfwWindow == nullptr) {
    throw std::runtime_error("Failed to create the GLFW window.");
  }

  glfwMakeContextCurrent(m_glfwWindow);
  glfwSwapInterval(1);
  glfwShowWindow(m_glfwWindow);
}

void Window::loop() {
  while (!glfwWindowShouldClose(m_glfwWindow)) {
    glfwPollEvents();
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(m_glfwWindow);
  }
}
