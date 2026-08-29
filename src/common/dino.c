#include "dino.h"

Dino dino_create(float floor_y, float x, float width, float height) {
    Dino d;
    d.x = x;
    d.y = floor_y - height;
    d.vy = 0.0f;
    d.width = width;
    d.height = height;
    d.on_ground = true;
    return d;
}

void dino_jump(Dino *dino, float jump_velocity) {
    if (dino->on_ground) {
        dino->vy = -jump_velocity; /* negativo = hacia arriba */
        dino->on_ground = false;
    }
}

void dino_update(Dino *dino, float gravity, float floor_y, float dt) {
    if (dino->on_ground) {
        return;
    }

    dino->vy += gravity * dt;
    dino->y  += dino->vy * dt;

    float ground_top = floor_y - dino->height;
    if (dino->y >= ground_top) {
        dino->y = ground_top;
        dino->vy = 0.0f;
        dino->on_ground = true;
    }
}

bool dino_should_jump(const Dino *dino, float next_obstacle_x, float jump_distance) {
    if (!dino->on_ground || next_obstacle_x < 0.0f) {
        return false;
    }
    float distance = next_obstacle_x - dino->x;
    return distance > 0.0f && distance < jump_distance;
}
