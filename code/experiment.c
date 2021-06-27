#include <stdio.h>

#include "widereach.h"
#include "helper.h"

#define SAMPLE_SEEDS 3
unsigned long int samples_seeds[SAMPLE_SEEDS] = {
    85287339, // 412
    20200621154912, // 378
    20200623170005 // 433
};

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
    glp_scale_prob(p, GLP_SF_AUTO);
    glp_simplex(p, NULL);

    glp_iocp *parm = iocp(env);
    parm->tm_lim = 120000;
    // parm->tm_lim = 600000;
    // parm->tm_lim = 1800000;
    parm->bt_tech = GLP_BT_DFS;
    // parm->bt_tech = GLP_BT_BLB;
    /* MFV chooses the largest {x} (e.g., 0.99 in favor of 0.1)
    * It would be similar to branch_target=1 for the positive samples,
    * but the opposite for negative samples */
    // parm->br_tech = GLP_BR_LFV;
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

// Compute 10^d, where d is even or d=1, 3
int pow10quick(int d) {
  if (!d) {
    return 1;
  }
  if (d % 2) {
    return 10 * pow10quick(d - 1);
  }
  int partial = pow10quick(d / 2);
  return partial * partial;
}

int main() {
    env_t env;
    env.params = params_default();
    // env.params->theta = 0.99;
    env.params->theta = 0.4;
    env.params->branch_target = 0.0;
    env.params->iheur_method = deep;
    // int n = 400;
    // env.params->lambda = 100 * (n + 1); 
    env.params->rnd_trials = 10000;
    // env.params->rnd_trials_cont = 10;
    env.params->rnd_trials_cont = 0;
    
    size_t dimension = 2;
    clusters_info_t clusters[2];
    int n = pow10quick(dimension);
    clusters_info_singleton(clusters, n * .8, dimension);
    clusters_info_t *info = clusters + 1;
    info->dimension = dimension;
    size_t cluster_cnt = info->cluster_cnt = 2;
    info->count = CALLOC(cluster_cnt, size_t); 
    info->shift = CALLOC(cluster_cnt, double);
    info->side = CALLOC(cluster_cnt, double);
    info->shift[0] = 0.;
    info->side[0] = 1.;
    info->shift[1] = .9;
    info->side[1] = .1;
    info->count[0] = info->count[1] = n / 10;
    
    // for (int s = 0; s < SAMPLE_SEEDS; s++) {
    for (int s = 0; s < 1; s++) {
        srand48(samples_seeds[s]);
    
        samples_t *samples;
        
        // samples = random_samples(n, n / 2, dimension);
        samples = random_sample_clusters(clusters);
        // FILE *infile =
            // fopen("../../data/breast-cancer/wdbc.dat", "r");
            // fopen("../../data/wine-quality/winequality-red.dat", "r");
            // fopen("../../data/wine-quality/winequality-white.dat", "r"); 
            // fopen("../../data/south-german-credit/SouthGermanCredit.dat", "r");
            // fopen("../../data/cross-sell/train-nocat.dat", "r"); */
            // fopen("../../data/crops/sample.dat", "r");
            // fopen("../../data/crops/small-sample.dat", "r");
        // samples_t *samples = read_binary_samples(infile);
        // fclose(infile);
        
        env.samples = samples;
        n = samples_total(samples);
        env.params->lambda = 10 * (n + 1);
        /*
        print_samples(env.samples);
        return 0; */
        
        // for (int t = 0; t <= MIP_SEEDS; t++) {    
        for (int t = 0; t < 1; t++) {
            single_run(t, &env);
        }
    }
    
    delete_clusters_info(clusters);
    delete_clusters_info(clusters + 1);
}
