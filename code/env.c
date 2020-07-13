#include "widereach.h"

env_t *delete_env(env_t *env) {
	free(delete_samples(env->samples));
    solution_data_t *solution_data = env->solution_data;
    if (solution_data != NULL) {
        free(delete_solution_data(env->solution_data));
    }
	free(env->params);
	return env;
}
