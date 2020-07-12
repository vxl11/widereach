#include <stdio.h>

#include "widereach.h"

int main() {
	env_t env;
    env.params = params_default();
	env.params->theta = 0.51;
    // env.params->theta = 0.55;
	env.params->branch_target = 0.0;
	env.params->iheur_method = deep;
	int n = 1000;
	env.params->lambda = 100 * (n + 1);
	// srand48(20200621154912); // 378
	// srand48(20200623170005); // 433
	srand48(85287339);  // 412
        env.samples = random_samples(n, n / 2, 2);
	// print_samples(env.samples);
	env.solution_data = solution_data_init(n);

	glp_prob *p = milp(&env);
	// glp_write_lp(p, NULL, "tmp.lp");
	glp_simplex(p, NULL);

	glp_iocp *parm = iocp(&env);
    parm->tm_lim = 120000;
	// parm->bt_tech = GLP_BT_DFS;
	// parm->bt_tech = GLP_BT_BLB;
	/* MFV chooses the largest {x} (e.g., 0.99 in favor of 0.1)
	 * It would be similar to branch_target=1 for the positive samples,
	 * but the opposite for negative samples */
	// parm->br_tech = GLP_BR_LFV; 
	glp_intopt(p, parm);
	free(parm);

    /*
	int index_max = violation_idx(0, env.samples);
	for (int i = 1; i <= index_max; i++) {
		glp_printf("%s:\t%g\n", 
				glp_get_col_name(p, i), 
				glp_mip_col_val(p, i));
	}*/

	glp_delete_prob(p);
    delete_env(&env);
}
