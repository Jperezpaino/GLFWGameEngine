# GLFW Demo para Visual Studio 2019

Demo minima en C++ que abre una ventana con GLFW y limpia el fondo con OpenGL.

## Requisitos

- Visual Studio 2019 con el workload **Desktop development with C++**.
- Windows SDK instalado desde Visual Studio Installer.

## GLFW

GLFW 3.4 esta copiado dentro del proyecto en `third_party\glfw` y se compila desde sus fuentes junto con la demo. No hace falta generar ni descargar `glfw3.lib`.

El proyecto sigue aceptando una variable de entorno `GLFW_ROOT` si quieres apuntar a otra copia de GLFW con la misma estructura de carpetas, pero por defecto usa la copia local.

## Compilar y ejecutar

1. Abre `GLFWDemo.sln` en Visual Studio 2019.
2. Selecciona `x64` o `x86` y `Debug` o `Release`.
3. Compila y ejecuta `GLFWDemo`.

La aplicacion cierra al pulsar `Esc` o al cerrar la ventana.
