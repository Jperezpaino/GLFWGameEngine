# GLFW Game Engine 0.4.0

Proyecto demo en C++17 para Visual Studio 2019 que usa GLFW 3.4 para crear una ventana, recibir input y organizar la logica en escenas con actualizacion y renderizado separados.

La version `0.4.0` introduce una demo visual completa con dos escenas: una escena de cuadrado y una escena de triangulo. Ambas escenas dibujan geometria OpenGL basica y cambian entre si con una transicion de fondo temporizada mediante `deltaTime`.

## Requisitos

- Visual Studio 2019 con el workload **Desktop development with C++**.
- Windows SDK instalado desde Visual Studio Installer.
- Configuracion recomendada: `x64` y `Debug` durante desarrollo.

## GLFW

GLFW 3.4 esta copiado dentro del proyecto en `third_party\glfw` y se compila desde sus fuentes junto con la aplicacion. No hace falta generar ni descargar `glfw3.lib`.

El proyecto sigue aceptando la variable de entorno `GLFW_ROOT` si quieres apuntar a otra copia de GLFW con la misma estructura de carpetas. Si no existe esa variable, Visual Studio usa la copia local vendorizada.

## Estructura principal

```text
GLFWDemo/src
|-- main.cpp
|-- Application.h
|-- Application.cpp
|-- Window.h
|-- Window.cpp
|-- KeyListener.h
|-- KeyListener.cpp
|-- MouseListener.h
|-- MouseListener.cpp
|-- scene
|   |-- Scene.h
|   |-- LevelEditorScene.h
|   |-- LevelEditorScene.cpp
|   |-- LevelScene.h
|   `-- LevelScene.cpp
`-- util
    |-- TimeUtil.h
    `-- TimeUtil.cpp
```

## Responsabilidades

- `main.cpp`: punto de entrada real del programa. Crea `Application`, ejecuta `run()` y captura excepciones.
- `Application`: capa principal de la aplicacion. Posee una instancia de `Window`.
- `Window`: encapsula GLFW, el contexto OpenGL, callbacks, bucle principal, input global, calculo de tiempo, escena activa y render base.
- `Scene`: contrato comun de escenas con `init()`, `update(deltaTime)` y `render()`.
- `LevelEditorScene`: escena inicial. Dibuja un cuadrado y cambia al triangulo con `ESPACIO`.
- `LevelScene`: escena de juego/demo. Dibuja un triangulo y vuelve al cuadrado con `ESPACIO`.
- `KeyListener`: conserva el estado del teclado recibido desde callbacks de GLFW.
- `MouseListener`: conserva botones, posicion, desplazamiento y scroll del raton para futuras escenas.
- `TimeUtil`: centraliza el reloj base usado para calcular segundos transcurridos desde el inicio del bucle.

## Demo actual

- Al arrancar se carga `LevelEditorScene`.
- `LevelEditorScene` muestra un cuadrado de colores sobre fondo rojo.
- `LevelScene` muestra un triangulo de colores sobre fondo azul.
- `ESPACIO`: inicia una transicion temporizada hacia la otra escena.
- `ESC`: cierra la ventana.
- La transicion usa `deltaTime`, por lo que no depende de los FPS.
- El fondo interpola suavemente desde el color de la escena actual hasta un color oscuro antes de cambiar.
- El cambio de escena se aplica de forma diferida si se solicita dentro de `Scene::update()`.

## Bucle principal

El flujo de cada frame es:

```text
updateDeltaTime()
glfwPollEvents()
processInput()
scene.update(deltaTime)
aplicar cambio de escena pendiente
render()
  -> limpiar pantalla
  -> scene.render()
glfwSwapBuffers()
MouseListener::update()
```

Este orden separa tres ideas importantes:

- GLFW actualiza eventos antes de que la escena lea input.
- La escena recibe un `deltaTime` estable calculado una sola vez por frame.
- La escena dibuja despues de limpiar la pantalla, por lo que la geometria no se borra antes de mostrarse.

## Render actual

La demo usa OpenGL inmediato (`glBegin`, `glVertex2f`, `glColor3f`) para mantener la primera version visual sencilla y sin introducir todavia un loader como GLAD o GLEW.

Esta tecnica es antigua y no es la ruta recomendada para OpenGL moderno, pero es util para aprender el flujo basico: limpiar pantalla, dibujar una primitiva, intercambiar buffers y repetir.

## Compilar y ejecutar

Desde Visual Studio 2019:

1. Abre `GLFWDemo.sln`.
2. Selecciona `x64`.
3. Selecciona `Debug` o `Release`.
4. Compila y ejecuta `GLFWDemo`.

Desde PowerShell:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe' 'GLFWDemo.sln' /p:Configuration=Debug /p:Platform=x64
```

La aplicacion tambien se puede cerrar usando el boton de cerrar de la ventana.

## Convenciones actuales

- C++17.
- Ficheros en UTF-8 sin BOM.
- Finales de linea `LF`.
- Miembros privados con prefijo `m_`.
- Instancia global controlada de `Window` mediante `Window::get()` mientras el proyecto sigue siendo pequeno.
- Escenas creadas con `std::unique_ptr<Scene>` para aprovechar polimorfismo y propiedad clara.

## Siguientes pasos sugeridos

- Sustituir los ids numericos de escena por un `enum class`.
- Crear un `SceneManager` si aumenta el numero de escenas.
- Mover el render OpenGL a una clase `Renderer`.
- Integrar GLAD o GLEW para usar OpenGL moderno con shaders, VAO, VBO y EBO.
- Crear `WindowConfig` para ancho, alto y titulo.
- Anadir callback de resize para actualizar `glViewport`.
