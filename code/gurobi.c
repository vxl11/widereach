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

GRBmodel *model_conditional(int state, GRBmodel *model) {
  return state ? NULL : model;
}


GRBmodel *add_gurobi_hyperplane(GRBmodel *model, size_t dimension) {
  int dimension_int = (int) dimension;
  int hyperplane_cnt = 1 + dimension_int;
  double *lb = CALLOC(hyperplane_cnt, double);
  char **varnames = CALLOC(hyperplane_cnt, char *);
  char *name;
	
  for (int i = 0; i < hyperplane_cnt; i++) {
    lb[i] = -GRB_INFINITY;
    name = varnames[i] = CALLOC(NAME_LEN_MAX, char);
    snprintf(name, NAME_LEN_MAX, "w%u", i + 1);
  }
  snprintf(varnames[dimension_int], NAME_LEN_MAX, "c");
  
  int state =  GRBaddvars(model, hyperplane_cnt, 
                          0, NULL, NULL, NULL, 
                          NULL, lb, NULL, 
                          NULL, 
                          varnames);
  return model_conditional(state, model);
}


GRBmodel *add_gurobi_sample_var(GRBmodel *model, int label, char *name) {
  int state = GRBaddvar(model, 0, NULL, NULL, 
                        label_to_obj(label), 
                        0., 1., GRB_BINARY, 
                        name);
  return model_conditional(state, model);
}

GRBmodel *add_gurobi_sample_constr(GRBmodel *model, int label, char *name) {
  return NULL;
}


GRBmodel *add_gurobi_sample(GRBmodel *model, 
                            sample_locator_t locator, 
                            const env_t *env) {
  samples_t *samples = env->samples;
  size_t class = locator.class;
  int label = samples->label[class];
  size_t sample_index = locator.index;
  char name[NAME_LEN_MAX];
  snprintf(name, NAME_LEN_MAX, "%c%u", 
          label_to_varname(label), 
          (unsigned int) sample_index + 1);
  
  if (NULL == add_gurobi_sample_var(model, label, name)) {
    return NULL;
  }
  return model;
  
  int dimension = samples->dimension;
  int numnz = dimension + 2;
  int *cind = CALLOC(numnz, int);
  double *cval = CALLOC(numnz, double);
  
  // Set coefficient of c
  cind[0] = 0;
  cval[0] = -1.;
  
  // Set coefficients of w
  for (int i = 1; i < numnz; i++) {
    cind[i] = i;
    cval[i] = samples->samples[class][sample_index][i-1];
  }
  
  // Change sign depending on sample class
  for (int i = 0; i < numnz; i++) {
    cval[i] *= -label;
  }

  // Add sample decision variable
  int sample_ptr = numnz - 1;
  cind[sample_ptr] = idx(0, class, sample_index, samples);
  cval[sample_ptr] = label; 
  
  int state = GRBaddconstr(model,
                           numnz, cind, cval, 
                           GRB_LESS_EQUAL, label_to_bound(label, env->params),
                           name);
  return model_conditional(state, model);
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
