#include <stdio.h>

#include "widereach.h"

#define LINE_MAX 255

int main() {
	env_t env;
        env.params = params_default();
	env.params->theta = 0.7;
	int n = 5;
	env.params->lambda = 10 * (n + 1);
	srand48(20200621154912);
        env.samples = random_samples(n, n / 2, 2);
	env.solution_data = solution_data_init(n);

	glp_prob *p = milp(&env);

	printf("Integration testing: samples\n");
	print_samples(env.samples);
	printf("Integration testing: CPLEX LP compare\n");
	glp_write_lp(p, NULL, "tmp.lp");
	printf("Comparison result:\t");
	char line[LINE_MAX];
	FILE *cmp = popen("cmp itest.lp tmp.lp", "r");
	printf("%s\n", fgets(line, sizeof(line), cmp) == NULL ? 
			"success" : "FAILURE");
	pclose(cmp);
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
