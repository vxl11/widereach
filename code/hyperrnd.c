#include <stdio.h>
#include <float.h>

#include "widereach.h"

#define SAMPLE_SEEDS 3
unsigned long int samples_seeds[SAMPLE_SEEDS] = {
    85287339, // 412
    20200621154912, // 378
    20200623170005 // 433
};

#define HYPER_SEEDS 30
unsigned int hyper_seeds[HYPER_SEEDS] = {
    734517477, 145943044, 869199209, 499223379, 523437323, 964156444,
    248689460, 115706114, 711104006, 311906069, 205328448, 471055100,
    307531192, 543901355, 24851720, 704008414, 2921762, 181094221,
    234474543, 782516264, 519948660, 115033019, 205486123, 657145193,
    83898336, 41744843, 153111583, 318522606, 952537249, 298531860
};

#define TRIALS 100000
// #define TRIALS 10000

double single_run(int s, int t, env_t *env) {
    if (t > 0) {
        srand48(hyper_seeds[t-1]);
    }
    
    samples_t *samples = env->samples;
    params_t *params = env->params;
    double best_value = -DBL_MAX;
    for (int k = 0; k < params->rnd_trials; k++) {
        double *hyperplane = random_hyperplane(samples->dimension);
        int X = side_cnt(1, samples, hyperplane, params->epsilon_positive);
        int Y = side_cnt(0, samples, hyperplane, params->epsilon_positive);
        free(hyperplane);
        double theta = params->theta;
        double violation = 
            (theta - 1.) * X + theta * (Y + params->epsilon_precision);
        if (violation < 0.) {
            violation = 0.;
        }
        double value = X - params->lambda * violation;
        if (value > best_value) {
            best_value = value;
            glp_printf("%i\t%i\t%i\t%g\n", s, t, k, best_value);
        }
    }
    
    return best_value;
}

int main() {
    env_t env;
    env.params = params_default();
    env.params->theta = 0.51;
    // env.params->theta = 0.7;
    int n = 10000;
    env.params->lambda = 100 * (n + 1);
    env.params->rnd_trials = TRIALS;
    
    for (size_t dimension = 2; dimension <= 2; dimension += dimension) {
        // TODO #pragma omp parallel for
        for (int s = 0; s < SAMPLE_SEEDS; s++) {
        // for (int s = 0; s < 1; s++) {
            srand48(samples_seeds[s]);
    
            samples_t *samples = random_samples(n, n / 2, dimension);
            // print_samples(env.samples);
            env.samples = samples;
    
            // for (int t = 0; t <= HYPER_SEEDS; t++) {    
            for (int t = 0; t < 1; t++) {
                // double value = 
                single_run(s, t, &env);
                // glp_printf("%g\n", value);
            }
            // glp_printf("---\n");
        }
    }
}
