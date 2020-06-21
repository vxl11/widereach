#include "widereach.h"

int main() {
	env_t env;
        env.params = params_default();
        env.samples = random_samples(5, 3, 2);

	glp_prob *p = milp(&env);
	// glp_iocp *parms = iocp(env.params);

	glp_delete_prob(p);
        delete_env(&env);
}
