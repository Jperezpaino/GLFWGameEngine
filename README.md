# GLFW Game Engine 0.1.0

Demo base en C++ para Visual Studio 2019 que abre una ventana con GLFW, crea un contexto OpenGL y limpia el fondo en rojo.

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
`-- Window.cpp
```

- `main.cpp`: punto de entrada minimo.
- `Application`: capa principal de la aplicacion.
- `Window`: encapsula GLFW, la ventana y el bucle basico.

## Compilar y ejecutar

1. Abre `GLFWDemo.sln` en Visual Studio 2019.
2. Selecciona `x64` o `x86` y `Debug` o `Release`.
3. Compila y ejecuta `GLFWDemo`.

La aplicacion se cierra al cerrar la ventana.
