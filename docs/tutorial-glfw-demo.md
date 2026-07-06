# Tutorial del proyecto GLFW Demo

Este documento explica la estructura del proyecto y el papel de cada parte del codigo. El objetivo actual de la aplicacion es abrir una ventana con GLFW, crear un contexto OpenGL basico y mantener un bucle principal hasta que el usuario cierre la ventana o pulse `Esc`.

## 1. Estructura general

```text
GLFW - Demo
|-- GLFWDemo.sln
|-- GLFWDemo
|   |-- GLFWDemo.vcxproj
|   `-- src
|       |-- main.cpp
|       |-- Window.h
|       `-- Window.cpp
`-- third_party
    `-- glfw
```

### `GLFWDemo.sln`

Es la solucion de Visual Studio 2019. Agrupa el proyecto C++ y permite abrirlo directamente desde Visual Studio.

### `GLFWDemo.vcxproj`

Es el archivo de proyecto de C++. Define:

- Configuraciones `Debug` y `Release`.
- Plataformas `x64` y `Win32`.
- Rutas de include para GLFW.
- Librerias de sistema necesarias, como `opengl32.lib`, `user32.lib`, `gdi32.lib` y `shell32.lib`.
- Archivos `.cpp` y `.c` que se compilan.

El proyecto compila GLFW desde sus fuentes en `third_party/glfw/src`, por eso no depende de un `glfw3.lib` externo.

### `third_party/glfw`

Contiene la libreria GLFW 3.4. GLFW se encarga de crear ventanas, gestionar entrada de teclado/raton y preparar un contexto OpenGL multiplataforma.

En este proyecto se usa especialmente para:

- Inicializar GLFW.
- Crear la ventana.
- Crear y activar el contexto OpenGL.
- Procesar eventos del sistema.
- Consultar si se ha pulsado una tecla.

## 2. Punto de entrada: `main.cpp`

Archivo: `GLFWDemo/src/main.cpp`

```cpp
#include "Window.h"
#include <iostream>

int main() {
    try {
        Window* window = Window::get();
        window->run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
```

### `#include "Window.h"`

Incluye la declaracion de la clase `Window`. Gracias a esto, `main.cpp` conoce que existe una clase llamada `Window` y que tiene metodos como `get()` y `run()`.

### `#include <iostream>`

Permite escribir mensajes en la consola usando `std::cout` y `std::cerr`.

### `int main()`

Es el punto de entrada de la aplicacion. Cuando se ejecuta el programa, la ejecucion empieza aqui.

### Bloque `try`

```cpp
try {
    Window* window = Window::get();
    window->run();
}
```

Dentro del bloque `try` se crea u obtiene la ventana principal y se ejecuta la aplicacion.

La llamada importante es:

```cpp
Window* window = Window::get();
```

`Window::get()` devuelve la instancia unica de la clase `Window`. Este proyecto usa una forma sencilla del patron Singleton: solo deberia existir una ventana principal.

Despues se llama a:

```cpp
window->run();
```

Ese metodo arranca el flujo principal de la ventana: inicializacion de GLFW, creacion de ventana y bucle de renderizado.

### Bloque `catch`

```cpp
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
}
```

Si ocurre un error durante la inicializacion, por ejemplo si GLFW no puede arrancar o no se puede crear la ventana, se lanza una excepcion. Este bloque la captura, muestra el mensaje por consola y termina el programa con codigo `-1`.

### `return 0`

Indica que el programa ha terminado correctamente.

## 3. Declaracion de la ventana: `Window.h`

Archivo: `GLFWDemo/src/Window.h`

```cpp
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

    Window();

public:
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    static Window* get();

    void run();
    void init();
    void loop();

    ~Window();
};

#endif // WINDOW_H
```

Este archivo declara la clase `Window`, pero no implementa su logica. La implementacion esta en `Window.cpp`.

### Guardas de inclusion

```cpp
#ifndef WINDOW_H
#define WINDOW_H
...
#endif
```

Evitan que el archivo se incluya mas de una vez en la misma unidad de compilacion. Sin esto, el compilador podria encontrar declaraciones duplicadas.

### Includes

```cpp
#include <string>
#include <GLFW/glfw3.h>
```

`<string>` permite usar `std::string` para guardar el titulo de la ventana.

`<GLFW/glfw3.h>` incluye las declaraciones de GLFW, como `GLFWwindow`, `glfwInit`, `glfwCreateWindow`, etc.

### Miembros privados

```cpp
int width, height;
std::string title;
GLFWwindow* glfwWindow;
```

Estos datos representan el estado interno de la ventana:

- `width`: ancho inicial de la ventana.
- `height`: alto inicial de la ventana.
- `title`: texto que aparece en la barra de titulo.
- `glfwWindow`: puntero interno de GLFW a la ventana real creada por la libreria.

### Miembro estatico `window`

```cpp
static Window* window;
```

Este puntero guarda la instancia unica de `Window`. Al ser `static`, pertenece a la clase y no a un objeto concreto.

### Constructor privado

```cpp
Window();
```

El constructor es privado para impedir que otras partes del programa creen ventanas directamente con `new Window()` o `Window w;`.

La ventana se obtiene mediante:

```cpp
Window::get();
```

### Constructor de copia eliminado

```cpp
Window(const Window&) = delete;
Window& operator=(const Window&) = delete;
```

Estas dos lineas impiden copiar la ventana. Tiene sentido porque una ventana GLFW representa un recurso del sistema operativo; copiarla podria provocar errores o dobles liberaciones.

### Metodos publicos

```cpp
static Window* get();
void run();
void init();
void loop();
~Window();
```

- `get()`: devuelve la instancia unica de `Window`.
- `run()`: arranca la ejecucion principal.
- `init()`: inicializa GLFW y crea la ventana.
- `loop()`: ejecuta el bucle principal.
- `~Window()`: destruye la ventana y termina GLFW.

## 4. Implementacion de la ventana: `Window.cpp`

Archivo: `GLFWDemo/src/Window.cpp`

Este archivo contiene el codigo real de los metodos declarados en `Window.h`.

### Includes

```cpp
#include "Window.h"

#include <iostream>
#include <stdexcept>
```

`Window.h` trae la declaracion de la clase.

`<iostream>` se usa para escribir mensajes en consola.

`<stdexcept>` permite lanzar excepciones como `std::runtime_error`.

### Instancia estatica

```cpp
Window* Window::window = nullptr;
```

Aqui se define el puntero estatico declarado en `Window.h`.

Al principio vale `nullptr`, lo que significa que todavia no existe ninguna instancia de `Window`.

### Constructor

```cpp
Window::Window()
    : width(1920),
      height(1080),
      title("Mario"),
      glfwWindow(nullptr)
{
}
```

Inicializa los valores internos de la ventana:

- Ancho: `1920`.
- Alto: `1080`.
- Titulo: `Mario`.
- Puntero GLFW: `nullptr` porque la ventana aun no se ha creado.

La sintaxis despues de los dos puntos es una lista de inicializacion. Es la forma recomendada en C++ para inicializar miembros de una clase.

### Metodo `get()`

```cpp
Window* Window::get()
{
    if (window == nullptr)
    {
        window = new Window();
    }

    return window;
}
```

Este metodo implementa el Singleton.

La primera vez que se llama, `window` vale `nullptr`, asi que crea una instancia nueva:

```cpp
window = new Window();
```

En llamadas posteriores, ya existe una ventana, asi que simplemente devuelve el mismo puntero.

Nota: para una demo sencilla esta implementacion es suficiente. En un proyecto mas grande se podria usar `std::unique_ptr` o una instancia estatica local para gestionar mejor la memoria.

### Metodo `run()`

```cpp
void Window::run()
{
    std::cout << "Hello GLFW " << glfwGetVersionString() << "!" << std::endl;
    init();
    loop();
}
```

Este metodo coordina el flujo principal:

1. Muestra por consola la version de GLFW.
2. Llama a `init()` para preparar la ventana.
3. Llama a `loop()` para entrar en el bucle principal.

La llamada `glfwGetVersionString()` devuelve una cadena con informacion de la version de GLFW usada.

### Metodo `init()`

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
```

Este es uno de los metodos mas importantes.

#### Callback de errores

```cpp
glfwSetErrorCallback([](int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
});
```

Registra una funcion que GLFW llamara cuando ocurra un error interno.

La sintaxis `[](...) { ... }` es una lambda de C++. En este caso se usa para imprimir el codigo de error y su descripcion.

#### Inicializacion de GLFW

```cpp
if (!glfwInit())
{
    throw std::runtime_error("Unable to initialize GLFW.");
}
```

`glfwInit()` inicializa la libreria GLFW. Debe llamarse antes de usar casi cualquier otra funcion de GLFW.

Si devuelve falso, se lanza una excepcion.

#### Configuracion de hints

```cpp
glfwDefaultWindowHints();
glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
```

Los window hints son opciones que GLFW usa al crear la ventana.

- `glfwDefaultWindowHints()`: restaura los valores por defecto.
- `GLFW_VISIBLE, GLFW_FALSE`: crea la ventana oculta inicialmente.
- `GLFW_RESIZABLE, GLFW_TRUE`: permite redimensionar la ventana.
- `GLFW_MAXIMIZED, GLFW_TRUE`: solicita que la ventana se abra maximizada.

Crear la ventana oculta y mostrarla al final evita que el usuario vea estados intermedios mientras se configura.

#### Creacion de la ventana

```cpp
glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
```

Crea una ventana GLFW y un contexto OpenGL asociado.

Parametros:

- `width`: ancho inicial.
- `height`: alto inicial.
- `title.c_str()`: titulo convertido a cadena estilo C.
- Primer `nullptr`: indica que no se crea en modo pantalla completa.
- Segundo `nullptr`: indica que no comparte recursos OpenGL con otra ventana.

#### Comprobacion de error

```cpp
if (glfwWindow == nullptr)
{
    glfwTerminate();
    throw std::runtime_error("Failed to create the GLFW window.");
}
```

Si la ventana no se pudo crear, se libera GLFW con `glfwTerminate()` y se lanza una excepcion.

#### Activar el contexto OpenGL

```cpp
glfwMakeContextCurrent(glfwWindow);
```

Hace que el contexto OpenGL de esta ventana sea el contexto activo del hilo actual.

Esto es necesario antes de llamar a funciones OpenGL como `glClearColor` o `glClear`.

#### Activar v-sync

```cpp
glfwSwapInterval(1);
```

Activa la sincronizacion vertical. Esto hace que el intercambio de buffers se sincronice con el refresco del monitor, reduciendo tearing y evitando que el bucle renderice miles de frames por segundo sin necesidad.

#### Mostrar la ventana

```cpp
glfwShowWindow(glfwWindow);
```

Muestra la ventana despues de haberla configurado.

### Metodo `loop()`

```cpp
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
```

Este metodo contiene el bucle principal de la aplicacion.

Mientras la ventana no deba cerrarse, el programa repite estos pasos:

1. Procesa eventos.
2. Comprueba entrada del teclado.
3. Limpia la pantalla.
4. Presenta el frame en pantalla.

#### Condicion del bucle

```cpp
while (!glfwWindowShouldClose(glfwWindow))
```

`glfwWindowShouldClose` pregunta si la ventana deberia cerrarse.

Esto puede ocurrir si el usuario pulsa el boton de cerrar de la ventana o si el programa llama a `glfwSetWindowShouldClose`.

#### Procesar eventos

```cpp
glfwPollEvents();
```

Procesa eventos pendientes del sistema operativo, como teclado, raton, movimiento de ventana o cierre.

Si no se llama a esta funcion, la ventana podria dejar de responder.

#### Cerrar con Escape

```cpp
if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
{
    glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
}
```

Comprueba si la tecla `Esc` esta pulsada. Si lo esta, marca la ventana para cerrarse.

Esto no destruye la ventana inmediatamente; solo hace que la condicion del `while` falle en la siguiente iteracion.

#### Limpiar pantalla

```cpp
glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);
```

`glClearColor` define el color de limpieza. Los valores son rojo, verde, azul y alfa, entre `0.0f` y `1.0f`.

En este caso:

```text
R = 1.0
G = 1.0
B = 1.0
A = 1.0
```

Eso significa blanco opaco.

`glClear(GL_COLOR_BUFFER_BIT)` limpia el buffer de color usando ese color.

#### Intercambiar buffers

```cpp
glfwSwapBuffers(glfwWindow);
```

GLFW usa doble buffer por defecto. Mientras se dibuja un frame, se dibuja en un buffer trasero. Cuando el frame esta listo, `glfwSwapBuffers` intercambia el buffer trasero con el frontal, mostrando el resultado en pantalla.

Esto evita parpadeos y frames incompletos.

### Destructor

```cpp
Window::~Window()
{
    if (glfwWindow != nullptr)
    {
        glfwDestroyWindow(glfwWindow);
        glfwWindow = nullptr;
    }

    glfwTerminate();
}
```

El destructor libera los recursos usados por la ventana.

Primero comprueba si existe una ventana GLFW:

```cpp
if (glfwWindow != nullptr)
```

Si existe, la destruye:

```cpp
glfwDestroyWindow(glfwWindow);
```

Despues pone el puntero a `nullptr` para evitar que apunte a memoria ya liberada.

Finalmente llama a:

```cpp
glfwTerminate();
```

Esto libera los recursos globales de GLFW.

## 5. Flujo completo de ejecucion

El flujo de la aplicacion es este:

```text
main()
  -> Window::get()
      -> crea la instancia unica de Window si aun no existe
  -> Window::run()
      -> imprime la version de GLFW
      -> init()
          -> registra callback de errores
          -> inicializa GLFW
          -> configura hints de ventana
          -> crea la ventana
          -> activa el contexto OpenGL
          -> activa v-sync
          -> muestra la ventana
      -> loop()
          -> procesa eventos
          -> comprueba tecla Escape
          -> limpia la pantalla
          -> intercambia buffers
  -> al cerrar, termina el programa
```

## 6. Detalles importantes de C++ usados en el proyecto

### Clases

La clase `Window` agrupa datos y funciones relacionados con la ventana. Esto evita tener toda la logica en `main.cpp`.

### Encapsulacion

Los datos internos estan en `private`, asi que no se modifican directamente desde fuera.

```cpp
private:
    int width, height;
    std::string title;
    GLFWwindow* glfwWindow;
```

Esto permite controlar mejor el estado de la ventana.

### Singleton

El proyecto usa un Singleton simple:

```cpp
static Window* window;
static Window* get();
```

La idea es tener una unica instancia de `Window`.

Para una demo es suficiente. En proyectos mas grandes conviene valorar alternativas, porque los Singletons pueden complicar pruebas, orden de destruccion y gestion de dependencias.

### Excepciones

Se usan excepciones para errores de inicializacion:

```cpp
throw std::runtime_error("Unable to initialize GLFW.");
```

`main.cpp` captura esas excepciones y muestra el error por consola.

### Punteros

`GLFWwindow*` es un puntero a una estructura interna de GLFW.

No se crea con `new`; lo crea GLFW mediante:

```cpp
glfwCreateWindow(...)
```

Y se destruye con:

```cpp
glfwDestroyWindow(glfwWindow);
```

## 7. Detalles importantes de GLFW y OpenGL

### GLFW no dibuja graficos complejos

GLFW crea la ventana y gestiona eventos, pero no es un motor grafico. Para dibujar se usa OpenGL.

En este proyecto solo se usa OpenGL para limpiar la pantalla en blanco.

### Contexto OpenGL

Antes de llamar a funciones OpenGL, hay que activar un contexto:

```cpp
glfwMakeContextCurrent(glfwWindow);
```

Sin esto, las llamadas OpenGL no tendrian un destino valido.

### Bucle de render

Toda aplicacion grafica interactiva suele tener un bucle principal:

```text
leer eventos -> actualizar estado -> dibujar -> presentar frame
```

Este proyecto ya tiene esa estructura basica.

## 8. Problema corregido durante la separacion de logica

Al separar la logica en `Window.cpp` y `Window.h`, aparecieron dos detalles importantes.

### `Window.cpp` debe compilarse

No basta con crear el archivo. Tambien debe estar incluido en el `.vcxproj` para que Visual Studio lo compile.

Si `Window.cpp` no se compila, aparecen errores de linker como:

```text
simbolo externo Window::get sin resolver
simbolo externo Window::run sin resolver
```

Eso significa que `main.cpp` conoce que esos metodos existen, pero el enlazador no encuentra su implementacion compilada.

### Conflicto entre `Window.cpp` y `window.c`

GLFW tiene un archivo llamado:

```text
third_party/glfw/src/window.c
```

Tu proyecto tiene:

```text
GLFWDemo/src/Window.cpp
```

En Windows, ambos pueden generar objetos con nombre muy parecido o equivalente para el enlazador:

```text
window.obj
Window.obj
```

Como el sistema de archivos y algunas herramientas de Windows no distinguen bien mayusculas y minusculas, esto puede provocar conflictos.

La solucion aplicada fue hacer que tu `Window.cpp` genere:

```text
AppWindow.obj
```

Asi no choca con el `window.obj` de GLFW.

## 9. Como compilar

Desde Visual Studio 2019:

1. Abrir `GLFWDemo.sln`.
2. Seleccionar `x64`.
3. Seleccionar `Debug` o `Release`.
4. Compilar la solucion.
5. Ejecutar `GLFWDemo`.

Desde terminal, usando MSBuild de Visual Studio 2019:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe' 'GLFWDemo.sln' /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /m
```

Para Release:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe' 'GLFWDemo.sln' /t:Rebuild /p:Configuration=Release /p:Platform=x64 /m
```

## 10. Posibles siguientes mejoras

### Usar `std::unique_ptr`

Ahora el Singleton usa `new`:

```cpp
window = new Window();
```

Una mejora seria usar una instancia estatica local o `std::unique_ptr` para gestionar mejor la vida del objeto.

### Separar inicializacion y render

Ahora `loop()` limpia la pantalla directamente. Mas adelante podria separarse en metodos como:

```cpp
processInput();
update();
render();
```

Eso dejaria la arquitectura preparada para juegos o demos mas complejas.

### Gestionar resize

Actualmente no hay callback de resize. Si la ventana cambia de tamano, convendria actualizar el viewport de OpenGL con:

```cpp
glViewport(0, 0, width, height);
```

### Cambiar color o dibujar geometria

El siguiente paso natural seria dibujar un triangulo con OpenGL, o cambiar el color de fondo en funcion del tiempo.

## 11. Resumen rapido

- `main.cpp` arranca la aplicacion.
- `Window.h` declara la clase `Window`.
- `Window.cpp` implementa la logica de la ventana.
- GLFW crea la ventana y gestiona eventos.
- OpenGL limpia la pantalla.
- El bucle principal se mantiene vivo hasta cerrar la ventana o pulsar `Esc`.
- El proyecto compila GLFW desde `third_party/glfw`.
