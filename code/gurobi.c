#include "widereach.h"
#include "helper.h"

glp_prob *init_gurobi_env(const env_t *env) {
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
  
  samples_t *samples = env->samples;
  
  /* HERE
	glp_add_cols(p, violation_idx(0, samples));
	glp_add_rows(p, violation_idx(1, samples));
	return p; */
  return NULL;
}

GRBenv *gurobi_milp(const env_t *env) {
    samples_t *samples = env->samples;
	if (!is_binary(samples)) {
		return NULL;
	}
	return NULL; // TODO
	/*
	glp_prob *p = init_prob(env);
	p = add_hyperplane(p, samples->dimension);
	p = add_samples(p, env);
	p = add_precision(p, env);
 	// p = add_valid_constraints(p, env);
	return p;*/
}
