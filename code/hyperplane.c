#include <float.h>
#include <stddef.h>

#include "widereach.h"
#include "helper.h"

void copy_hyperplane(size_t dimension, double *dest, double *src) {
    for (size_t i = 0; i <= dimension; i++) {
        dest[i] = src[i];
    }
}


double *random_hyperplane(size_t dimension) {
    double *w = CALLOC(dimension + 1, double);
    random_unit_vector(dimension, w);
    double *origin = random_point(dimension);
    w[dimension] = 0.;
    for (size_t i = 0; i < dimension; i++) {
        w[dimension] -= w[i] * origin[i];
    }
    free(origin);
    return w;
}


double *best_random_hyperplane(env_t *env) {
    samples_t *samples = env->samples;
    size_t dimension = samples->dimension;
    double best_value = -DBL_MAX;
    double *best_hyperplane = CALLOC(dimension + 1, double);
    params_t *params = env->params;
    for (int k = 0; k < params->rnd_trials; k++) {
        double *hyperplane = random_hyperplane(dimension);
        int X = side_cnt(1, samples, hyperplane, params->epsilon_positive);
        int Y = side_cnt(0, samples, hyperplane, params->epsilon_positive);
        double theta = params->theta;
        double violation = 
            (theta - 1.) * X + theta * (Y + params->epsilon_precision);
        if (violation < 0.) {
            violation = 0.;
        }
        double value = X - params->lambda * violation;
        if (value > best_value) {
            best_value = value;
            copy_hyperplane(dimension, best_hyperplane, hyperplane);
            // glp_printf("%i\t%g\n", k, best_value);
        }
        free(hyperplane);
    }
    return best_hyperplane;
}

