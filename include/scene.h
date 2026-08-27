#ifndef SCENE_H
#define SCENE_H

#include "config.h"

/* Ciclo de dia/noche y estaciones: paleta de colores en funcion del tiempo. Siempre secuencial. */

typedef struct {
    unsigned char r, g, b;
} Color;

typedef struct {
    Color sky;
    Color ground;
    Color particle_hint;
} Palette;

/* Paleta interpolada para un instante dado (seno sobre time_sec para dia/noche + estacion activa). */
Palette scene_palette_at(float time_sec, float day_length_sec, Season fixed_season);

/* Estacion activa cuando la configuracion pide SEASON_AUTO. */
Season scene_active_season(float time_sec, float season_length_sec);

#endif /* SCENE_H */
