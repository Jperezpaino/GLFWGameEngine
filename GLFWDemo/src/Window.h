#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

class Scene;

class Window {

  public:

    Window();
    ~Window();

    Window(const Window&) = delete;
    Window& operator = (const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator = (Window&&) = delete;

    static Window& get();

    void run();
    void changeScene(int scene);
    void setClearColor(float red, float green, float blue, float alpha = 1.0f);

  private:

    static Window* s_instance;

    int m_width = 1440;
    int m_height = 810;
    std::string m_title = "GLFW Game Engine";
    GLFWwindow* m_glfwWindow = nullptr;
    bool m_glfwInitialized = false;

    std::unique_ptr<Scene> m_scene;
    int m_pendingScene = -1;
    bool m_isUpdatingScene = false;
    float m_red = 0.1f;
    float m_green = 0.1f;
    float m_blue = 0.1f;
    float m_alpha = 1.0f;
    float m_lastFrameTime = 0.0f;
    float m_deltaTime = 0.0f;

    void init();
    void loop();
    void processInput();
    void render();
    void updateDeltaTime();
    void loadScene(int scene);

};
