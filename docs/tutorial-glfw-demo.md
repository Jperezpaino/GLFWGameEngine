# Tutorial del proyecto GLFW Game Engine 0.1.0

Este documento explica la estructura actual del proyecto y el papel de cada parte del codigo. La version `0.1.0` abre una ventana con GLFW, crea un contexto OpenGL basico y limpia la pantalla en rojo.

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

Contiene el punto de entrada real del programa. En C++, `main()` debe ser una funcion libre. La clase `Application` organiza el programa, pero no sustituye a `main()`.

### `Application`

Representa la aplicacion completa. Ahora mismo contiene una unica `Window`, pero mas adelante puede coordinar otros sistemas como renderizado, escenas, recursos o audio.

### `Window`

Representa la ventana GLFW, el contexto OpenGL y el bucle basico de ejecucion.

## 2. `main.cpp`

```cpp
#include "Application.h"

#include <exception>
#include <iostream>

int main() {
  try {
    Application application;
    application.run();
  } catch (const std::exception& exception) {
    std::cerr << "Error: " << exception.what() << std::endl;
    return -1;
  }

  return 0;
}
```

`main()` se mantiene pequeno a proposito. Su trabajo es:

1. Crear la aplicacion.
2. Ejecutarla.
3. Capturar errores graves.
4. Devolver un codigo de salida.

Esto mantiene la entrada del programa clara y deja la logica real dentro de clases.

## 3. `Application.h`

```cpp
#pragma once

#include "Window.h"

class Application {

  public:

    Application();

    Application(const Application&) = delete;
    Application& operator = (const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator = (Application&&) = delete;

    void run();

  private:

    Window m_window;

};
```

### `#pragma once`

Evita que el mismo header se incluya mas de una vez. Es una forma sencilla y visual de evitar inclusiones duplicadas.

### Copia y movimiento desactivados

`Application` contiene una `Window`, y `Window` gestiona recursos nativos de GLFW. Por eso se desactivan copia y movimiento con `= delete`.

### `Window m_window`

`Application` posee la ventana principal. Cuando `Application` se destruye, tambien se destruye `Window` automaticamente.

## 4. `Application.cpp`

```cpp
#include "Application.h"

Application::Application() = default;

void Application::run() {
  m_window.run();
}
```

El constructor por defecto es suficiente porque `Window` se construye automaticamente como miembro de `Application`.

`run()` delega en `m_window.run()`. Aunque ahora parece pequeno, este metodo sera el punto natural para coordinar mas sistemas cuando el proyecto crezca.

## 5. `Window.h`

```cpp
#pragma once

#include <GLFW/glfw3.h>
#include <string>

class Window {

  public:

    Window();
    ~Window();

    Window(const Window&) = delete;
    Window& operator = (const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator = (Window&&) = delete;

    void run();

  private:

    int m_width = 1440;
    int m_height = 810;
    std::string m_title = "GLFW Game Engine";
    GLFWwindow* m_glfwWindow = nullptr;
    bool m_glfwInitialized = false;

    void init();
    void loop();

};
```

El orden busca ser familiar si vienes de Java:

1. Parte publica.
2. Parte privada con atributos.
3. Metodos privados auxiliares.

### Miembros internos

- `m_width` y `m_height`: tamano inicial de la ventana.
- `m_title`: titulo de la ventana.
- `m_glfwWindow`: puntero que GLFW devuelve al crear la ventana nativa.
- `m_glfwInitialized`: indica si `glfwInit()` funciono, para saber si hay que llamar a `glfwTerminate()`.

## 6. `Window.cpp`

### Constructor y destructor

```cpp
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
```

El constructor por defecto basta porque los atributos ya tienen valores iniciales en `Window.h`.

El destructor libera recursos. Este patron se conoce como RAII: el objeto se encarga de limpiar aquello que posee.

### `run()`

```cpp
void Window::run() {
  std::cout << "Hello GLFW " << glfwGetVersionString() << "!" << std::endl;

  init();
  loop();
}
```

`run()` muestra la version de GLFW, inicializa la ventana y entra en el bucle principal.

### `init()`

```cpp
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
```

Este metodo prepara GLFW y crea la ventana.

- `glfwSetErrorCallback`: registra como se muestran errores de GLFW.
- `glfwInit`: inicializa GLFW.
- `glfwWindowHint`: configura opciones de la ventana que se creara despues.
- `glfwCreateWindow`: crea la ventana y su contexto OpenGL.
- `glfwMakeContextCurrent`: hace activo el contexto OpenGL de la ventana.
- `glfwSwapInterval(1)`: activa v-sync.
- `glfwShowWindow`: muestra la ventana.

Los dos `nullptr` de `glfwCreateWindow` significan:

- No usar pantalla completa.
- No compartir recursos OpenGL con otra ventana.

### `loop()`

```cpp
void Window::loop() {
  while (!glfwWindowShouldClose(m_glfwWindow)) {
    glfwPollEvents();
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(m_glfwWindow);
  }
}
```

Este es el bucle principal de la ventana.

Cada iteracion hace cuatro cosas:

1. `glfwPollEvents()`: procesa eventos del sistema operativo.
2. `glClearColor(...)`: define el color de limpieza en rojo.
3. `glClear(GL_COLOR_BUFFER_BIT)`: limpia la pantalla con ese color.
4. `glfwSwapBuffers(...)`: presenta el frame en pantalla.

El bucle termina cuando GLFW detecta que la ventana debe cerrarse, por ejemplo al pulsar el boton de cerrar.

## 7. Flujo completo

```text
main()
  -> crea Application
  -> application.run()
      -> m_window.run()
          -> init()
              -> inicializa GLFW
              -> crea ventana
              -> activa contexto OpenGL
          -> loop()
              -> procesa eventos
              -> limpia pantalla en rojo
              -> presenta frame
  -> se destruye Application
      -> se destruye Window
          -> glfwDestroyWindow()
          -> glfwTerminate()
```

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

## 9. Version 0.1.0

La version `0.1.0` incluye:

- Proyecto Visual Studio 2019 en C++17.
- GLFW 3.4 vendorizado como dependencia local.
- Clase `Application` como capa principal.
- Clase `Window` para encapsular GLFW y OpenGL basico.
- Ventana `1440x810`, redimensionable y maximizada.
- Fondo rojo renderizado con OpenGL.

## 10. Siguientes pasos razonables

- Crear una clase `Renderer` para separar OpenGL de `Window`.
- Crear una estructura `WindowConfig` para ancho, alto y titulo.
- Anadir callback de resize para llamar a `glViewport`.
- Recuperar entrada de teclado si quieres cerrar con `Esc`.
- Dibujar un triangulo como primera geometria real.
