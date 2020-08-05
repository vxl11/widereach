#include <stdio.h>

#include "widereach.h"

#define LINE_MAX 255

void compare_files(char *filename) {
    char line[LINE_MAX];
    snprintf(line, LINE_MAX, "cmp %s tmp.lp", filename);
	FILE *cmp = popen(line, "r");
	printf("%s\n", fgets(line, sizeof(line), cmp) == NULL ? 
			"success" : "FAILURE");
	pclose(cmp);
	system("rm tmp.lp");
}

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
	printf("\nIntegration testing: CPLEX LP compare\n");
	glp_write_lp(p, NULL, "tmp.lp");
	printf("Comparison result:\t");
    compare_files("itest.lp");

	printf("\nIntegration testing: solve relaxation\n");
	glp_simplex(p, NULL);

	printf("\nIntegration testing: solve integer problem\n");
	glp_iocp *parm = iocp(&env);
	glp_intopt(p, parm);
	free(parm);

	glp_delete_prob(p);
    
    printf("\nIntegration test: consistency\n");
    p = init_consistency_problem(2);
    sample_locator_t loc;
    for (size_t i = 0; i < 2; i++) {
        loc.class = i;
        for (int j = 0; j < 2; j++) {
            loc.index = j;
            append_sample(p, &loc, &env);
        }
    }
    loc.class = 0;
    loc.index = 2;
    int consistency = is_consistent_with(p, &loc, &env);
    glp_write_lp(p, NULL, "tmp.lp");
    printf("%s\n", !consistency ? "FAILURE": "success");
    glp_delete_prob(p);
    printf("Comparison result:\t");
    compare_files("consistency.lp");
    
    printf("\nIntegration test: obstruction\n");
    sample_locator_t target = { 1, 0 };
    sample_locator_t source, obstruction[2];
    source.class = 1;
    source.index = 1;
    sample_locator_t *obstruction_ptr[2];
    for (int i = 0; i < 2; i++) {
        obstruction[i].class = 0;
        obstruction[i].index = i;
        obstruction_ptr[i] = obstruction + i;
    }
    int status = 
        is_obstructed(&target, &source, 2, obstruction_ptr, env.samples);
    printf("%s (%i)\n", status ? "FAILURE": "success", status);
    
    delete_env(&env);
}
