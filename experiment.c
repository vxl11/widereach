#include <stdio.h>

#include "widereach.h"

int main() {
	env_t env;
        env.params = params_default();
	env.params->theta = 0.51;
	int n = 1000;
	env.params->lambda = 100 * (n + 1);
	srand48(20200621154912);
        env.samples = random_samples(n, n / 2, 2);

	glp_prob *p = milp(&env);
	// glp_write_lp(p, NULL, "tmp.lp");
	glp_simplex(p, NULL);

	glp_iocp *parm = iocp(&env);
	// parm->bt_tech = GLP_BT_DFS;
	glp_intopt(p, parm);
	free(parm);

	int index_max = violation_idx(0, env.samples);
	for (int i = 1; i <= index_max; i++) {
		glp_printf("%s:\t%g\n", 
				glp_get_col_name(p, i), 
				glp_mip_col_val(p, i));
	}

	glp_delete_prob(p);
        delete_env(&env);
}
