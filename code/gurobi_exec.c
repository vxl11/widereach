#include "widereach.h"
#include "helper.h"

int error_handle(int state, GRBmodel *model, char *step) {
  if (!state) {
    return 0;
  }
  fprintf(stderr, "Gurobi error (%s): %i\n", step, state);
  fprintf(stderr, "Error message: %s\n", GRBgeterrormsg(GRBgetenv(model)));
  return state;
}

double *single_gurobi_run(unsigned int *seed, int tm_lim, env_t *env) {
    samples_t *samples = env->samples;
    env->solution_data = solution_data_init(samples_total(samples));
    
    if (seed != NULL) {
        srand48(*seed);
    }

    int state;
    GRBmodel *model;
    TRY(
      model = gurobi_milp(&state, env), 
      error_handle(state, model, "model creation"), 
      NULL)
    
    TRY(
      state = GRBoptimize(model), 
      error_handle(state, model, "optimize"), 
      NULL)

    // GRBwrite(model, "tmp.lp");
    /*
    // glp_scale_prob(p, GLP_SF_AUTO);
    glp_simplex(p, NULL);

    glp_iocp *parm = iocp(env);
    parm->tm_lim = tm_lim;
    parm->bt_tech = GLP_BT_DFS;
    // parm->bt_tech = GLP_BT_BLB;
    MFV chooses the largest {x} (e.g., 0.99 in favor of 0.1)
    * It would be similar to branch_target=1 for the positive samples,
    * but the opposite for negative samples 
    // parm->br_tech = GLP_BR_LFV;
    glp_intopt(p, parm);
    free(parm);

    double *result = solution_values_mip(p);
     size_t dimension = samples->dimension;
    double *result = CALLOC(dimension + 2, double);
    double *h = hyperplane(p, samples);
    copy_hyperplane(dimension, result, h);
    free(h); 
    double obj = result[0] = glp_mip_obj_val(p);
    glp_printf("Objective: %g\n", obj);
    // result[dimension + 1] = obj;
    
    int index_max = violation_idx(0, env.samples);
    for (int i = 1; i <= index_max; i++) {
        glp_printf("%s:\t%g\n", glp_get_col_name(p, i), glp_mip_col_val(p, i));
    }

    glp_delete_prob(p);
    free(delete_solution_data(env->solution_data));
    
    // return result; */
    return NULL;
}
