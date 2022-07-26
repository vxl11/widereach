#include "widereach.h"
#include "helper.h"

#define TRY_MODEL(premise, step) \
  TRY(premise, \
      error_handle(state, model, step), \
      NULL)
  
#define MSG_LEN 256
  
int error_handle(int state, GRBmodel *model, char *step) {
  if (!state) {
    return 0;
  }
  
  GRBenv *env = GRBgetenv(model);
  char msg[MSG_LEN];
  snprintf(msg, MSG_LEN, 
           "Error (%s): %i\nError message: %s\n", 
           step, state, GRBgeterrormsg(env));
  GRBmsg(env, msg);
  return state;
}

double *single_gurobi_run(unsigned int *seed, 
                          int tm_lim, 
                          int tm_lim_tune, 
                          env_t *env) {
    /* samples_t *samples = env->samples;
    env->solution_data = solution_data_init(samples_total(samples));
    
    if (seed != NULL) {
        srand48(*seed);
    }*/

    int state;
    GRBmodel *model;
    
    TRY_MODEL(model = gurobi_milp(&state, env), "model creation") 

    TRY_MODEL(
      state = GRBsetdblparam(GRBgetenv(model), 
                             "TuneTimeLimit", 
                             tm_lim_tune / 1000.),
      "set time limit for tuning")
    TRY_MODEL(state = GRBtunemodel(model), "parameter tuning")
    
    
    // Cut generation (TODO warning: just goofing around)
    /*
    GRBsetintparam(GRBgetenv(model), "CoverCuts", 2);
    GRBsetintparam(GRBgetenv(model), "ImpliedCuts", 2);
    GRBsetintparam(GRBgetenv(model), "InfProofCuts", 2); */
    
    printf("optimize ...\n");
    TRY_MODEL(
      state = GRBsetdblparam(GRBgetenv(model), 
                             "TimeLimit", 
                             tm_lim / 1000.),
      "set time limit")
    
    // GRBwrite(model, "tmp.lp");
    
    TRY_MODEL(state = GRBoptimize(model), "optimize")

    int optimstatus;
    TRY_MODEL(
      state = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus), 
      "get optimization status")
    

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
