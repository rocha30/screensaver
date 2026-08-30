#include "particle.h"

/* Version secuencial: recorre las N particulas con un for comun. La Etapa 2
 * agrega la version paralela con la misma firma para comparar el speedup. */
int particles_update(Particle *particles, int n, const SimState *state, float dt) {
    int respawned = 0;
    for (int i = 0; i < n; i++) {
        if (particle_step(&particles[i], state, dt)) {
            respawned++;
        }
    }
    return respawned;
}
