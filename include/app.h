#ifndef APP_H
#define APP_H

#include "config.h"
#include "particle.h"

/*
 * Loop principal (src/common/app.c). update_fn se pasa como puntero a
 * funcion para que la Etapa 2 (version paralela) reuse este mismo run_app()
 * sin duplicar la logica de juego/render.
 */
typedef int (*ParticleUpdateFn)(Particle *particles, int n,
                                 const SimState *state, float dt);

/* Modo benchmark (headless) o interactivo (ventana SDL) segun cfg->benchmark. */
int run_app(const Config *cfg, ParticleUpdateFn update_fn);

#endif /* APP_H */
