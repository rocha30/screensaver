# Screensaver Paralelo — Dino Autojugable

Proyecto 1 de Computación Paralela y Distribuida (UVG, semestre 2 2026).
Ver `Proyecto_1_Computacion_Paralela_Distribuida.md` para el enunciado
completo.

Screensaver estilo "Dino de Chrome": un dinosaurio salta obstáculos solo,
con ciclo de día/noche y estaciones que cambian la paleta de la escena.
El parámetro `N` controla la cantidad de partículas de clima (lluvia,
nieve u hojas según la estación) que caen con física (gravedad + viento).

**Estado actual: Etapa 2 — versión secuencial y versión paralela (OpenMP).**
Ambas comparten toda la lógica de juego/render en `src/common/`; solo
difiere el loop que actualiza las partículas de clima
(`particles_update()`), que es la parte medida y paralelizada. Todavía
falta la documentación de entrega (anexos/informe) y correr los
benchmarks formales — se agregan en orden más adelante.

## Dependencias

- Compilador C con soporte C11 (clang o gcc).
- [SDL2](https://www.libsdl.org/) para gráficos.
- OpenMP para la versión paralela. En Linux normalmente ya viene con
  gcc/clang; en macOS hace falta `libomp` de Homebrew (el `cc` de Apple
  no trae OpenMP integrado).

**macOS (Homebrew):** `brew install sdl2 pkg-config libomp`
**Debian/Ubuntu:** `sudo apt install libsdl2-dev pkg-config gcc`

## Compilar y ejecutar

```sh
make                 # compila ambas versiones (seq y par)
./bin/screensaver_seq --n 500
./bin/screensaver_par --n 500 --threads 4
```

Cerrar la ventana o presionar `Esc` para salir. Ver todos los argumentos
con `./bin/screensaver_seq --help` (mismos argumentos en ambos binarios).

`--threads N` fija el número de hilos de OpenMP en la versión paralela
(0 = automático, decide OpenMP). No tiene efecto en la versión secuencial.

### Modo benchmark (sin ventana)

```sh
./bin/screensaver_par --n 5000 --frames 500 --threads 4 --benchmark
```

Corre `frames` pasos del loop de partículas sin abrir SDL y mide el
tiempo total, útil para comparar tiempos entre la versión secuencial y la
paralela con distinto número de hilos. Imprime una línea CSV:
`n,threads,frames,segundos,particulas_recicladas`.

## Estructura del proyecto

```
screensaver/
├── include/          # headers del programa
├── src/
│   ├── common/          # lógica compartida: args, partículas, dino,
│   │                       obstáculos, escena, render, timer, loop principal
│   ├── seq/               # main.c + particle_update.c (versión secuencial)
│   └── par/               # main.c + particle_update.c (versión paralela, OpenMP)
├── Makefile
└── README.md
```
