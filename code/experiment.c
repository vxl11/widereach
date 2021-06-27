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
    env.params->theta = 0.15;
    env.params->branch_target = 0.0;
    env.params->iheur_method = deep;
    int n = 400;
    // env.params->lambda = 100 * (n + 1); 
    env.params->rnd_trials = 10000;
    // env.params->rnd_trials_cont = 10;
    env.params->rnd_trials_cont = 0;
    
    size_t dimension = 3;
    clusters_info_t clusters[2];
    // int n = pow10quick(dimension);
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
        // samples = random_sample_clusters(clusters);
        FILE *infile =
            // fopen("../../data/breast-cancer/wdbc.dat", "r");
            fopen("../../data/wine-quality/winequality-red.dat", "r");
            // fopen("../../data/wine-quality/winequality-white.dat", "r"); 
            // fopen("../../data/south-german-credit/SouthGermanCredit.dat", "r");
            // fopen("../../data/cross-sell/train-nocat.dat", "r"); */
            // fopen("../../data/crops/sample.dat", "r");
            // fopen("../../data/crops/small-sample.dat", "r");
        samples = read_binary_samples(infile);
        fclose(infile);
        
        env.samples = samples;
        n = samples_total(samples);
        env.params->lambda = 10 * (n + 1);
        
        /*print_samples(env.samples);
        return 0; */ 
        
        // for (int t = 0; t <= MIP_SEEDS; t++) {    
        for (int t = 0; t < 1; t++) {
            single_run(t ? mip_seeds + (t - 1) : NULL, 120000, &env);
        }
    }
    
    delete_clusters_info(clusters);
    delete_clusters_info(clusters + 1);
}
