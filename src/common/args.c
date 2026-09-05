#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"

static int parse_int_arg(const char *value, const char *flag, int *out) {
    char *end = NULL;
    long parsed = strtol(value, &end, 10);
    if (end == value || *end != '\0') {
        fprintf(stderr, "Error: el valor de %s debe ser un entero (recibido: '%s')\n", flag, value);
        return -1;
    }
    *out = (int) parsed;
    return 0;
}

static int parse_season(const char *value, Season *out) {
    if (strcmp(value, "spring") == 0) { *out = SEASON_SPRING; return 0; }
    if (strcmp(value, "summer") == 0) { *out = SEASON_SUMMER; return 0; }
    if (strcmp(value, "autumn") == 0) { *out = SEASON_AUTUMN; return 0; }
    if (strcmp(value, "winter") == 0) { *out = SEASON_WINTER; return 0; }
    if (strcmp(value, "auto") == 0)   { *out = SEASON_AUTO;   return 0; }
    fprintf(stderr, "Error: --season invalido ('%s'). Usar spring|summer|autumn|winter|auto.\n", value);
    return -1;
}

void print_usage(const char *prog_name) {
    fprintf(stderr,
        "Uso: %s [opciones]\n"
        "  --n N          cantidad de particulas de clima (default %d)\n"
        "  --width W       ancho del canvas, minimo %d (default %d)\n"
        "  --height H       alto del canvas, minimo %d (default %d)\n"
        "  --fps F          limite de FPS en modo interactivo (default %d)\n"
        "  --frames F        correr exactamente F frames y salir (default: sin limite)\n"
        "  --threads T        hilos de OpenMP a usar en la version paralela (sin efecto en la secuencial; default: automatico)\n"
        "  --seed S            semilla pseudoaleatoria (default: basada en el reloj)\n"
        "  --season S           spring | summer | autumn | winter | auto (default: auto)\n"
        "  --benchmark           correr sin ventana, solo medir e imprimir tiempos (requiere --frames)\n"
        "  --help                muestra esta ayuda\n",
        prog_name, DEFAULT_N, MIN_WIDTH, DEFAULT_WIDTH, MIN_HEIGHT, DEFAULT_HEIGHT,
        DEFAULT_FPS_CAP);
}

int parse_args(int argc, char **argv, Config *out) {
    out->n         = DEFAULT_N;
    out->width     = DEFAULT_WIDTH;
    out->height    = DEFAULT_HEIGHT;
    out->fps_cap   = DEFAULT_FPS_CAP;
    out->frames    = DEFAULT_FRAMES;
    out->threads   = DEFAULT_THREADS;
    out->seed      = DEFAULT_SEED;
    out->season    = SEASON_AUTO;
    out->benchmark = false;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        int has_value = (i + 1 < argc);

        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            print_usage(argv[0]);
            return -1;
        } else if (strcmp(arg, "--benchmark") == 0) {
            out->benchmark = true;
        } else if (strcmp(arg, "--n") == 0 && has_value) {
            if (parse_int_arg(argv[++i], "--n", &out->n) != 0) return -1;
        } else if (strcmp(arg, "--width") == 0 && has_value) {
            if (parse_int_arg(argv[++i], "--width", &out->width) != 0) return -1;
        } else if (strcmp(arg, "--height") == 0 && has_value) {
            if (parse_int_arg(argv[++i], "--height", &out->height) != 0) return -1;
        } else if (strcmp(arg, "--fps") == 0 && has_value) {
            if (parse_int_arg(argv[++i], "--fps", &out->fps_cap) != 0) return -1;
        } else if (strcmp(arg, "--frames") == 0 && has_value) {
            if (parse_int_arg(argv[++i], "--frames", &out->frames) != 0) return -1;
        } else if (strcmp(arg, "--threads") == 0 && has_value) {
            if (parse_int_arg(argv[++i], "--threads", &out->threads) != 0) return -1;
        } else if (strcmp(arg, "--seed") == 0 && has_value) {
            int seed_value;
            if (parse_int_arg(argv[++i], "--seed", &seed_value) != 0) return -1;
            out->seed = (unsigned int) seed_value;
        } else if (strcmp(arg, "--season") == 0 && has_value) {
            if (parse_season(argv[++i], &out->season) != 0) return -1;
        } else {
            fprintf(stderr, "Error: argumento desconocido o incompleto: '%s'\n", arg);
            print_usage(argv[0]);
            return -1;
        }
    }

    /* Programacion defensiva: validar antes de reservar memoria o abrir SDL */
    if (out->n <= 0) {
        fprintf(stderr, "Error: --n debe ser mayor a 0 (recibido: %d)\n", out->n);
        return -1;
    }
    if (out->width < MIN_WIDTH || out->height < MIN_HEIGHT) {
        fprintf(stderr, "Error: el canvas debe ser al menos %dx%d (recibido: %dx%d)\n",
                MIN_WIDTH, MIN_HEIGHT, out->width, out->height);
        return -1;
    }
    if (out->fps_cap <= 0) {
        fprintf(stderr, "Error: --fps debe ser mayor a 0 (recibido: %d)\n", out->fps_cap);
        return -1;
    }
    if (out->frames < 0) {
        fprintf(stderr, "Error: --frames no puede ser negativo (recibido: %d)\n", out->frames);
        return -1;
    }
    if (out->threads < 0) {
        fprintf(stderr, "Error: --threads no puede ser negativo (recibido: %d)\n", out->threads);
        return -1;
    }
    if (out->benchmark && out->frames <= 0) {
        fprintf(stderr, "Error: --benchmark requiere --frames > 0 (para una medicion reproducible)\n");
        return -1;
    }

    if (out->seed == 0) {
        out->seed = (unsigned int) time(NULL);
    }

    return 0;
}
