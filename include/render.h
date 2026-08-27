#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "particle.h"
#include "dino.h"
#include "obstacle.h"
#include "scene.h"

/* Dibujo con SDL2. Se llama siempre desde el hilo principal (SDL_Renderer no es thread-safe). */
void render_frame(SDL_Renderer *renderer,
                   const Palette *palette,
                   const Particle *particles, int n,
                   const Dino *dino,
                   const ObstacleField *obstacles,
                   int floor_y);

#endif /* RENDER_H */
