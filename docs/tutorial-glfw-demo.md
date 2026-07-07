# Tutorial del proyecto GLFW Demo

Este documento explica la estructura del proyecto y el papel de cada parte del codigo. La aplicacion abre una ventana con GLFW, crea un contexto OpenGL basico, limpia la pantalla en blanco y mantiene un bucle principal hasta que el usuario cierra la ventana o pulsa `Esc`.

## 1. Estructura general

```text
GLFW - Demo
|-- GLFWDemo.sln
|-- GLFWDemo
|   |-- GLFWDemo.vcxproj
|   `-- src
|       |-- main.cpp
|       |-- Application.h
|       |-- Application.cpp
|       |-- Window.h
|       `-- Window.cpp
`-- third_party
    `-- glfw
```

### `main.cpp`

Contiene solo el punto de entrada real del programa. En C++, `main()` debe ser una funcion libre, no un metodo de una clase.

### `Application.h` y `Application.cpp`

Contienen la clase `Application`, que representa la aplicacion completa. Su responsabilidad actual es poseer una ventana y arrancar el programa.

### `Window.h` y `Window.cpp`

Contienen la clase `Window`, dedicada a GLFW, la ventana, el contexto OpenGL, el bucle principal, la entrada y el render basico.

### `third_party/glfw`

Contiene el subconjunto vendorizado de GLFW 3.4 necesario para compilar esta demo en Windows con Visual Studio.

## 2. Punto de entrada: `main.cpp`

```cpp
#include "Application.h"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        Application app;
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
```

`main()` se mantiene pequeno a proposito. Su trabajo es:

1. Crear la aplicacion.
2. Ejecutarla.
3. Capturar errores no recuperables.
4. Devolver un codigo de salida.

Esto deja el punto de entrada limpio y evita meter logica de motor o ventana directamente en `main.cpp`.

## 3. Clase `Application`

Archivo: `GLFWDemo/src/Application.h`

```cpp
#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.h"

class Application
{
public:
    Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void run();

private:
    Window m_window;
};

#endif // APPLICATION_H
```

Archivo: `GLFWDemo/src/Application.cpp`

```cpp
#include "Application.h"

Application::Application() = default;

void Application::run()
{
    m_window.run();
}
```

### Por que existe `Application`

`Application` es la capa que coordina el programa. Ahora solo contiene una ventana, pero mas adelante puede encargarse de objetos como:

- Renderer.
- Sistema de input.
- Escenas.
- Recursos.
- Audio.
- Estado principal del juego.

Asi `Window` no se convierte en una clase que sabe demasiado.

### Por que `run()` y no todo en el constructor

El constructor deja el objeto creado. `run()` arranca el comportamiento principal.

Esto hace que el flujo sea mas claro:

```cpp
Application app;
app.run();
```

Si en el futuro necesitas un `init()`, un bucle de aplicacion o una fase de cierre, `Application` es el lugar natural para ponerlo.

### Copia y movimiento desactivados

`Application` contiene una `Window`, y `Window` gestiona recursos nativos. Por eso se desactivan copia y movimiento: no queremos dos aplicaciones gestionando la misma ventana.

## 4. Clase `Window`

Archivo: `GLFWDemo/src/Window.h`

```cpp
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
```

`Window` se concentra en la ventana y en GLFW. Desde fuera solo se llama a `run()`.

Los metodos privados separan responsabilidades:

- `init()`: inicializa GLFW y crea la ventana.
- `loop()`: mantiene vivo el bucle principal.
- `processInput()`: lee entrada del usuario.
- `render()`: dibuja el frame actual.

## 5. Implementacion de `Window`

### Constructor y destructor

```cpp
Window::Window() = default;

Window::~Window()
{
    if (m_glfwWindow != nullptr)
    {
        glfwDestroyWindow(m_glfwWindow);
        m_glfwWindow = nullptr;
    }

    if (m_glfwInitialized)
    {
        glfwTerminate();
        m_glfwInitialized = false;
    }
}
```

Los miembros se inicializan directamente en `Window.h`, por eso el constructor puede ser `default`.

El destructor aplica RAII: si la ventana existe, se destruye; si GLFW fue inicializado, se termina.

### `run()`

```cpp
void Window::run()
{
    std::cout << "Hello GLFW " << glfwGetVersionString() << "!" << std::endl;
    init();
    loop();
}
```

`run()` coordina la vida de la ventana: inicializa y luego entra en el bucle principal.

### `init()`

```cpp
void Window::init()
{
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    });

    if (!glfwInit())
    {
        throw std::runtime_error("Unable to initialize GLFW.");
    }

    m_glfwInitialized = true;

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    m_glfwWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (m_glfwWindow == nullptr)
    {
        throw std::runtime_error("Failed to create the GLFW window.");
    }

    glfwMakeContextCurrent(m_glfwWindow);
    glfwSwapInterval(1);
    glfwShowWindow(m_glfwWindow);
}
```

Este metodo:

1. Registra el callback de errores de GLFW.
2. Inicializa GLFW.
3. Configura opciones de ventana.
4. Crea la ventana.
5. Activa el contexto OpenGL.
6. Activa v-sync.
7. Muestra la ventana.

### `loop()`

```cpp
void Window::loop()
{
    while (!glfwWindowShouldClose(m_glfwWindow))
    {
        glfwPollEvents();
        processInput();
        render();
        glfwSwapBuffers(m_glfwWindow);
    }
}
```

Este es el bucle principal de la ventana.

La secuencia es la base de muchas aplicaciones graficas:

```text
procesar eventos -> procesar input -> renderizar -> presentar frame
```

### `processInput()`

```cpp
void Window::processInput()
{
    if (glfwGetKey(m_glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
    }
}
```

Si el usuario pulsa `Esc`, la ventana se marca para cerrarse.

### `render()`

```cpp
void Window::render()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
```

Por ahora el render solo limpia la pantalla en blanco.

## 6. Flujo completo

```text
main()
  -> Application app
  -> app.run()
      -> m_window.run()
          -> Window::init()
              -> inicializa GLFW
              -> crea ventana
              -> activa contexto OpenGL
          -> Window::loop()
              -> glfwPollEvents()
              -> processInput()
              -> render()
              -> glfwSwapBuffers()
  -> termina main()
      -> se destruye Application
          -> se destruye Window
              -> se destruye ventana GLFW
              -> glfwTerminate()
```

## 7. Por que esta arquitectura es mas POO

- `main.cpp` solo arranca el programa.
- `Application` representa la aplicacion completa.
- `Window` representa la ventana y su contexto grafico.
- Cada clase tiene una responsabilidad clara.
- Los recursos se liberan automaticamente con destructores.
- El codigo queda preparado para anadir mas sistemas sin inflar `main.cpp` ni `Window`.

## 8. Como compilar

Desde Visual Studio 2019:

1. Abre `GLFWDemo.sln`.
2. Selecciona `x64`.
3. Selecciona `Debug` o `Release`.
4. Compila y ejecuta `GLFWDemo`.

Desde PowerShell:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe' 'GLFWDemo.sln' /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /m
```

Para Release:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe' 'GLFWDemo.sln' /t:Rebuild /p:Configuration=Release /p:Platform=x64 /m
```

## 9. Posibles siguientes mejoras

- Mover el bucle principal de `Window` a `Application` si quieres que la aplicacion coordine varios sistemas.
- Crear una clase `Renderer` para aislar OpenGL.
- Crear una estructura `WindowConfig` para ancho, alto y titulo.
- Anadir callback de resize para llamar a `glViewport`.
- Dibujar un triangulo OpenGL como siguiente paso grafico.
