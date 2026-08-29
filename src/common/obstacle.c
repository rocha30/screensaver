#include <stdlib.h>
#include "obstacle.h"

ObstacleField obstacles_create(float speed) {
    ObstacleField field;
    field.speed = speed;
    field.spawn_timer = 0.5f;
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        field.items[i].active = false;
    }
    return field;
}

static void spawn_obstacle(ObstacleField *field, float floor_y, int canvas_width) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!field->items[i].active) {
            float height = 20.0f + (float)(rand() % 30);
            field->items[i].width = 12.0f + (float)(rand() % 15);
            field->items[i].height = height;
            field->items[i].x = (float) canvas_width;
            field->items[i].y = floor_y - height;
            field->items[i].active = true;
            return;
        }
    }
}

void obstacles_update(ObstacleField *field, float floor_y, int canvas_width, float dt) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        Obstacle *o = &field->items[i];
        if (!o->active) continue;
        o->x -= field->speed * dt;
        if (o->x + o->width < 0.0f) {
            o->active = false;
        }
    }

    field->spawn_timer -= dt;
    if (field->spawn_timer <= 0.0f) {
        spawn_obstacle(field, floor_y, canvas_width);
        field->spawn_timer = 1.2f + (float)(rand() % 100) / 100.0f;
    }
}

float obstacles_next_x(const ObstacleField *field, float reference_x) {
    float best = -1.0f;
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        const Obstacle *o = &field->items[i];
        if (!o->active || o->x < reference_x) continue;
        if (best < 0.0f || o->x < best) {
            best = o->x;
        }
    }
    return best;
}
