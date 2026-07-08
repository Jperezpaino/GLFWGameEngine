# GLFW Game Engine 0.2.1

Demo base en C++ para Visual Studio 2019 que abre una ventana con GLFW, crea un contexto OpenGL y procesa entrada de teclado y raton mediante listeners propios.

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
`-- MouseListener.cpp
```

- `main.cpp`: punto de entrada minimo.
- `Application`: capa principal de la aplicacion.
- `Window`: encapsula GLFW, la ventana, el bucle principal, el input y el render basico.
- `KeyListener`: guarda el estado de las teclas recibidas desde el callback de GLFW.
- `MouseListener`: guarda el estado de los botones del raton recibidos desde el callback de GLFW.

## Controles

- `ESC`: cierra la ventana.
- Clic izquierdo: activa un fundido progresivo del fondo hacia negro.

## Compilar y ejecutar

1. Abre `GLFWDemo.sln` en Visual Studio 2019.
2. Selecciona `x64` o `x86` y `Debug` o `Release`.
3. Compila y ejecuta `GLFWDemo`.

La aplicacion tambien se puede cerrar usando el boton de cerrar de la ventana.
