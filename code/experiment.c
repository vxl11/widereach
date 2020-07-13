#include <stdio.h>

#include "widereach.h"

#define MIP_SEEDS 30
unsigned int mip_seeds[MIP_SEEDS] = {
    734517477, 145943044, 869199209, 499223379, 523437323, 964156444,
    248689460, 115706114, 711104006, 311906069, 205328448, 471055100,
    307531192, 543901355, 24851720, 704008414, 2921762, 181094221,
    234474543, 782516264, 519948660, 115033019, 205486123, 657145193,
    83898336, 41744843, 153111583, 318522606, 952537249, 298531860
};

void single_run(int t, env_t *env) {
    samples_t *samples = env->samples;
    env->solution_data = solution_data_init(samples_total(samples));
        
    if (t > 0) {
        srand48(mip_seeds[t-1]);
    }

    glp_prob *p = milp(env);
    // glp_write_lp(p, NULL, "tmp.lp");
    glp_simplex(p, NULL);

    glp_iocp *parm = iocp(env);
    parm->tm_lim = 120000;
    // parm->tm_lim = 10000;
    // parm->bt_tech = GLP_BT_DFS;
    parm->bt_tech = GLP_BT_BLB;
    /* MFV chooses the largest {x} (e.g., 0.99 in favor of 0.1)
    * It would be similar to branch_target=1 for the positive samples,
    * but the opposite for negative samples */
    parm->br_tech = GLP_BR_LFV;
    glp_intopt(p, parm);
    free(parm);

    glp_printf("Objective: %g\n", glp_mip_obj_val(p));
    /*
    int index_max = violation_idx(0, env.samples);
    for (int i = 1; i <= index_max; i++) {
        glp_printf("%s:\t%g\n", glp_get_col_name(p, i), glp_mip_col_val(p, i));
    }*/

    glp_delete_prob(p);
    free(delete_solution_data(env->solution_data));
}

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
    
    samples_t *samples = random_samples(n, n / 2, 2);
    // print_samples(env.samples);
    env.samples = samples;
    
    for (int t = 0; t <= MIP_SEEDS; t++) {    
    // for (int t = 0; t < 1; t++) {
        single_run(t, &env);
    }
}
