#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "app.h"
#include "particle.h"
#include "dino.h"
#include "obstacle.h"
#include "scene.h"
#include "render.h"
#include "timer.h"

/* Constantes de diseno del screensaver (no vienen de argv porque no afectan
 * lo que se mide/paraleliza, solo la estetica). */
#define DAY_LENGTH_SEC     20.0f
#define GRAVITY_PX_S2      500.0f
#define WIND_STRENGTH      15.0f
#define DINO_JUMP_VELOCITY 260.0f
#define DINO_JUMP_DISTANCE 140.0f
#define OBSTACLE_SPEED     180.0f
#define FLOOR_MARGIN_PX    60

/* Modo headless: sin SDL, corre cfg->frames pasos y mide cuanto tarda el ciclo de particulas. */
static int run_benchmark(const Config *cfg, ParticleUpdateFn update_fn) {
    SimState state = {
        .width = cfg->width, .height = cfg->height, .time_sec = 0.0f,
        .gravity = GRAVITY_PX_S2, .wind_strength = WIND_STRENGTH, .season = cfg->season
    };

    Particle *particles = particles_create(cfg->n, &state, cfg->seed);
    if (particles == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria para %d particulas\n", cfg->n);
        return 1;
    }

    const float dt = 1.0f / (float) cfg->fps_cap;
    long total_respawns = 0;

    double start = timer_now_seconds();
    for (int frame = 0; frame < cfg->frames; frame++) {
        state.time_sec = (float) frame * dt;
        total_respawns += update_fn(particles, cfg->n, &state, dt);
    }
    double elapsed = timer_now_seconds() - start;

    printf("%d,%d,%d,%.6f,%ld\n", cfg->n, cfg->threads, cfg->frames, elapsed, total_respawns);

    particles_free(particles);
    return 0;
}

static int run_interactive(const Config *cfg, ParticleUpdateFn update_fn) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error al inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Screensaver Paralelo - Dino",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        cfg->width, cfg->height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Error al crear la ventana: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        fprintf(stderr, "Error al crear el renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SimState state = {
        .width = cfg->width, .height = cfg->height, .time_sec = 0.0f,
        .gravity = GRAVITY_PX_S2, .wind_strength = WIND_STRENGTH, .season = cfg->season
    };

    Particle *particles = particles_create(cfg->n, &state, cfg->seed);
    if (particles == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria para %d particulas\n", cfg->n);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int floor_y = cfg->height - FLOOR_MARGIN_PX;
    Dino dino = dino_create((float) floor_y, (float) (cfg->width / 6), 44.0f, 48.0f);
    ObstacleField obstacles = obstacles_create(OBSTACLE_SPEED);

    bool running = true;
    int frame_count = 0;
    double last_time = timer_now_seconds();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
        }

        double now = timer_now_seconds();
        float dt = (float) (now - last_time);
        if (dt > 0.05f) dt = 0.05f; /* evita saltos grandes tras una pausa (ej. mover la ventana) */
        last_time = now;
        state.time_sec += dt;

        update_fn(particles, cfg->n, &state, dt);

        obstacles_update(&obstacles, (float) floor_y, cfg->width, dt);
        if (dino_should_jump(&dino, obstacles_next_x(&obstacles, dino.x), DINO_JUMP_DISTANCE)) {
            dino_jump(&dino, DINO_JUMP_VELOCITY);
        }
        dino_update(&dino, GRAVITY_PX_S2, (float) floor_y, dt);

        Palette palette = scene_palette_at(state.time_sec, DAY_LENGTH_SEC, cfg->season);
        render_frame(renderer, &palette, particles, cfg->n, &dino, &obstacles, floor_y);

        frame_count++;
        if (cfg->frames > 0 && frame_count >= cfg->frames) {
            running = false;
        }
    }

    particles_free(particles);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int run_app(const Config *cfg, ParticleUpdateFn update_fn) {
    if (cfg->benchmark) {
        return run_benchmark(cfg, update_fn);
    }
    return run_interactive(cfg, update_fn);
}
