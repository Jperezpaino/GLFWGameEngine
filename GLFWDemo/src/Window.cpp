#include "Window.h"

#include "KeyListener.h"
#include "MouseListener.h"
#include "scene/LevelEditorScene.h"
#include "scene/LevelScene.h"
#include "util/TimeUtil.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

Window* Window::s_instance = nullptr;

Window::Window() {
  if (s_instance != nullptr) {
    throw std::runtime_error("Only one Window instance can exist.");
  }

  s_instance = this;
}

Window::~Window() {
  m_scene.reset();

  if (m_glfwWindow != nullptr) {
    glfwDestroyWindow(m_glfwWindow);
    m_glfwWindow = nullptr;
  }

  if (m_glfwInitialized) {
    glfwTerminate();
    glfwSetErrorCallback(nullptr);
    m_glfwInitialized = false;
  }

  if (s_instance == this) {
    s_instance = nullptr;
  }
}

Window& Window::get() {
  if (s_instance == nullptr) {
    throw std::runtime_error("Window has not been created yet.");
  }

  return *s_instance;
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

  changeScene(0);
}

void Window::loop() {
  TimeUtil::init();
  m_lastFrameTime = TimeUtil::getTime();

  while (!glfwWindowShouldClose(m_glfwWindow)) {
    updateDeltaTime();
    glfwPollEvents();
    processInput();

    if (m_scene != nullptr) {
      m_isUpdatingScene = true;
      m_scene->update(m_deltaTime);
      m_isUpdatingScene = false;
    }

    if (m_pendingScene >= 0) {
      loadScene(m_pendingScene);
      m_pendingScene = -1;
    }

    render();
    glfwSwapBuffers(m_glfwWindow);
    MouseListener::update();
  }
}

void Window::processInput() {
  if (KeyListener::isKeyPressed(GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
  }
}

void Window::render() {
  glClearColor(m_red, m_green, m_blue, m_alpha);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Window::updateDeltaTime() {
  const float currentTime = TimeUtil::getTime();
  m_deltaTime = currentTime - m_lastFrameTime;
  m_lastFrameTime = currentTime;
}

void Window::changeScene(
    int scene) {
  if (m_isUpdatingScene) {
    m_pendingScene = scene;
    return;
  }

  loadScene(scene);
}

void Window::loadScene(
    int scene) {
  switch (scene) {
    case 0:
      m_scene = std::make_unique<LevelEditorScene>();
      break;
    case 1:
      m_scene = std::make_unique<LevelScene>();
      break;
    default:
      assert(false && "Unknown scene");
      break;
  }
}

void Window::setClearColor(
    float red,
    float green,
    float blue,
    float alpha) {
  m_red = red;
  m_green = green;
  m_blue = blue;
  m_alpha = alpha;
}
