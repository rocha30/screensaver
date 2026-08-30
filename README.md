# Screensaver Paralelo — Dino Autojugable

Proyecto 1 de Computación Paralela y Distribuida (UVG, semestre 2 2026).
Ver `Proyecto_1_Computacion_Paralela_Distribuida.md` para el enunciado
completo.

Screensaver estilo "Dino de Chrome": un dinosaurio salta obstáculos solo,
con ciclo de día/noche y estaciones que cambian la paleta de la escena.
El parámetro `N` controla la cantidad de partículas de clima (lluvia,
nieve u hojas según la estación) que caen con física (gravedad + viento).

**Estado actual: Etapa 1 — versión secuencial.** Todavía no hay versión
paralela, benchmarks ni documentación de entrega (anexos/informe); esas
etapas se agregan en orden más adelante.

## Dependencias

- Compilador C con soporte C11 (clang o gcc).
- [SDL2](https://www.libsdl.org/) para gráficos.

**macOS (Homebrew):** `brew install sdl2 pkg-config`
**Debian/Ubuntu:** `sudo apt install libsdl2-dev pkg-config gcc`

## Compilar y ejecutar

```sh
make
./bin/screensaver_seq --n 500
```

Cerrar la ventana o presionar `Esc` para salir. Ver todos los argumentos
con `./bin/screensaver_seq --help`.

## Estructura del proyecto

```
screensaver/
├── include/          # headers del programa
├── src/
│   ├── common/          # lógica compartida: args, partículas, dino,
│   │                       obstáculos, escena, render, timer, loop principal
│   └── seq/               # main.c + particle_update.c (versión secuencial)
├── Makefile
└── README.md
```
