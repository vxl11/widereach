#include "widereach.h"

env_t *delete_env(env_t *env) {
	delete_samples(env->samples);
	return env;
}
