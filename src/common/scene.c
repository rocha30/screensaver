#include <math.h>
#include "scene.h"

static Color lerp_color(Color a, Color b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    Color out;
    out.r = (unsigned char)(a.r + (b.r - a.r) * t);
    out.g = (unsigned char)(a.g + (b.g - a.g) * t);
    out.b = (unsigned char)(a.b + (b.b - a.b) * t);
    return out;
}

static void season_ground_color(Season season, Color *ground, Color *hint) {
    switch (season) {
        case SEASON_WINTER:
            *ground = (Color){225, 230, 235};
            *hint   = (Color){255, 255, 255};
            break;
        case SEASON_AUTUMN:
            *ground = (Color){150, 100, 60};
            *hint   = (Color){200, 110, 40};
            break;
        case SEASON_SUMMER:
            *ground = (Color){210, 190, 120};
            *hint   = (Color){235, 220, 150};
            break;
        case SEASON_SPRING:
        case SEASON_AUTO:
        default:
            *ground = (Color){90, 150, 70};
            *hint   = (Color){120, 170, 230};
            break;
    }
}

Season scene_active_season(float time_sec, float season_length_sec) {
    if (season_length_sec <= 0.0f) return SEASON_SPRING;
    long index = (long)(time_sec / season_length_sec) % 4;
    switch (index) {
        case 0: return SEASON_SPRING;
        case 1: return SEASON_SUMMER;
        case 2: return SEASON_AUTUMN;
        default: return SEASON_WINTER;
    }
}

Palette scene_palette_at(float time_sec, float day_length_sec, Season fixed_season) {
    Season season = (fixed_season == SEASON_AUTO)
                        ? scene_active_season(time_sec, day_length_sec * 4.0f)
                        : fixed_season;

    /* fase del dia: oscila con seno entre 0 (medianoche) y 1 (mediodia) */
    float phase = (day_length_sec > 0.0f)
                      ? 0.5f + 0.5f * sinf((time_sec / day_length_sec) * 6.2831853f - 1.5707963f)
                      : 1.0f;

    Color night_sky = (Color){15, 15, 45};
    Color day_sky   = (Color){135, 206, 235};

    Palette palette;
    palette.sky = lerp_color(night_sky, day_sky, phase);
    season_ground_color(season, &palette.ground, &palette.particle_hint);
    palette.ground = lerp_color(night_sky, palette.ground, 0.4f + 0.6f * phase);

    return palette;
}
