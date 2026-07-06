#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <GLFW/glfw3.h>

class Window {
private:
    int width, height;
    std::string title;
    GLFWwindow* glfwWindow;

    static Window* window;

    // Constructor privado (singleton)
    Window();

public:
    // Eliminar constructor de copia y operador de asignación
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // Método estático para obtener la instancia única
    static Window* get();

    // Métodos públicos
    void run();
    void init();
    void loop();

    // Destructor
    ~Window();
};

#endif // WINDOW_H