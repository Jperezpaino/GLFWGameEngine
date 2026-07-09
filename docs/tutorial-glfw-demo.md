# Tutorial del proyecto GLFW Game Engine 0.3.1

Este documento explica con detalle la estructura actual del proyecto. La version `0.3.1` marca un cambio importante respecto a las demos anteriores: la logica deja de vivir dentro de `Window` y empieza a organizarse en escenas.

La idea principal es que `Window` actue como base del motor: crea la ventana, registra callbacks, calcula `deltaTime`, ejecuta el bucle principal y llama a la escena activa. La logica concreta, como una pantalla de editor o una pantalla de juego, vive en clases que heredan de `Scene`.

## 1. Objetivo de la version 0.3.1

Hasta ahora el proyecto habia servido para probar piezas individuales de GLFW: teclado, raton, scroll, posicion del cursor y tiempo. Eso fue util para aprender la API, pero no era una buena estructura a largo plazo, porque `Window` acababa teniendo demasiadas responsabilidades.

En `0.3.1` el objetivo es limpiar esa fase de pruebas y dejar una base mas parecida a un motor pequeno:

- `Window` controla el ciclo de vida de GLFW.
- `Window` calcula `deltaTime` una vez por frame.
- `Scene` define donde vive la logica del programa.
- `LevelEditorScene` y `LevelScene` son las primeras escenas concretas.
- El cambio de escena es seguro incluso si se solicita dentro de `update()`.
- La documentacion ya describe el sistema actual, no las demos antiguas.

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

La carpeta `src` contiene el codigo propio del proyecto. La carpeta `third_party/glfw` contiene GLFW vendorizado, es decir, copiado dentro del repositorio para que Visual Studio pueda compilarlo junto al proyecto sin depender de una libreria externa generada a mano.

## 3. Flujo completo de ejecucion

El programa arranca en `main.cpp`, crea una `Application`, la aplicacion ejecuta su `Window`, y `Window` se queda dentro del bucle principal hasta que la ventana se cierra.

```text
main()
  -> crea Application
  -> application.run()
      -> m_window.run()
          -> init()
              -> inicializa GLFW
              -> crea la ventana
              -> registra callbacks
              -> crea la primera escena
          -> loop()
              -> calcula deltaTime
              -> procesa eventos de GLFW
              -> procesa input global
              -> actualiza la escena activa
              -> aplica cambio de escena pendiente
              -> renderiza
              -> presenta el frame
  -> se destruye Application
      -> se destruye Window
          -> libera escena
          -> destruye ventana GLFW
          -> termina GLFW
```

Este esquema es importante porque ayuda a separar responsabilidades. `main()` no sabe nada de GLFW, `Application` no sabe como se renderiza, y las escenas no tienen que crear la ventana.

## 4. `main.cpp`

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

`main()` se mantiene pequeno a proposito. En C++, `main()` debe ser una funcion libre, no un metodo de clase como podria imaginar alguien que viene de Java.

Sus responsabilidades son solo tres:

- Crear la aplicacion.
- Ejecutarla.
- Capturar errores graves y devolver un codigo de salida.

El bloque `try/catch` permite mostrar por consola cualquier `std::exception` lanzada durante la inicializacion o ejecucion. Por ejemplo, si GLFW no pudiera inicializarse, `Window` lanza una excepcion y `main()` la muestra.

## 5. `Application`

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

`Application` representa la aplicacion completa. Ahora mismo solo contiene una `Window`, pero ya sirve como punto de ampliacion natural.

Mas adelante podria coordinar:

- Configuracion global.
- Sistema de recursos.
- Audio.
- Renderizado.
- Gestor de escenas.
- Sistemas de debug.

Las operaciones de copia y movimiento estan borradas con `= delete`. Esto evita copiar accidentalmente una `Application`, lo cual copiaria tambien una `Window` que gestiona recursos nativos de GLFW. Es una decision defensiva y muy comun en C++ moderno cuando una clase posee recursos.

## 6. `Window`: papel general

`Window` es la clase mas importante de esta fase. Su trabajo no es contener la logica del juego, sino sostener el entorno donde esa logica se ejecuta.

Responsabilidades actuales de `Window`:

- Inicializar y terminar GLFW.
- Crear y destruir la ventana nativa.
- Crear el contexto OpenGL.
- Registrar callbacks de teclado y raton.
- Ejecutar el bucle principal.
- Calcular `deltaTime`.
- Gestionar input global, como `ESC` para cerrar.
- Poseer la escena activa.
- Cambiar de escena de forma segura.
- Limpiar la pantalla con un color base.

Lo que ya no debe hacer `Window`:

- Contener demos especificas de raton.
- Contener reglas de juego.
- Contener logica de editor.
- Contener animaciones concretas que pertenezcan a una escena.

Esta separacion es el cambio mas importante de `0.3.1`.

## 7. Instancia actual de `Window`

`Window` contiene un puntero estatico privado:

```cpp
static Window* s_instance;
```

Y expone:

```cpp
static Window& get();
```

Esto permite que una escena haga cosas como:

```cpp
Window::get().setClearColor(0.50f, 0.50f, 0.50f);
Window::get().changeScene(1);
```

Para este proyecto, esta solucion es sencilla y facil de seguir. Es parecida a tener una referencia global controlada a la ventana principal.

Hay dos protecciones importantes:

- El constructor lanza error si ya existe otra `Window`.
- `Window::get()` lanza error si se llama antes de crear la ventana.

No es necesariamente la arquitectura final de un motor grande, pero es perfectamente razonable para una demo educativa. Cuando el proyecto crezca, esta idea puede evolucionar a un `SceneManager`, un `EngineContext` o dependencias pasadas por constructor.

## 8. Ciclo de vida de `Window`

El constructor registra la instancia:

```cpp
Window::Window() {
  if (s_instance != nullptr) {
    throw std::runtime_error("Only one Window instance can exist.");
  }

  s_instance = this;
}
```

El destructor libera en orden:

```cpp
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
```

El orden importa:

- Primero se destruye la escena activa.
- Luego se destruye la ventana nativa de GLFW.
- Despues se termina GLFW.
- Finalmente se limpia la instancia estatica.

Este patron encaja con RAII: los recursos se adquieren al crear/inicializar objetos y se liberan automaticamente al destruirlos.

## 9. Inicializacion de GLFW

`Window::init()` configura y crea la ventana.

Puntos clave:

```cpp
glfwSetErrorCallback(...);
```

Registra una funcion para imprimir errores internos de GLFW.

```cpp
if (!glfwInit()) {
  throw std::runtime_error("Unable to initialize GLFW.");
}
```

Inicializa GLFW. Si falla, se lanza una excepcion.

```cpp
glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
```

Configura pistas de la ventana antes de crearla. En este caso se crea oculta, redimensionable y maximizada.

```cpp
m_glfwWindow = glfwCreateWindow(...);
```

Crea la ventana nativa.

```cpp
glfwMakeContextCurrent(m_glfwWindow);
glfwSwapInterval(1);
glfwShowWindow(m_glfwWindow);
```

Activa el contexto OpenGL, habilita v-sync y muestra la ventana.

Al final se crea la primera escena:

```cpp
changeScene(0);
```

Actualmente el id `0` significa `LevelEditorScene`.

## 10. Callbacks de input

`Window::init()` conecta GLFW con nuestras clases listener:

```cpp
glfwSetKeyCallback(m_glfwWindow, KeyListener::keyCallback);
glfwSetMouseButtonCallback(m_glfwWindow, MouseListener::mouseButtonCallback);
glfwSetCursorPosCallback(m_glfwWindow, MouseListener::mousePositionCallback);
glfwSetScrollCallback(m_glfwWindow, MouseListener::mouseScrollCallback);
```

GLFW trabaja por callbacks: cuando ocurre un evento, llama a una funcion. En vez de repartir logica por todo el proyecto, los callbacks actualizan `KeyListener` y `MouseListener`.

Luego cualquier clase puede preguntar:

```cpp
KeyListener::isKeyPressed(GLFW_KEY_SPACE)
MouseListener::getXPosition()
MouseListener::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)
```

Ahora mismo `LevelEditorScene` usa teclado para detectar `ESPACIO`, y `Window` usa teclado para cerrar con `ESC`. El raton queda preparado para futuras escenas.

## 11. `TimeUtil`

`TimeUtil` centraliza el reloj base del motor.

```cpp
class TimeUtil {

  public:

    static void init();
    static float getTime();

  private:

    static std::chrono::steady_clock::time_point m_timeStarted;

};
```

`std::chrono::steady_clock` es apropiado para medir duraciones porque es monotono. Eso significa que no depende de cambios en la hora del sistema.

`init()` guarda el instante inicial:

```cpp
void TimeUtil::init() {
  m_timeStarted = std::chrono::steady_clock::now();
}
```

`getTime()` devuelve los segundos transcurridos desde ese instante:

```cpp
float TimeUtil::getTime() {
  const auto now = std::chrono::steady_clock::now();
  const auto elapsed = now - m_timeStarted;
  return std::chrono::duration<float>(elapsed).count();
}
```

Se han eliminado metodos que no se usaban (`getTimeMillis`, `getTimeNanos`, `sleep`, `deltaTime`) para mantener la clase minima y clara.

## 12. `deltaTime`

`deltaTime` representa cuanto tiempo ha pasado desde el frame anterior. Es fundamental para que animaciones, movimientos y temporizadores no dependan de los FPS.

En `Window::loop()` se inicializa el reloj antes de entrar al bucle:

```cpp
TimeUtil::init();
m_lastFrameTime = TimeUtil::getTime();
```

Cada frame se calcula asi:

```cpp
void Window::updateDeltaTime() {
  const float currentTime = TimeUtil::getTime();
  m_deltaTime = currentTime - m_lastFrameTime;
  m_lastFrameTime = currentTime;
}
```

Ejemplo conceptual:

- Si un frame tarda `0.016`, eso equivale aproximadamente a 60 FPS.
- Si un frame tarda `0.033`, eso equivale aproximadamente a 30 FPS.
- Si una transicion resta `deltaTime`, durara lo mismo en ambos casos.

Por eso la escena recibe:

```cpp
m_scene->update(m_deltaTime);
```

Y no simplemente `m_scene->update()`.

## 13. Bucle principal de `Window`

El bucle principal completo tiene esta estructura:

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

Orden explicado paso a paso:

- `updateDeltaTime()`: calcula el tiempo entre frames.
- `glfwPollEvents()`: permite que GLFW procese eventos pendientes.
- `processInput()`: procesa input global del motor.
- `m_scene->update(m_deltaTime)`: ejecuta la logica de la escena activa.
- Bloque de `m_pendingScene`: aplica un cambio de escena pedido durante `update()`.
- `render()`: limpia la pantalla.
- `glfwSwapBuffers()`: presenta el frame calculado.
- `MouseListener::update()`: prepara el estado del raton para el siguiente frame.

Este bucle es ya una base muy reconocible para un juego o motor pequeno.

## 14. Input global

`Window::processInput()` contiene solo input que pertenece al motor o a la ventana, no a una escena concreta.

```cpp
void Window::processInput() {
  if (KeyListener::isKeyPressed(GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
  }
}
```

`ESC` es global porque cerrar la ventana no depende de estar en editor o en juego.

En cambio, `ESPACIO` para cambiar de escena vive en `LevelEditorScene`, porque es una regla de esa escena concreta.

Esta separacion es importante:

- Input global: `Window`.
- Input especifico de una pantalla: escena.

## 15. Render basico

El render actual sigue siendo muy sencillo:

```cpp
void Window::render() {
  glClearColor(m_red, m_green, m_blue, m_alpha);
  glClear(GL_COLOR_BUFFER_BIT);
}
```

`glClearColor` define el color de limpieza y `glClear(GL_COLOR_BUFFER_BIT)` limpia el buffer de color.

Las escenas todavia no dibujan geometria real. Por ahora cambian el color de fondo mediante:

```cpp
Window::get().setClearColor(red, green, blue);
```

Esto permite comprobar visualmente que la escena activa influye en el frame.

## 16. `Scene`

`Scene` es una interfaz en estilo C++: una clase base abstracta con un metodo virtual puro.

```cpp
class Scene {

  public:

    Scene() = default;
    virtual ~Scene() = default;

    virtual void update(float deltaTime) = 0;

};
```

Detalles importantes:

- `virtual ~Scene() = default;` permite destruir escenas derivadas desde un puntero `Scene*` o `std::unique_ptr<Scene>`.
- `update(float deltaTime) = 0;` obliga a cada escena concreta a implementar su logica.
- Al ser abstracta, no se puede crear un objeto `Scene` directamente.

Por eso `Window` guarda:

```cpp
std::unique_ptr<Scene> m_scene;
```

Y crea escenas concretas:

```cpp
m_scene = std::make_unique<LevelEditorScene>();
m_scene = std::make_unique<LevelScene>();
```

## 17. `LevelEditorScene`

`LevelEditorScene` es la primera escena activa.

Estado interno:

```cpp
bool m_changingScene = false;
float m_timeToChangeScene = 2.0f;
```

- `m_changingScene`: indica si la transicion ya empezo.
- `m_timeToChangeScene`: tiempo restante antes de cambiar a `LevelScene`.

Constructor:

```cpp
LevelEditorScene::LevelEditorScene() {
  std::cout << "Inside level editor scene" << std::endl;
  Window::get().setClearColor(0.50f, 0.50f, 0.50f);
}
```

Al entrar en la escena, escribe en consola y define un color gris inicial.

Update:

```cpp
void LevelEditorScene::update(float deltaTime) {
  if (!m_changingScene && KeyListener::isKeyPressed(GLFW_KEY_SPACE)) {
    m_changingScene = true;
    std::cout << "Iniciando transicion a la escena de juego..." << std::endl;
  }

  if (!m_changingScene) {
    return;
  }

  m_timeToChangeScene -= deltaTime;

  const float transitionProgress = std::clamp(1.0f - m_timeToChangeScene, 0.0f, 1.0f);
  const float color = 0.18f * (1.0f - transitionProgress);
  Window::get().setClearColor(color, color, color);

  if (m_timeToChangeScene <= 0.0f) {
    Window::get().changeScene(1);
  }
}
```

La primera parte detecta `ESPACIO`. Si todavia no estaba cambiando de escena, activa la transicion.

La segunda parte sale inmediatamente si la transicion no ha empezado. Esto mantiene el metodo facil de leer.

Cuando la transicion esta activa, se resta `deltaTime` al contador. Al llegar a cero, se pide cambiar a la escena `1`, que actualmente es `LevelScene`.

## 18. `LevelScene`

`LevelScene` representa la escena de juego. Ahora mismo es minima:

```cpp
LevelScene::LevelScene() {
  std::cout << "Inside level scene" << std::endl;
  Window::get().setClearColor(1.0f, 1.0f, 1.0f);
}

void LevelScene::update(float) {
}
```

El constructor muestra un mensaje y cambia el fondo a blanco.

`update(float)` no usa todavia `deltaTime`, pero mantiene la firma exigida por `Scene`. Dejar el parametro sin nombre evita advertencias por variable no usada y comunica que la escena todavia no necesita ese valor.

## 19. Cambio de escena

El cambio de escena se solicita con:

```cpp
Window::get().changeScene(1);
```

`Window::changeScene()` decide si puede cargar la escena inmediatamente o si debe esperar.

```cpp
void Window::changeScene(int scene) {
  if (m_isUpdatingScene) {
    m_pendingScene = scene;
    return;
  }

  loadScene(scene);
}
```

Si no estamos dentro de `Scene::update()`, se llama directamente a `loadScene(scene)`.

Si estamos dentro de `Scene::update()`, se guarda el id en `m_pendingScene`. Esto evita destruir una escena mientras aun se esta ejecutando codigo de esa misma escena.

Despues del `update`, el bucle principal aplica el cambio:

```cpp
if (m_pendingScene >= 0) {
  loadScene(m_pendingScene);
  m_pendingScene = -1;
}
```

## 20. `loadScene()`

`loadScene()` contiene el mapeo actual entre ids y clases:

```cpp
void Window::loadScene(int scene) {
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
```

Actualmente:

- `0`: `LevelEditorScene`.
- `1`: `LevelScene`.

Esto funciona, pero no es la forma mas expresiva. Un siguiente paso razonable seria cambiar esos numeros por un `enum class`, por ejemplo:

```cpp
enum class SceneType {
  LevelEditor,
  Level
};
```

Asi el codigo seria mas legible y menos propenso a errores.

## 21. `KeyListener`

`KeyListener` mantiene el estado de las teclas.

GLFW llama a este callback:

```cpp
static void keyCallback(GLFWwindow* window, int keyCode, int scancode, int action, int mods);
```

Cuando una tecla se pulsa, se marca como `true`. Cuando se suelta, se marca como `false`.

El resto del codigo no necesita saber como funciona GLFW por dentro. Puede preguntar:

```cpp
KeyListener::isKeyPressed(GLFW_KEY_ESCAPE)
KeyListener::isKeyPressed(GLFW_KEY_SPACE)
```

Esto hace que el input sea mas facil de leer desde `Window` o desde una escena.

## 22. `MouseListener`

`MouseListener` mantiene estado de raton:

- Botones pulsados.
- Posicion X/Y.
- Desplazamiento X/Y entre frames.
- Scroll X/Y.
- Estado de arrastre.

Aunque ahora mismo ninguna escena lo usa de forma visible, se mantiene porque es infraestructura del motor. Las demos antiguas ya demostraron que funciona y las proximas escenas podran consultar esa informacion.

La llamada importante al final del frame es:

```cpp
MouseListener::update();
```

Esto copia la posicion actual como posicion anterior y resetea el scroll del frame. Por eso debe ejecutarse despues de que la escena haya tenido oportunidad de leer el input.

## 23. Proyecto Visual Studio

Los ficheros nuevos de escena se han incluido en `GLFWDemo.vcxproj`:

```xml
<ClCompile Include="src\scene\LevelEditorScene.cpp" />
<ClCompile Include="src\scene\LevelScene.cpp" />
<ClInclude Include="src\scene\Scene.h" />
<ClInclude Include="src\scene\LevelEditorScene.h" />
<ClInclude Include="src\scene\LevelScene.h" />
```

Tambien se han incluido en `GLFWDemo.vcxproj.filters` para que Visual Studio los muestre agrupados.

Ademas, el proyecto incluye `$(ProjectDir)src` en las rutas de include. Esto permite que desde `src/scene` se puedan usar includes simples:

```cpp
#include "Window.h"
#include "KeyListener.h"
```

sin tener que escribir rutas relativas fragiles como `../Window.h`.

## 24. Convenciones de nombres

Convenciones actuales del proyecto:

- Clases en `PascalCase`: `Window`, `Scene`, `LevelScene`.
- Metodos en `camelCase`: `run`, `changeScene`, `setClearColor`.
- Miembros privados con prefijo `m_`: `m_scene`, `m_deltaTime`, `m_timeStarted`.
- Miembros estaticos internos tambien siguen `m_` si representan estado privado de la clase, como `TimeUtil::m_timeStarted`.
- Constantes con nombre descriptivo y, cuando existan, idealmente `constexpr`.

Esta convencion no es la unica valida en C++, pero lo importante es que el proyecto sea coherente.

## 25. Codificacion y finales de linea

El proyecto debe mantenerse con:

- UTF-8 sin BOM.
- Finales de linea `LF`.

Esto reduce diferencias innecesarias en Git y evita que Visual Studio, PowerShell o herramientas externas introduzcan cambios de formato no deseados.

## 26. Como compilar

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

## 27. Resumen de cambios de 0.3.1

- Sistema inicial de escenas integrado en el proyecto.
- Nueva clase base abstracta `Scene`.
- Nuevas escenas `LevelEditorScene` y `LevelScene`.
- `Window` posee la escena activa con `std::unique_ptr<Scene>`.
- `Window` calcula `deltaTime` y lo entrega a `Scene::update(float)`.
- Cambio de escena diferido cuando se solicita desde dentro de `update()`.
- Limpieza de demos antiguas de raton, scroll, fade, FPS y pulso visual dentro de `Window`.
- `TimeUtil` simplificado para conservar solo la API usada actualmente.
- `TimeUtil::m_timeStarted` renombrado para seguir la convencion de miembros privados.
- README y tutorial actualizados para reflejar la arquitectura actual.
- Proyecto Visual Studio actualizado con los ficheros de escena y rutas de include.

## 28. Siguientes pasos razonables

- Sustituir ids numericos de escena por `enum class`.
- Crear `SceneManager` si aumenta el numero de escenas.
- Separar renderizado en una clase `Renderer`.
- Crear `WindowConfig` para evitar valores fijos dentro de `Window`.
- Anadir callback de resize y llamar a `glViewport`.
- Dibujar un triangulo como primera geometria OpenGL real.
- Anadir un metodo `render()` a `Scene` cuando las escenas empiecen a dibujar contenido propio.
- Crear una escena de prueba de input para validar teclado y raton desde el nuevo sistema.
