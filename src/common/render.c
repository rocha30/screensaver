#include "render.h"

static void draw_filled_rect(SDL_Renderer *renderer, float x, float y, float w, float h,
                              unsigned char r, unsigned char g, unsigned char b) {
    SDL_Rect rect = { (int) x, (int) y, (int) w, (int) h };
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void render_frame(SDL_Renderer *renderer,
                   const Palette *palette,
                   const Particle *particles, int n,
                   const Dino *dino,
                   const ObstacleField *obstacles,
                   int floor_y) {
    SDL_SetRenderDrawColor(renderer, palette->sky.r, palette->sky.g, palette->sky.b, 255);
    SDL_RenderClear(renderer);

    int window_w = 0, window_h = 0;
    SDL_GetRendererOutputSize(renderer, &window_w, &window_h);
    draw_filled_rect(renderer, 0.0f, (float) floor_y, (float) window_w, (float) (window_h - floor_y),
                      palette->ground.r, palette->ground.g, palette->ground.b);

    for (int i = 0; i < n; i++) {
        const Particle *p = &particles[i];
        draw_filled_rect(renderer, p->x - p->radius, p->y - p->radius,
                          p->radius * 2.0f, p->radius * 2.0f, p->r, p->g, p->b);
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        const Obstacle *o = &obstacles->items[i];
        if (!o->active) continue;
        draw_filled_rect(renderer, o->x, o->y, o->width, o->height, 60, 60, 60);
    }

    draw_filled_rect(renderer, dino->x, dino->y, dino->width, dino->height, 40, 40, 40);

    SDL_RenderPresent(renderer);
}
