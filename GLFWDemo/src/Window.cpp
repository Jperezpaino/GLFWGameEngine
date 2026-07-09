#include "Window.h"
#include "KeyListener.h"
#include "MouseListener.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>

Window::Window() = default;

Window::~Window() {
  if (m_glfwWindow != nullptr) {
    glfwDestroyWindow(m_glfwWindow);
    m_glfwWindow = nullptr;
  }

  if (m_glfwInitialized) {
    glfwTerminate();
    glfwSetErrorCallback(nullptr);
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

  glfwSetKeyCallback(m_glfwWindow, KeyListener::keyCallback);
  glfwSetMouseButtonCallback(m_glfwWindow, MouseListener::mouseButtonCallback);
  glfwSetCursorPosCallback(m_glfwWindow, MouseListener::mousePositionCallback);
  glfwSetScrollCallback(m_glfwWindow, MouseListener::mouseScrollCallback);

  glfwMakeContextCurrent(m_glfwWindow);
  glfwSwapInterval(1);
  glfwShowWindow(m_glfwWindow);
}

void Window::loop() {
  while (!glfwWindowShouldClose(m_glfwWindow)) {
    glfwPollEvents();
    processInput();
    render();
    glfwSwapBuffers(m_glfwWindow);
    MouseListener::update();
  }
}

void Window::processInput() {
  if (KeyListener::isKeyPressed(GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
  }

  if (MouseListener::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
    m_fadeToBlack = true;
  }

  const float scrollY = MouseListener::getScrollYPosition();
  if (scrollY != 0.0f) {
    m_colorOffset = std::clamp(m_colorOffset + (scrollY * 0.05f), -0.5f, 0.5f);
    std::cout << "\rScroll Y: " << scrollY
              << " | Color offset: " << m_colorOffset
              << "      " << std::flush;
  }

  if (!m_fadeToBlack) {
    int windowWidth = 1;
    int windowHeight = 1;
    glfwGetWindowSize(m_glfwWindow, &windowWidth, &windowHeight);

    const float x = MouseListener::getXPosition();
    const float y = MouseListener::getYPosition();
    const float normalizedX = std::clamp(x / static_cast<float>(std::max(windowWidth, 1)), 0.0f, 1.0f);
    const float normalizedY = std::clamp(y / static_cast<float>(std::max(windowHeight, 1)), 0.0f, 1.0f);

    m_red = std::clamp(normalizedX + m_colorOffset, 0.0f, 1.0f);
    m_green = std::clamp((1.0f - normalizedY) + m_colorOffset, 0.0f, 1.0f);
    m_blue = std::clamp(0.25f + (0.75f * normalizedY) + m_colorOffset, 0.0f, 1.0f);
  }

  if ((MouseListener::getXDisplacement() != 0.0f)
   || (MouseListener::getYDisplacement() != 0.0f)) {
    std::cout << "\rMouse X: " << MouseListener::getXPosition()
              << " | Mouse Y: " << MouseListener::getYPosition()
              << " | Color offset: " << m_colorOffset
              << "      " << std::flush;
  }
}

void Window::render() {
  glClearColor(m_red, m_green, m_blue, m_alpha);
  if (m_fadeToBlack) {
    m_red = std::max((m_red - 0.01f), 0.0f);
    m_green = std::max((m_green - 0.01f), 0.0f);
    m_blue = std::max((m_blue - 0.01f), 0.0f);
  }
  glClear(GL_COLOR_BUFFER_BIT);
}
