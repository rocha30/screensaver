#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <stdbool.h>

/* Obstaculos que se mueven a velocidad constante y se reciclan al salir de pantalla. */

typedef struct {
    float x, y;
    float width, height;
    bool  active;
} Obstacle;

#define MAX_OBSTACLES 8

typedef struct {
    Obstacle items[MAX_OBSTACLES];
    float    speed;
    float    spawn_timer;
} ObstacleField;

ObstacleField obstacles_create(float speed);

/* Mueve los obstaculos, desactiva los que salen por la izquierda y genera uno nuevo si corresponde. */
void obstacles_update(ObstacleField *field, float floor_y, int canvas_width, float dt);

/* x del obstaculo activo mas cercano por delante de reference_x, o -1 si no hay ninguno. */
float obstacles_next_x(const ObstacleField *field, float reference_x);

#endif /* OBSTACLE_H */
