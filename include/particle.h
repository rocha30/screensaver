#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdbool.h>
#include "config.h"

/*
 * Particulas de clima (lluvia/nieve/hojas). Cada una es independiente de
 * las demas (sin colision entre particulas), por diseno: en la Etapa 2
 * esto permite paralelizar particles_update() sin locks.
 */

typedef struct {
    float x, y;
    float vx, vy;
    float wind_phase;          /* fase del balanceo senoidal del viento */
    float radius;
    unsigned char r, g, b;
    unsigned int rng_state;     /* generador propio (rand_r), independiente por particula */
} Particle;

/* Estado de la escena, solo lectura para la fisica */
typedef struct {
    int   width;
    int   height;
    float time_sec;
    float gravity;
    float wind_strength;
    Season season;         /* determina el color al reciclar una particula */
} SimState;

/* Reserva y llena n particulas. Devuelve NULL si malloc falla (revisar siempre). */
Particle *particles_create(int n, const SimState *state, unsigned int seed);

/* Libera lo reservado por particles_create(). Acepta NULL. */
void particles_free(Particle *particles);

/* Reubica una particula arriba del canvas con nueva posicion/velocidad/color. */
void particle_respawn_top(Particle *p, const SimState *state);

/* Fisica de una particula: gravedad + viento, rebote lateral, reciclaje al salir por abajo.
 * Devuelve true si se reciclo en este paso. */
bool particle_step(Particle *p, const SimState *state, float dt);

/* Recorre las n particulas llamando particle_step(). Devuelve cuantas se reciclaron
 * en el frame. Version secuencial en src/seq/particle_update.c, version
 * paralela (OpenMP) en src/par/particle_update.c -- misma firma, mismo
 * particle_step(), solo cambia el loop. */
int particles_update(Particle *particles, int n, const SimState *state, float dt);

#endif /* PARTICLE_H */
