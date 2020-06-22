#include <stdio.h>

#include "widereach.h"

int main() {
	env_t env;
        env.params = params_default();
	env.params->theta = 0.7;
	int n = 100;
	env.params->lambda = 10 * (n + 1);
	srand48(20200621154912);
        env.samples = random_samples(n, n / 2, 2);

	glp_prob *p = milp(&env);
	glp_simplex(p, NULL);

	glp_iocp *parm = iocp(&env);
	parm->bt_tech = GLP_BT_DFS;
	glp_intopt(p, parm);
	free(parm);

	glp_delete_prob(p);
        delete_env(&env);
}
