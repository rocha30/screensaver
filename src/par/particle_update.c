#include "particle.h"

#ifdef _OPENMP
#include <omp.h>
#endif

/* Version paralela con OpenMP: misma fisica por particula que la version
 * secuencial (particle_step(), en src/common/particle.c) -- lo unico que
 * cambia es el loop que la recorre.
 *
 * Por que es seguro paralelizar este for con un simple parallel for:
 *   - Cada particula es independiente: no hay colisiones ni datos
 *     compartidos entre particulas (ver comentario en particle.h), asi que
 *     no hace falta ningun lock para leer/escribir particles[i].
 *   - state (SimState) es de solo lectura dentro de particle_step(), varios
 *     hilos pueden leerlo al mismo tiempo sin problema.
 *   - El generador aleatorio (rand_r) usa un estado propio por particula
 *     (p->rng_state), asi que tampoco hay una semilla global compartida
 *     que sincronizar entre hilos.
 *
 * El unico dato que varios hilos "tocarian" a la vez es el contador de
 * particulas recicladas en este frame. En vez de proteger un acumulador
 * compartido con un lock/critical (que serializaria el loop y mataria el
 * speedup), se usa la clausula reduction(+:respawned) de OpenMP: cada hilo
 * acumula su propio total local y OpenMP los suma al final del region
 * paralela. Ese es el mecanismo de sincronia de esta version.
 */
int particles_update(Particle *particles, int n, const SimState *state, float dt) {
    int respawned = 0;

    #pragma omp parallel for schedule(static) reduction(+:respawned)
    for (int i = 0; i < n; i++) {
        if (particle_step(&particles[i], state, dt)) {
            respawned++;
        }
    }

    return respawned;
}
