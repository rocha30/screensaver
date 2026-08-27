#ifndef DINO_H
#define DINO_H

#include <stdbool.h>

/* El dino "juega solo": salto parabolico, decision de saltar determinista (sin IA). */

typedef struct {
    float x, y;         /* esquina superior izquierda del sprite */
    float vy;
    float width, height;
    bool  on_ground;
} Dino;

Dino dino_create(float floor_y, float x, float width, float height);

/* Si esta en el piso, le da impulso vertical hacia arriba. */
void dino_jump(Dino *dino, float jump_velocity);

/* Integra la trayectoria parabolica y detecta el aterrizaje. */
void dino_update(Dino *dino, float gravity, float floor_y, float dt);

/* Regla de auto-juego: salta si el obstaculo mas cercano esta a menos de
 * jump_distance px y el dino esta en el piso. */
bool dino_should_jump(const Dino *dino, float next_obstacle_x, float jump_distance);

#endif /* DINO_H */
