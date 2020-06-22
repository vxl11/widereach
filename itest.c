#include <stdio.h>

#include "widereach.h"

int main() {
	env_t env;
        env.params = params_default();
	env.params->theta = 0.7;
	// int n = 5;
	int n = 100;
	env.params->lambda = 10 * (n + 1);
	srand48(20200621154912);
        env.samples = random_samples(n, n / 2, 2);

	glp_prob *p = milp(&env);

	printf("Integration testing: CPLEX LP compare\n");
	glp_write_lp(p, NULL, "tmp.lp");
	printf("Comparison result:\t[");
	system("cmp itest.lp tmp.lp");
	printf("]\nNo output: MILP matches, differs: error\n");
	system("rm tmp.lp");

	printf("Integration testing: solve relaxation\n");
	glp_simplex(p, NULL);

	printf("Integration testing: solve integer problem\n");
	glp_iocp *parm = iocp(&env);
	glp_intopt(p, parm);
	free(parm);

	glp_delete_prob(p);
        delete_env(&env);
}
