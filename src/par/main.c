#include "app.h"
#include "config.h"
#include "particle.h"

#ifdef _OPENMP
#include <omp.h>
#endif

int main(int argc, char **argv) {
    Config cfg;
    if (parse_args(argc, argv, &cfg) != 0) {
        return 1;
    }

#ifdef _OPENMP
    /* cfg->threads == 0 (default) deja que OpenMP decida (normalmente
     * el numero de cores disponibles); si el usuario pasa --threads N
     * se fuerza ese numero de hilos para poder medir speedup a mano. */
    if (cfg.threads > 0) {
        omp_set_num_threads(cfg.threads);
    }
#endif

    return run_app(&cfg, particles_update);
}
