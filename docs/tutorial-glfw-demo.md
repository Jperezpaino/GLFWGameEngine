# Tutorial del proyecto GLFW Game Engine 0.4.1

Este documento explica la estructura actual del proyecto. La version `0.4.1` da un paso importante: las escenas ya no solo actualizan logica, tambien dibujan contenido propio. La demo actual alterna entre una escena con un cuadrado y otra escena con un triangulo, usando `deltaTime` para realizar transiciones suaves de color de fondo.

## 1. Objetivo de la version 0.4.1

La rama `0.3.x` dejo preparado el sistema de escenas y el calculo de `deltaTime`. La version `0.4.1` completa esa base con una demo visual clara:

- `Scene` tiene tres fases: `init()`, `update(deltaTime)` y `render()`.
- `Window` mantiene el bucle principal y delega la logica y el dibujo en la escena activa.
- `LevelEditorScene` dibuja un cuadrado.
- `LevelScene` dibuja un triangulo.
- `ESPACIO` inicia una transicion temporizada hacia la otra escena.
- El fondo cambia suavemente usando interpolacion y `deltaTime`.
- `ESC` sigue cerrando la ventana desde el input global.

El objetivo no es todavia tener OpenGL moderno con shaders. El objetivo de esta version es entender bien el ciclo basico de escena: inicializar, actualizar, renderizar y cambiar de escena.

## 2. Estructura general

```text
GLFW - Demo
|-- GLFWDemo.sln
|-- GLFWDemo
|   |-- GLFWDemo.vcxproj
|   |-- GLFWDemo.vcxproj.filters
|   `-- src
|       |-- main.cpp
|       |-- Application.h
|       |-- Application.cpp
|       |-- Window.h
|       |-- Window.cpp
|       |-- KeyListener.h
|       |-- KeyListener.cpp
|       |-- MouseListener.h
|       |-- MouseListener.cpp
|       |-- scene
|       |   |-- Scene.h
|       |   |-- LevelEditorScene.h
|       |   |-- LevelEditorScene.cpp
|       |   |-- LevelScene.h
|       |   `-- LevelScene.cpp
|       `-- util
|           |-- TimeUtil.h
|           `-- TimeUtil.cpp
|-- docs
|   `-- tutorial-glfw-demo.md
`-- third_party
    `-- glfw
```

La carpeta `src` contiene el codigo propio del proyecto. La carpeta `third_party/glfw` contiene GLFW vendorizado y compilado junto al proyecto. La carpeta `third_party/glad` contiene GLAD, generado para OpenGL 3.3 Compatibility con loader activado.

## 3. Flujo completo de ejecucion

```text
main()
  -> crea Application
  -> application.run()
      -> m_window.run()
          -> init()
              -> inicializa GLFW
              -> crea la ventana
              -> crea el contexto OpenGL
              -> inicializa GLAD
              -> registra callbacks
              -> crea la primera escena
              -> llama a scene.init()
          -> loop()
              -> calcula deltaTime
              -> procesa eventos de GLFW
              -> procesa input global
              -> scene.update(deltaTime)
              -> aplica cambio de escena pendiente
              -> limpia pantalla
              -> scene.render()
              -> presenta el frame
  -> se destruye Application
      -> se destruye Window
          -> libera escena
          -> destruye ventana GLFW
          -> termina GLFW
```

La idea importante es que `Window` sostiene el motor, pero la logica visual concreta pertenece a las escenas.

## 4. `main.cpp`

`main.cpp` se mantiene pequeno:

```cpp
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

`main()` crea la aplicacion, la ejecuta y captura errores graves. Esto mantiene el punto de entrada limpio y deja el resto del programa dentro de clases propias.

## 5. `Application`

`Application` posee una unica `Window`:

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

Las operaciones de copia y movimiento se borran porque `Window` gestiona recursos nativos. Evitar copias accidentales es una practica sana en C++ cuando una clase posee recursos.

## 6. Responsabilidad de `Window`

`Window` encapsula:

- Inicializacion y finalizacion de GLFW.
- Creacion de ventana.
- Contexto OpenGL.
- Inicializacion de GLAD.
- Registro de callbacks.
- Bucle principal.
- Calculo de `deltaTime`.
- Input global.
- Escena activa.
- Cambio diferido de escena.
- Limpieza de pantalla.

`Window` no contiene ya reglas de demo como dibujar un cuadrado o un triangulo. Eso vive en las escenas.

## 7. Bucle principal

El bucle principal esta en `Window::loop()`:

```cpp
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
```

El orden es deliberado:

- Primero se calcula `deltaTime`.
- Despues GLFW procesa eventos.
- Luego se procesa input global.
- La escena actualiza su logica.
- Si una escena pidio cambiar a otra, el cambio se aplica al salir de `update()`.
- Se renderiza el frame.
- Se muestran los buffers.
- Se cierra el estado temporal del raton.

## 8. Render en `Window`

`Window::render()` limpia la pantalla y despues deja dibujar a la escena:

```cpp
void Window::render() {
  glClearColor(m_red, m_green, m_blue, m_alpha);
  glClear(GL_COLOR_BUFFER_BIT);

  if (m_scene != nullptr) {
    m_scene->render();
  }
}
```

Este orden es importante. Si la escena dibujara antes de `glClear`, el dibujo se borraria antes de llegar a pantalla.

## 9. GLAD

GLAD es el loader de OpenGL del proyecto. En Windows, incluir GLFW no basta para usar funciones modernas de OpenGL. GLAD carga esas funciones despues de que exista un contexto OpenGL activo.

En el proyecto se ha integrado asi:

- `third_party/glad/include` se anade a las rutas de include.
- `third_party/glad/src/glad.c` se compila junto con la aplicacion.
- `GLFW_INCLUDE_NONE` se define en los headers propios antes de incluir GLFW para evitar que GLFW incluya `gl.h`.
- Los ficheros que usan funciones OpenGL incluyen primero `glad/glad.h`.

La inicializacion ocurre en `Window::init()`, justo despues de `glfwMakeContextCurrent(...)`:

```cpp
if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
  throw std::runtime_error("Failed to initialize GLAD.");
}
```

Ese orden es obligatorio: GLAD necesita que el contexto OpenGL exista antes de cargar punteros de funciones.

## 10. Contrato de `Scene`

`Scene` define la interfaz comun:

```cpp
class Scene {

  public:

    Scene() = default;
    virtual ~Scene() = default;

    virtual void init() {}
    virtual void update(float deltaTime) = 0;
    virtual void render() {}

};
```

- `init()` se llama al cargar la escena. Es opcional.
- `update(float deltaTime)` es obligatorio y contiene logica.
- `render()` es opcional y contiene dibujo.

Esta separacion es muy habitual en juegos y motores pequenos: actualizar estado y dibujar estado son fases diferentes.

## 11. Carga y cambio de escena

`Window` guarda la escena activa con:

```cpp
std::unique_ptr<Scene> m_scene;
```

Cuando carga una escena, crea una clase concreta y llama a `init()`:

```cpp
switch (scene) {
  case 0:
    m_scene = std::make_unique<LevelEditorScene>();
    break;
  case 1:
    m_scene = std::make_unique<LevelScene>();
    break;
}

if (m_scene != nullptr) {
  m_scene->init();
}
```

Actualmente:

- `0`: `LevelEditorScene`, la escena del cuadrado.
- `1`: `LevelScene`, la escena del triangulo.

Si una escena pide cambiar durante `update()`, `Window` no destruye la escena inmediatamente. Guarda el cambio en `m_pendingScene` y lo aplica justo despues de terminar `update()`. Esto evita destruir un objeto mientras aun se esta ejecutando un metodo suyo.

## 12. `deltaTime`

`deltaTime` es el tiempo transcurrido desde el frame anterior.

```cpp
void Window::updateDeltaTime() {
  const float currentTime = TimeUtil::getTime();
  m_deltaTime = currentTime - m_lastFrameTime;
  m_lastFrameTime = currentTime;
}
```

Las escenas lo reciben en `update(float deltaTime)`. En `0.4.1` se usa para temporizar transiciones de fondo. Asi la transicion dura aproximadamente lo mismo independientemente de si la aplicacion corre a 30, 60 o mas FPS.

## 13. Transicion suave de fondo

Cada escena tiene:

```cpp
bool m_changingScene = false;
float m_timeToChangeScene = 1.0f;
```

Cuando se pulsa `ESPACIO`, la escena activa `m_changingScene`. A partir de ese momento resta `deltaTime` al contador.

El progreso se calcula asi:

```cpp
const float transitionDuration = 1.0f;
const float transitionProgress = std::clamp(
  1.0f - (m_timeToChangeScene / transitionDuration),
  0.0f,
  1.0f
);
```

El valor empieza cerca de `0.0f` y termina en `1.0f`. Luego se usa una interpolacion lineal:

```cpp
float lerp(float start, float end, float progress) {
  return start + ((end - start) * progress);
}
```

Esto evita el salto brusco. El color parte del color real de la escena y avanza poco a poco hacia un color oscuro.

## 14. `LevelEditorScene`: cuadrado

`LevelEditorScene` representa la escena inicial. En `init()` define un fondo rojo:

```cpp
Window::get().setClearColor(1.00f, 0.25f, 0.25f);
```

En `update()` espera `ESPACIO`, inicia la transicion y al terminar cambia a `LevelScene`:

```cpp
if (m_timeToChangeScene <= 0.0f) {
  Window::get().changeScene(1);
}
```

En `render()` dibuja un cuadrado con OpenGL inmediato:

```cpp
glBegin(GL_QUADS);

glColor3f(1.0f, 0.2f, 0.2f);
glVertex2f(-0.35f, -0.35f);

glColor3f(0.2f, 1.0f, 0.2f);
glVertex2f(0.35f, -0.35f);

glColor3f(0.2f, 0.4f, 1.0f);
glVertex2f(0.35f, 0.35f);

glColor3f(1.0f, 1.0f, 0.2f);
glVertex2f(-0.35f, 0.35f);

glEnd();
```

Al final restaura el color blanco para no dejar estado OpenGL arrastrado:

```cpp
glColor3f(1.0f, 1.0f, 1.0f);
```

## 15. `LevelScene`: triangulo

`LevelScene` representa la segunda escena. En `init()` define un fondo azul:

```cpp
Window::get().setClearColor(0.25f, 0.25f, 1.00f);
```

En `update()` espera `ESPACIO`, inicia la transicion y al terminar vuelve a `LevelEditorScene`:

```cpp
if (m_timeToChangeScene <= 0.0f) {
  Window::get().changeScene(0);
}
```

En `render()` dibuja un triangulo:

```cpp
glBegin(GL_TRIANGLES);

glColor3f(0.9f, 0.1f, 0.2f);
glVertex2f(0.0f, 0.45f);

glColor3f(0.1f, 0.6f, 1.0f);
glVertex2f(-0.45f, -0.35f);

glColor3f(0.2f, 0.9f, 0.3f);
glVertex2f(0.45f, -0.35f);

glEnd();
```

## 16. Sobre OpenGL inmediato

La demo usa `glBegin`, `glEnd`, `glVertex2f` y `glColor3f`. Es OpenGL antiguo, pero tiene una ventaja para esta fase: permite dibujar geometria simple antes de migrar a shaders, VAO, VBO y EBO. GLAD ya esta integrado, asi que el proyecto esta preparado para ese siguiente salto.

Esto no es la meta final para un motor moderno. Es una herramienta didactica para ver ya el ciclo completo:

```text
clear -> scene.render() -> swap buffers
```

El siguiente salto natural sera sustituir OpenGL inmediato por OpenGL moderno usando GLAD, shaders, VAO, VBO y EBO.

## 17. Input

`Window` mantiene input global:

```cpp
if (KeyListener::isKeyPressed(GLFW_KEY_ESCAPE)) {
  glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
}
```

Las escenas leen su propio input. En esta demo, ambas usan `ESPACIO` para cambiar de escena.

Una mejora futura sera distinguir entre tecla mantenida y tecla pulsada este frame. Ahora mismo `isKeyPressed()` devuelve si la tecla esta abajo, por lo que mantener `ESPACIO` puede disparar la accion al entrar en la siguiente escena.

## 18. Como compilar

Desde Visual Studio 2019:

1. Abre `GLFWDemo.sln`.
2. Selecciona `x64`.
3. Selecciona `Debug` para desarrollo o `Release` para una build optimizada.
4. Compila y ejecuta `GLFWDemo`.

Desde PowerShell:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe' 'GLFWDemo.sln' /p:Configuration=Debug /p:Platform=x64
```

Para Release:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe' 'GLFWDemo.sln' /p:Configuration=Release /p:Platform=x64
```

## 19. Resumen de cambios de 0.4.1

- GLAD se integra como dependencia externa vendorizada.
- `Scene` incorpora `render()` ademas de `init()` y `update(deltaTime)`.
- `Window::render()` limpia pantalla y despues llama a `m_scene->render()`.
- `LevelEditorScene` dibuja un cuadrado de colores.
- `LevelScene` dibuja un triangulo de colores.
- `ESPACIO` alterna entre ambas escenas.
- Ambas escenas usan `deltaTime` para transiciones temporizadas.
- El fondo interpola suavemente desde el color inicial de la escena hasta un color oscuro.
- Se mantiene el cambio de escena diferido para evitar destruir la escena durante su propio `update()`.
- Se conserva UTF-8 sin BOM y finales de linea `LF`.

## 20. Siguientes pasos razonables

- Sustituir ids numericos de escena por `enum class`.
- Crear un `SceneManager`.
- Crear una clase `Renderer`.
- Sustituir OpenGL inmediato por shaders, VAO, VBO y EBO usando GLAD.
- Anadir deteccion de tecla pulsada este frame.
- Anadir callback de resize y actualizar `glViewport`.
