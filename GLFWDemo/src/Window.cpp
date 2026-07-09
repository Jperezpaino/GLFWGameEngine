#include "Window.h"
#include "KeyListener.h"
#include "MouseListener.h"
#include "util/TimeUtil.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cmath>

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
  TimeUtil::init();
  m_lastFrameTime = TimeUtil::getTime();

  while (!glfwWindowShouldClose(m_glfwWindow)) {
    updateDeltaTime();
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
              << " | Delta time: " << m_deltaTime
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
    const float pulse = 0.08f * std::sin(m_timePulse * 3.14159265f * 2.0f);

    m_red = std::clamp(normalizedX + m_colorOffset + pulse, 0.0f, 1.0f);
    m_green = std::clamp((1.0f - normalizedY) + m_colorOffset + pulse, 0.0f, 1.0f);
    m_blue = std::clamp(0.25f + (0.75f * normalizedY) + m_colorOffset + pulse, 0.0f, 1.0f);
  }

  if ((MouseListener::getXDisplacement() != 0.0f)
   || (MouseListener::getYDisplacement() != 0.0f)) {
    std::cout << "\rMouse X: " << MouseListener::getXPosition()
              << " | Mouse Y: " << MouseListener::getYPosition()
              << " | Color offset: " << m_colorOffset
              << " | Delta time: " << m_deltaTime
              << "      " << std::flush;
  }
}

void Window::render() {
  glClearColor(m_red, m_green, m_blue, m_alpha);
  if (m_fadeToBlack) {
    const float fadeSpeed = 0.75f;
    m_red = std::max((m_red - (fadeSpeed * m_deltaTime)), 0.0f);
    m_green = std::max((m_green - (fadeSpeed * m_deltaTime)), 0.0f);
    m_blue = std::max((m_blue - (fadeSpeed * m_deltaTime)), 0.0f);
  }
  glClear(GL_COLOR_BUFFER_BIT);
}

void Window::updateDeltaTime() {
  const float currentTime = TimeUtil::getTime();
  m_deltaTime = TimeUtil::deltaTime(m_lastFrameTime);
  m_lastFrameTime = currentTime;

  m_timePulse += m_deltaTime;
  if (m_timePulse > 1.0f) {
    m_timePulse -= 1.0f;
  }

  m_fpsTimer += m_deltaTime;
  ++m_frameCount;

  if (m_fpsTimer >= 1.0f) {
    std::cout << "\rFPS: " << m_frameCount
              << " | dt: " << m_deltaTime
              << "      " << std::flush;
    m_fpsTimer = 0.0f;
    m_frameCount = 0;
  }
}
