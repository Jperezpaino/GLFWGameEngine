# GLFW Game Engine 0.3.0

Demo base en C++ para Visual Studio 2019 que abre una ventana con GLFW, crea un contexto OpenGL y procesa entrada de teclado, raton y tiempo mediante utilidades propias.

## Requisitos

- Visual Studio 2019 con el workload **Desktop development with C++**.
- Windows SDK instalado desde Visual Studio Installer.

## GLFW

GLFW 3.4 esta copiado dentro del proyecto en `third_party\glfw` y se compila desde sus fuentes junto con la demo. No hace falta generar ni descargar `glfw3.lib`.

El proyecto sigue aceptando una variable de entorno `GLFW_ROOT` si quieres apuntar a otra copia de GLFW con la misma estructura de carpetas, pero por defecto usa la copia local.

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
`-- util
    |-- TimeUtil.h
    `-- TimeUtil.cpp
```

- `main.cpp`: punto de entrada minimo.
- `Application`: capa principal de la aplicacion.
- `Window`: encapsula GLFW, la ventana, el bucle principal, el input, el tiempo y el render basico.
- `KeyListener`: guarda el estado de las teclas recibidas desde el callback de GLFW.
- `MouseListener`: guarda el estado de los botones, la posicion, el desplazamiento y el scroll del raton.
- `TimeUtil`: centraliza tiempo transcurrido, delta time y pausa en milisegundos.

## Demo actual

- Mover el raton cambia el color base del fondo usando la posicion X/Y del cursor.
- Usar la rueda del raton sube o baja la intensidad de los colores.
- El fondo tiene un pulso suave basado en tiempo.
- La consola muestra FPS y `dt` una vez por segundo.
- `ESC`: cierra la ventana.
- Clic izquierdo: activa un fundido progresivo hacia negro usando `deltaTime`.

## Compilar y ejecutar

1. Abre `GLFWDemo.sln` en Visual Studio 2019.
2. Selecciona `x64` o `x86` y `Debug` o `Release`.
3. Compila y ejecuta `GLFWDemo`.

La aplicacion tambien se puede cerrar usando el boton de cerrar de la ventana.