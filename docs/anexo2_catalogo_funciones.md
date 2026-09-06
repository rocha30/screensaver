# Anexo 2 — Catálogo de funciones

Para cada función: **entradas** (nombre, tipo, uso), **salidas** (nombre,
tipo, uso) y **descripción** (propósito y funcionamiento). Los tipos
`Config`, `SimState`, `Particle`, `Dino`, `ObstacleField`, `Obstacle`,
`Palette`, `Color` y `Season` están definidos en `include/*.h`.

---

## `src/common/args.c`

### `parse_args`
```c
int parse_args(int argc, char **argv, Config *out);
```
- **Entradas:** `argc` (`int`, cantidad de argumentos de línea de
  comandos); `argv` (`char **`, los argumentos crudos); `out` (`Config *`,
  struct a llenar).
- **Salidas:** valor de retorno `int` (`0` si el parseo y la validación
  fueron correctos, `-1` si hubo error); `*out` queda lleno con los
  valores finales (defaults + lo leído de `argv`).
- **Descripción:** recorre `argv` reconociendo las banderas
  (`--n`, `--width`, `--height`, `--fps`, `--frames`, `--threads`,
  `--seed`, `--season`, `--benchmark`, `--help`). Aplica defaults antes de
  parsear, convierte los valores numéricos con `strtol` validando que no
  queden caracteres sobrantes, y al final corre la validación defensiva
  de rangos (incluye la regla cruzada `--benchmark` requiere
  `--frames > 0`). Si `--seed` no se especifica (queda en 0), la
  reemplaza con `time(NULL)`.

### `parse_int_arg` *(estática)*
```c
static int parse_int_arg(const char *value, const char *flag, int *out);
```
- **Entradas:** `value` (`const char *`, texto a convertir); `flag`
  (`const char *`, nombre de la bandera, solo para el mensaje de error);
  `out` (`int *`, destino del valor convertido).
- **Salidas:** retorno `int` (`0` éxito, `-1` error); `*out` con el valor
  parseado si tuvo éxito.
- **Descripción:** wrapper defensivo sobre `strtol()`: si la cadena no es
  un entero válido completo, imprime un error específico en `stderr` y
  retorna `-1` en vez de dejar pasar un valor basura.

### `parse_season` *(estática)*
```c
static int parse_season(const char *value, Season *out);
```
- **Entradas:** `value` (`const char *`, uno de
  `spring|summer|autumn|winter|auto`); `out` (`Season *`, destino).
- **Salidas:** retorno `int` (`0` éxito, `-1` valor inválido); `*out` con
  el enum correspondiente.
- **Descripción:** mapea el string de `--season` al enum `Season`;
  rechaza cualquier valor no reconocido con mensaje de error.

### `print_usage`
```c
void print_usage(const char *prog_name);
```
- **Entradas:** `prog_name` (`const char *`, típicamente `argv[0]`).
- **Salidas:** ninguna (efecto: imprime a `stderr`).
- **Descripción:** imprime la ayuda con todas las banderas soportadas y
  sus valores por defecto; se llama tanto en `--help` como cuando el
  parseo falla.

---

## `src/common/particle.c`

### `particles_create`
```c
Particle *particles_create(int n, const SimState *state, unsigned int seed);
```
- **Entradas:** `n` (`int`, cantidad de partículas a crear, ya validado
  `>0` por `parse_args`); `state` (`const SimState *`, dimensiones del
  canvas y estación activa, usado para ubicar cada partícula); `seed`
  (`unsigned int`, semilla base).
- **Salidas:** `Particle *` — puntero al arreglo reservado con `malloc`,
  o `NULL` si `n<=0` o si `malloc` falla (revisado siempre por el
  llamador).
- **Descripción:** reserva `n * sizeof(Particle)` bytes y llena cada
  partícula con `particle_respawn_top()`, dándole a cada una un
  `rng_state` propio derivado de `seed` (multiplicador
  `2654435761u`, constante de hash tipo Knuth) para que ningún hilo
  necesite compartir generador aleatorio en la Etapa 2. La `y` inicial se
  aleatoriza dentro de toda la altura del canvas (no solo arriba), para
  que la escena no arranque vacía.

### `particles_free`
```c
void particles_free(Particle *particles);
```
- **Entradas:** `particles` (`Particle *`, puede ser `NULL`).
- **Salidas:** ninguna.
- **Descripción:** libera la memoria reservada por `particles_create()`;
  `free(NULL)` es válido en C, así que no hace falta chequeo previo.

### `particle_respawn_top`
```c
void particle_respawn_top(Particle *p, const SimState *state);
```
- **Entradas:** `p` (`Particle *`, partícula a reubicar); `state`
  (`const SimState *`, ancho del canvas y estación, para color/posición).
- **Salidas:** ninguna (modifica `*p` in-place).
- **Descripción:** reposiciona la partícula arriba del canvas
  (`y` negativa) con nueva `x`, velocidad `vx/vy`, fase de viento y color
  aleatorios (vía `rand_r(&p->rng_state)`, estado propio de la
  partícula). Se llama tanto al crear el arreglo como cada vez que una
  partícula sale por abajo del canvas en `particle_step()`.

### `season_particle_color` *(estática)*
```c
static void season_particle_color(Season season, unsigned int *rng_state,
                                   unsigned char *r, unsigned char *g, unsigned char *b);
```
- **Entradas:** `season` (`Season`, estación activa); `rng_state`
  (`unsigned int *`, estado del RNG de la partícula, se actualiza).
- **Salidas:** `*r, *g, *b` (`unsigned char`, color RGB calculado).
- **Descripción:** color base según la estación (blanco-azulado para
  nieve, tonos cafés/naranja para hojas de otoño, dorado para
  polvo/verano, azul-grisáceo para lluvia) más un jitter aleatorio para
  que no todas las partículas se vean idénticas.

### `particle_step`
```c
bool particle_step(Particle *p, const SimState *state, float dt);
```
- **Entradas:** `p` (`Particle *`, partícula a simular); `state`
  (`const SimState *`, solo lectura: gravedad, viento, tiempo, tamaño de
  canvas); `dt` (`float`, delta de tiempo del frame en segundos).
- **Salidas:** retorno `bool` (`true` si la partícula se recicló en este
  paso); modifica `p->x, p->y, p->vx, p->vy` in-place.
- **Descripción:** física de la partícula — viento lateral sinusoidal
  (`sinf(time_sec * 1.5 + wind_phase) * wind_strength`, el elemento de
  **trigonometría** del proyecto), gravedad constante sobre `vy`,
  integración de posición (`x += vx*dt`, `y += vy*dt`), rebote elástico
  en los bordes izquierdo/derecho, y reciclaje (`particle_respawn_top`)
  si `y` supera la altura del canvas. Es la función que se llama en el
  loop de `particles_update()`, tanto en serie como en paralelo — es la
  unidad de trabajo independiente que hace segura la paralelización.

### `particles_update` (versión secuencial)
```c
int particles_update(Particle *particles, int n, const SimState *state, float dt);
```
*(`src/seq/particle_update.c`)*
- **Entradas:** `particles` (`Particle *`, arreglo de `n` partículas);
  `n` (`int`); `state` (`const SimState *`); `dt` (`float`).
- **Salidas:** retorno `int` — cantidad de partículas recicladas en este
  frame.
- **Descripción:** `for` secuencial de `0` a `n-1` que llama
  `particle_step()` por cada partícula y acumula cuántas se reciclaron.

### `particles_update` (versión paralela, OpenMP)
```c
int particles_update(Particle *particles, int n, const SimState *state, float dt);
```
*(`src/par/particle_update.c`)*
- **Entradas / salidas:** misma firma y mismo significado que la versión
  secuencial (son intercambiables vía el puntero a función
  `ParticleUpdateFn`).
- **Descripción:** mismo trabajo por partícula (`particle_step`), pero el
  `for` lleva `#pragma omp parallel for schedule(static)
  reduction(+:respawned)`: OpenMP reparte los índices `i` entre los hilos
  disponibles (`schedule(static)`, bloques contiguos) y cada hilo lleva su
  propio acumulador de `respawned` que se combina al cerrar la región
  paralela — el mecanismo de sincronía de esta versión. No hay lectura ni
  escritura compartida entre partículas ni un RNG global, así que no hace
  falta ningún `critical`/lock adicional.

---

## `src/common/dino.c`

### `dino_create`
```c
Dino dino_create(float floor_y, float x, float width, float height);
```
- **Entradas:** `floor_y` (`float`, altura en px del piso); `x`
  (`float`, posición horizontal fija del dino); `width, height`
  (`float`, tamaño del sprite).
- **Salidas:** `Dino` (por valor) inicializado parado sobre el piso
  (`on_ground = true`, `vy = 0`).
- **Descripción:** constructor simple del struct `Dino`.

### `dino_jump`
```c
void dino_jump(Dino *dino, float jump_velocity);
```
- **Entradas:** `dino` (`Dino *`); `jump_velocity` (`float`, magnitud del
  impulso).
- **Salidas:** ninguna (modifica `dino->vy` y `dino->on_ground`).
- **Descripción:** si el dino está en el piso, le da velocidad vertical
  negativa (hacia arriba); no hace nada si ya está en el aire.

### `dino_update`
```c
void dino_update(Dino *dino, float gravity, float floor_y, float dt);
```
- **Entradas:** `dino` (`Dino *`); `gravity` (`float`, aceleración
  constante); `floor_y` (`float`); `dt` (`float`).
- **Salidas:** ninguna (integra `vy` y `y`, detecta aterrizaje).
- **Descripción:** si está en el piso no hace nada; si está en el aire,
  integra la trayectoria parabólica (gravedad sobre `vy`, `vy` sobre `y`)
  y aterriza (`on_ground = true`, `vy = 0`) al llegar al piso.

### `dino_should_jump`
```c
bool dino_should_jump(const Dino *dino, float next_obstacle_x, float jump_distance);
```
- **Entradas:** `dino` (`const Dino *`); `next_obstacle_x` (`float`,
  posición x del obstáculo activo más cercano, o `-1` si no hay);
  `jump_distance` (`float`, umbral de distancia para saltar).
- **Salidas:** retorno `bool` — si el dino debe saltar este frame.
- **Descripción:** regla determinista de auto-juego (no es IA): salta si
  está en el piso, hay un obstáculo por delante, y la distancia a él es
  positiva y menor a `jump_distance`.

---

## `src/common/obstacle.c`

### `obstacles_create`
```c
ObstacleField obstacles_create(float speed);
```
- **Entradas:** `speed` (`float`, velocidad horizontal de los
  obstáculos).
- **Salidas:** `ObstacleField` (por valor) con todos los slots
  inactivos y un `spawn_timer` inicial de 0.5s.
- **Descripción:** constructor del contenedor de hasta `MAX_OBSTACLES`
  (8) obstáculos, reservado como arreglo fijo (sin `malloc`).

### `spawn_obstacle` *(estática)*
```c
static void spawn_obstacle(ObstacleField *field, float floor_y, int canvas_width);
```
- **Entradas:** `field` (`ObstacleField *`); `floor_y` (`float`);
  `canvas_width` (`int`).
- **Salidas:** ninguna (activa el primer slot libre).
- **Descripción:** busca el primer `Obstacle` inactivo y lo reinicializa
  al borde derecho del canvas con ancho/alto pseudoaleatorios (`rand()`).

### `obstacles_update`
```c
void obstacles_update(ObstacleField *field, float floor_y, int canvas_width, float dt);
```
- **Entradas:** `field` (`ObstacleField *`); `floor_y` (`float`);
  `canvas_width` (`int`); `dt` (`float`).
- **Salidas:** ninguna.
- **Descripción:** mueve cada obstáculo activo a la izquierda
  (`x -= speed*dt`), desactiva los que salen completamente de pantalla, y
  cuenta regresiva `spawn_timer` para generar el siguiente obstáculo con
  `spawn_obstacle()`.

### `obstacles_next_x`
```c
float obstacles_next_x(const ObstacleField *field, float reference_x);
```
- **Entradas:** `field` (`const ObstacleField *`); `reference_x`
  (`float`, posición desde la que se busca, típicamente `dino->x`).
- **Salidas:** retorno `float` — `x` del obstáculo activo más cercano por
  delante de `reference_x`, o `-1` si no hay ninguno.
- **Descripción:** recorre los `MAX_OBSTACLES` slots buscando el mínimo
  `x` entre los activos que estén por delante de la referencia; usado por
  `dino_should_jump()`.

---

## `src/common/scene.c`

### `scene_active_season`
```c
Season scene_active_season(float time_sec, float season_length_sec);
```
- **Entradas:** `time_sec` (`float`, tiempo transcurrido); `season_length_sec`
  (`float`, duración de cada estación).
- **Salidas:** retorno `Season` — estación activa (ciclo
  spring→summer→autumn→winter).
- **Descripción:** calcula el índice de estación con
  `(long)(time_sec / season_length_sec) % 4`; si `season_length_sec<=0`
  devuelve `SEASON_SPRING` por seguridad (evita división por cero).

### `season_ground_color` *(estática)*
```c
static void season_ground_color(Season season, Color *ground, Color *hint);
```
- **Entradas:** `season` (`Season`).
- **Salidas:** `*ground, *hint` (`Color`, color de piso y color de acento
  para partículas, según la estación).
- **Descripción:** tabla de colores base por estación (blanco invierno,
  café/naranja otoño, dorado verano, verde/celeste primavera-default).

### `lerp_color` *(estática)*
```c
static Color lerp_color(Color a, Color b, float t);
```
- **Entradas:** `a, b` (`Color`, colores extremos); `t` (`float`,
  interpolación, se clampa a `[0,1]`).
- **Salidas:** retorno `Color` — interpolación lineal canal por canal.
- **Descripción:** utilidad de mezcla de color usada para las
  transiciones día/noche.

### `scene_palette_at`
```c
Palette scene_palette_at(float time_sec, float day_length_sec, Season fixed_season);
```
- **Entradas:** `time_sec` (`float`); `day_length_sec` (`float`,
  duración de un ciclo día/noche); `fixed_season` (`Season`, o
  `SEASON_AUTO` para que cicle sola).
- **Salidas:** retorno `Palette` — colores de cielo, piso y acento de
  partículas para ese instante.
- **Descripción:** calcula la fase día/noche con
  `sinf()` (**trigonometría**) sobre `time_sec/day_length_sec`, interpola
  entre cielo nocturno y diurno, y ajusta el color de piso de la estación
  activa según esa misma fase (más oscuro de noche).

---

## `src/common/render.c`

### `draw_filled_rect` *(estática)*
```c
static void draw_filled_rect(SDL_Renderer *renderer, float x, float y, float w, float h,
                              unsigned char r, unsigned char g, unsigned char b);
```
- **Entradas:** `renderer` (`SDL_Renderer *`); `x,y,w,h` (`float`,
  posición y tamaño); `r,g,b` (`unsigned char`, color).
- **Salidas:** ninguna (dibuja en el framebuffer del renderer).
- **Descripción:** wrapper sobre `SDL_SetRenderDrawColor` +
  `SDL_RenderFillRect`, usado para dibujar partículas, obstáculos, dino y
  piso con la misma primitiva.

### `render_frame`
```c
void render_frame(SDL_Renderer *renderer, const Palette *palette,
                   const Particle *particles, int n, const Dino *dino,
                   const ObstacleField *obstacles, int floor_y);
```
- **Entradas:** `renderer` (`SDL_Renderer *`); `palette` (`const Palette *`,
  colores del frame actual); `particles` (`const Particle *`, arreglo de
  `n`); `n` (`int`); `dino` (`const Dino *`); `obstacles`
  (`const ObstacleField *`); `floor_y` (`int`).
- **Salidas:** ninguna (efecto: dibuja y hace `SDL_RenderPresent`).
- **Descripción:** limpia con el color de cielo, dibuja el piso, cada
  partícula (círculo aproximado con un rectángulo centrado en su radio),
  cada obstáculo activo y el dino, y presenta el frame. Se llama siempre
  desde el hilo principal — `SDL_Renderer` no es thread-safe, por eso el
  render nunca se toca desde la sección paralela.

---

## `src/common/timer.c`

### `timer_now_seconds`
```c
double timer_now_seconds(void);
```
- **Entradas:** ninguna.
- **Salidas:** retorno `double` — segundos desde una referencia fija del
  sistema (no época de calendario).
- **Descripción:** wrapper sobre `clock_gettime(CLOCK_MONOTONIC, ...)`;
  reloj monotónico (no se ve afectado por ajustes de hora del sistema),
  usado tanto para calcular `dt` en modo interactivo como para medir el
  tiempo total en modo `--benchmark`.

---

## `src/common/app.c`

### `run_benchmark` *(estática)*
```c
static int run_benchmark(const Config *cfg, ParticleUpdateFn update_fn);
```
- **Entradas:** `cfg` (`const Config *`, configuración ya validada);
  `update_fn` (`ParticleUpdateFn`, puntero a la versión seq o par de
  `particles_update`).
- **Salidas:** retorno `int` (`0` éxito, `1` si falla la reserva de
  partículas); imprime una línea CSV a stdout.
- **Descripción:** modo headless (sin SDL): crea las partículas, corre
  `cfg->frames` iteraciones llamando `update_fn()` y mide el tiempo total
  con `timer_now_seconds()`. Imprime
  `n,threads,frames,segundos,particulas_recicladas` — el despliegue de
  resultados del modo benchmark, la fuente de datos del Anexo 3.

### `run_interactive` *(estática)*
```c
static int run_interactive(const Config *cfg, ParticleUpdateFn update_fn);
```
- **Entradas:** `cfg` (`const Config *`); `update_fn`
  (`ParticleUpdateFn`).
- **Salidas:** retorno `int` (`0` éxito, `1` si falla `SDL_Init`,
  creación de ventana/renderer o la reserva de partículas).
- **Descripción:** inicializa SDL, crea ventana/renderer, partículas,
  dino y campo de obstáculos; corre el loop principal (eventos → `dt` →
  `update_fn()` → lógica de obstáculos/dino → paleta → `render_frame()`)
  hasta que el usuario cierra la ventana, presiona `Esc`, o se alcanza
  `cfg->frames` (si es distinto de 0); libera todo al salir.

### `run_app`
```c
int run_app(const Config *cfg, ParticleUpdateFn update_fn);
```
- **Entradas:** `cfg` (`const Config *`); `update_fn`
  (`ParticleUpdateFn`, inyectado desde `main()` — `particles_update` de
  `src/seq/` o `src/par/`).
- **Salidas:** retorno `int` — código de salida del proceso.
- **Descripción:** punto de entrada compartido por ambos binarios;
  decide entre `run_benchmark()` o `run_interactive()` según
  `cfg->benchmark`. Es lo que permite que `src/seq/main.c` y
  `src/par/main.c` sean casi idénticos.

---

## `src/seq/main.c` y `src/par/main.c`

### `main` (secuencial)
```c
int main(int argc, char **argv);
```
- **Entradas:** `argc, argv` (estándar de C).
- **Salidas:** retorno `int` — código de salida del proceso.
- **Descripción:** `parse_args()` y, si tuvo éxito, `run_app(&cfg,
  particles_update)` usando la versión secuencial de `particles_update`
  (enlazada desde `src/seq/particle_update.c`).

### `main` (paralelo)
```c
int main(int argc, char **argv);
```
- **Entradas:** `argc, argv`.
- **Salidas:** retorno `int`.
- **Descripción:** igual que la versión secuencial, pero antes de
  `run_app()` llama `omp_set_num_threads(cfg.threads)` si el usuario pasó
  `--threads N > 0` (si no, deja que OpenMP decida el número de hilos por
  default). Usa la versión paralela de `particles_update` (enlazada desde
  `src/par/particle_update.c`).
