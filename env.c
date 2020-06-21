#include "widereach.h"

env_t *delete_env(env_t *env) {
	delete_samples(env->samples);
	free(env->params);
	return env;
}
