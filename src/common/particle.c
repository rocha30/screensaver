#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "particle.h"

/* Color base por estacion, con jitter para variar entre particulas. */
static void season_particle_color(Season season, unsigned int *rng_state,
                                   unsigned char *r, unsigned char *g, unsigned char *b) {
    unsigned int jitter = rand_r(rng_state) % 30;
    switch (season) {
        case SEASON_WINTER: /* nieve */
            *r = (unsigned char)(215 + jitter % 40);
            *g = (unsigned char)(215 + jitter % 40);
            *b = 255;
            break;
        case SEASON_AUTUMN: /* hojas */
            *r = (unsigned char)(180 + jitter);
            *g = (unsigned char)(90 + jitter);
            *b = (unsigned char)(30 + jitter / 2);
            break;
        case SEASON_SUMMER: /* polvo/luz */
            *r = (unsigned char)(230 + jitter % 20);
            *g = (unsigned char)(220 + jitter % 20);
            *b = (unsigned char)(150 + jitter);
            break;
        case SEASON_SPRING:
        case SEASON_AUTO:
        default: /* lluvia */
            *r = (unsigned char)(80 + jitter);
            *g = (unsigned char)(140 + jitter);
            *b = (unsigned char)(220 + jitter % 30);
            break;
    }
}

void particle_respawn_top(Particle *p, const SimState *state) {
    p->x = (float)(rand_r(&p->rng_state) % (unsigned int) state->width);
    p->y = -(float)(rand_r(&p->rng_state) % 40);
    p->vx = ((float)(rand_r(&p->rng_state) % 100) - 50.0f) * 0.4f; /* aprox -20..20 px/s */
    p->vy = 40.0f + (float)(rand_r(&p->rng_state) % 60);
    p->wind_phase = (float)(rand_r(&p->rng_state) % 1000) / 1000.0f * 6.2831853f;
    p->radius = 1.5f + (float)(rand_r(&p->rng_state) % 20) / 10.0f;
    season_particle_color(state->season, &p->rng_state, &p->r, &p->g, &p->b);
}

Particle *particles_create(int n, const SimState *state, unsigned int seed) {
    if (n <= 0) return NULL;

    Particle *particles = malloc((size_t) n * sizeof(Particle));
    if (particles == NULL) {
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        /* semilla propia por particula: pensado para la Etapa 2, donde no
         * se puede compartir un rand() global entre hilos sin sincronizarlo */
        particles[i].rng_state = seed + (unsigned int) i * 2654435761u;
        particle_respawn_top(&particles[i], state);
        particles[i].y = (float)(rand_r(&particles[i].rng_state) % (unsigned int) state->height);
    }

    return particles;
}

void particles_free(Particle *particles) {
    free(particles);
}

bool particle_step(Particle *p, const SimState *state, float dt) {
    float wind = sinf(state->time_sec * 1.5f + p->wind_phase) * state->wind_strength;
    p->vx += wind * dt;
    p->vy += state->gravity * dt;

    p->x += p->vx * dt;
    p->y += p->vy * dt;

    if (p->x < 0.0f) {
        p->x = 0.0f;
        p->vx = -p->vx;
    } else if (p->x > (float) state->width) {
        p->x = (float) state->width;
        p->vx = -p->vx;
    }

    if (p->y > (float) state->height) {
        particle_respawn_top(p, state);
        return true;
    }

    return false;
}
