#include "widereach.h"
#include "helper.h"

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
