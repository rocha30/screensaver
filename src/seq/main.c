#include "app.h"
#include "config.h"
#include "particle.h"

int main(int argc, char **argv) {
    Config cfg;
    if (parse_args(argc, argv, &cfg) != 0) {
        return 1;
    }
    return run_app(&cfg, particles_update);
}
