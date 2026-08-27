#ifndef CONFIG_H
#define CONFIG_H

/* Valores por defecto de configuracion y su struct, poblada desde argv (ver args.c). */

#include <stdbool.h>

#define DEFAULT_N            300     /* cantidad de particulas de clima   */
#define DEFAULT_WIDTH        800     /* ancho del canvas (min. 640)       */
#define DEFAULT_HEIGHT       600     /* alto del canvas  (min. 480)       */
#define DEFAULT_FPS_CAP      60
#define DEFAULT_FRAMES       0       /* 0 = correr indefinidamente        */
#define DEFAULT_THREADS      0       /* 0 = default de OpenMP             */
#define DEFAULT_SEED         0       /* 0 = semilla basada en el reloj    */

#define MIN_WIDTH   640
#define MIN_HEIGHT  480

typedef enum {
    SEASON_AUTO = 0,   /* la estacion cambia sola con el tiempo */
    SEASON_SPRING,
    SEASON_SUMMER,
    SEASON_AUTUMN,
    SEASON_WINTER
} Season;

typedef struct {
    int    n;
    int    width;
    int    height;
    int    fps_cap;
    int    frames;          /* 0 = sin limite */
    int    threads;         /* sin efecto en la version secuencial */
    unsigned int seed;
    Season season;
    bool   benchmark;       /* true = correr sin ventana, solo medir tiempos */
} Config;

/* Parsea argv, aplica defaults y valida rangos. Devuelve 0 si OK, -1 si hubo error (mensaje en stderr). */
int parse_args(int argc, char **argv, Config *out);

void print_usage(const char *prog_name);

#endif /* CONFIG_H */
