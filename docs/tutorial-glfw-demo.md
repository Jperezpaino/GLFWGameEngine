# Tutorial del proyecto GLFW Game Engine 0.2.0

Este documento explica la estructura actual del proyecto y el papel de cada parte del codigo. La version `0.2.0` abre una ventana con GLFW, crea un contexto OpenGL basico y usa una clase `KeyListener` para leer el teclado.

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
|       |-- Window.cpp
|       |-- KeyListener.h
|       `-- KeyListener.cpp
`-- third_party
    `-- glfw
```

### `main.cpp`

Contiene el punto de entrada real del programa. En C++, `main()` debe ser una funcion libre. La clase `Application` organiza el programa, pero no sustituye a `main()`.

### `Application`

Representa la aplicacion completa. Ahora mismo contiene una unica `Window`, pero mas adelante puede coordinar otros sistemas como renderizado, escenas, recursos o audio.

### `Window`

Representa la ventana GLFW, el contexto OpenGL, el bucle principal, la lectura de input y el render basico.

### `KeyListener`

Representa el estado del teclado. GLFW avisa cuando una tecla se pulsa o se suelta, y `KeyListener` guarda esa informacion para que otras partes del programa puedan consultarla.

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

`main()` se mantiene pequeno a proposito. Su trabajo es crear la aplicacion, ejecutarla, capturar errores graves y devolver un codigo de salida.

## 3. `Application`

```cpp
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

`Application` posee la ventana principal. Cuando `Application` se destruye, tambien se destruye `Window` automaticamente.

La copia y el movimiento estan desactivados con `= delete` porque `Window` gestiona recursos nativos de GLFW. Evitar copias accidentales hace que la propiedad de esos recursos sea mas clara.

## 4. `Window.h`

```cpp
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

    float m_red = 1.0f;
    float m_green = 1.0f;
    float m_blue = 1.0f;
    float m_alpha = 1.0f;
    bool m_fadeToBlack = false;

    void init();
    void loop();
    void processInput();
    void render();

};
```

El orden busca ser familiar si vienes de Java: primero la parte publica, despues los atributos privados y finalmente los metodos privados auxiliares.

### Miembros internos

- `m_width` y `m_height`: tamano inicial de la ventana.
- `m_title`: titulo de la ventana.
- `m_glfwWindow`: puntero que GLFW devuelve al crear la ventana nativa.
- `m_glfwInitialized`: indica si `glfwInit()` funciono, para saber si hay que llamar a `glfwTerminate()`.
- `m_red`, `m_green`, `m_blue` y `m_alpha`: color actual con el que se limpia la pantalla.
- `m_fadeToBlack`: indica si el fondo debe empezar a oscurecerse.

## 5. `Window.cpp`

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
    glfwSetErrorCallback(nullptr);
    m_glfwInitialized = false;
  }
}
```

El constructor por defecto basta porque los atributos ya tienen valores iniciales en `Window.h`.

El destructor libera recursos. Este patron se conoce como RAII: el objeto se encarga de limpiar aquello que posee cuando deja de existir.

### `init()`

```cpp
glfwSetKeyCallback(m_glfwWindow, KeyListener::keyCallback);
```

Despues de crear la ventana, `Window` registra el callback de teclado. Esta linea conecta GLFW con nuestra clase `KeyListener`.

El metodo tambien inicializa GLFW, configura la ventana, crea el contexto OpenGL, activa v-sync con `glfwSwapInterval(1)` y muestra la ventana.

### `loop()`

```cpp
void Window::loop() {
  while (!glfwWindowShouldClose(m_glfwWindow)) {
    glfwPollEvents();
    processInput();
    render();
    glfwSwapBuffers(m_glfwWindow);
  }
}
```

Este es el bucle principal. Cada vuelta procesa eventos, interpreta el input, renderiza un frame y finalmente presenta ese frame en pantalla.

### `processInput()`

```cpp
void Window::processInput() {
  if (KeyListener::isKeyPressed(GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
  }
  if (KeyListener::isKeyPressed(GLFW_KEY_SPACE)) {
    m_fadeToBlack = true;
  }
}
```

`processInput()` pregunta a `KeyListener` si una tecla esta pulsada.

- `ESC`: marca la ventana para cerrarse.
- `SPACE`: activa el fundido hacia negro.

### `render()`

```cpp
void Window::render() {
  glClearColor(m_red, m_green, m_blue, m_alpha);
  if (m_fadeToBlack) {
    m_red = std::max((m_red - 0.01f), 0.0f);
    m_green = std::max((m_green - 0.01f), 0.0f);
    m_blue = std::max((m_blue - 0.01f), 0.0f);
  }
  glClear(GL_COLOR_BUFFER_BIT);
}
```

`render()` define el color de limpieza y limpia la pantalla. Si `m_fadeToBlack` esta activo, reduce los componentes rojo, verde y azul poco a poco hasta llegar a `0.0f`.

## 6. `KeyListener`

```cpp
class KeyListener {

  public:

    static KeyListener& get();
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static bool isKeyPressed(int keyCode);

  private:

    static constexpr int KEY_COUNT = GLFW_KEY_LAST + 1;
    bool m_keyPressed[KEY_COUNT] = {};

    KeyListener() = default;

};
```

`KeyListener` usa un singleton local: `get()` devuelve siempre la misma instancia.

```cpp
KeyListener& KeyListener::get() {
  static KeyListener instance;
  return instance;
}
```

La instancia no se crea con `new`, asi que no hay que liberarla manualmente. C++ se encarga de crearla la primera vez que se llama a `get()` y destruirla al finalizar el programa.

`keyCallback()` tiene la firma que GLFW espera. Cuando GLFW detecta una tecla, llama a esa funcion. Si la accion es `GLFW_PRESS`, la tecla queda marcada como pulsada. Si la accion es `GLFW_RELEASE`, queda marcada como suelta.

`isKeyPressed()` es la parte comoda para el resto del programa: recibe una tecla, por ejemplo `GLFW_KEY_ESCAPE`, y devuelve `true` o `false`.

## 7. Flujo completo

```text
main()
  -> crea Application
  -> application.run()
      -> m_window.run()
          -> init()
              -> inicializa GLFW
              -> crea ventana
              -> registra KeyListener como callback de teclado
              -> activa contexto OpenGL
          -> loop()
              -> glfwPollEvents()
                  -> GLFW llama a KeyListener::keyCallback() si hay eventos de teclado
              -> processInput()
                  -> consulta ESC y SPACE
              -> render()
                  -> limpia pantalla con el color actual
              -> glfwSwapBuffers()
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

## 9. Version 0.2.0

La version `0.2.0` incluye:

- Proyecto Visual Studio 2019 en C++17.
- GLFW 3.4 vendorizado como dependencia local.
- Clase `Application` como capa principal.
- Clase `Window` para encapsular GLFW, OpenGL basico, input y render.
- Clase `KeyListener` para centralizar el estado del teclado.
- Cierre de ventana con `ESC`.
- Fundido hacia negro al pulsar `SPACE`.

## 10. Siguientes pasos razonables

- Crear una clase `Renderer` para separar OpenGL de `Window`.
- Crear una estructura `WindowConfig` para ancho, alto y titulo.
- Anadir callback de resize para llamar a `glViewport`.
- Crear un `MouseListener` siguiendo la misma idea que `KeyListener`.
- Dibujar un triangulo como primera geometria real.
