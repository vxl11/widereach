#include "widereach.h"
#include "helper.h"

#define NAME_LEN_MAX 255


GRBmodel *init_gurobi_model(const env_t *env) {
  GRBenv *p = NULL;
  if (GRBemptyenv(&p)) {
      return NULL;
  }
  
  if (GRBstartenv(p)) {
    return NULL;
  }
  
  params_t *params = env->params;
  GRBmodel *model = NULL;
  if (GRBnewmodel(p, &model, params->name, 0, NULL, NULL, NULL, NULL, NULL)) {
    return NULL;
  }
  
  if (GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE)) {
    return NULL;
  }

  return model; 
}

GRBmodel *add_gurobi_hyperplane(GRBmodel *model, size_t dimension) {
  int hyperplane_cnt = 1 + (int) dimension;
  int ary_size = hyperplane_cnt + 1;
  double *lb = CALLOC(ary_size, double);
  char **varnames = CALLOC(ary_size, char *);
  char *name;
	
  for (int i = 0; i <= hyperplane_cnt; i++) {
    lb[i] = -GRB_INFINITY;
    name = varnames[i] = CALLOC(NAME_LEN_MAX, char);
    snprintf(name, NAME_LEN_MAX, "w%u", i);
  }
  snprintf(varnames[0], NAME_LEN_MAX, "c");
  
  int state =  GRBaddvars(model, hyperplane_cnt, 
                          0, NULL, NULL, NULL, 
                          NULL, lb, NULL, 
                          NULL, 
                          varnames);
  return state ? NULL : model;
}


GRBmodel *gurobi_milp(const env_t *env) {
    samples_t *samples = env->samples;
	if (!is_binary(samples)) {
		return NULL;
	}
	return init_gurobi_model(env); // TODO
	/*
	p = add_hyperplane(p, samples->dimension);
	p = add_samples(p, env);
	p = add_precision(p, env);
 	// p = add_valid_constraints(p, env);
	return p;*/
}
