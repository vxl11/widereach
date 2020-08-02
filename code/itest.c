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
    
    printf("Integration test: obstruction");
    sample_locator_t target = { 0, 0 };
    sample_locator_t source[2], obstruction[2];
    source[0].class = source[1].class = 0;
    source[0].index = 1;
    source[1].index = 2;
    obstruction[0].class = obstruction[1].class = 1;
    obstruction[0].index = 0;
    obstruction[1].index = 1;
    sample_locator_t *source_ptr[2], *obstruction_ptr[2];
    for (int i = 0; i < 2; i++) { 
        source_ptr[i] = source + i;
        obstruction_ptr[i] = obstruction + i;
    }
    int status = 
        is_obstructed(&target, 2, source_ptr, 2, obstruction_ptr, env.samples);
    printf("obstructed? (should be 0): %i\n", status);
    
    delete_env(&env);
}
