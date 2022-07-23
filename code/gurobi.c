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

int add_gurobi_hyperplane(GRBmodel *model, size_t dimension) {
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
  
  return GRBaddvars(model, hyperplane_cnt, 
                          0, NULL, NULL, NULL, 
                          NULL, lb, NULL, 
                          NULL, 
                          varnames);
}


int add_gurobi_sample_var(GRBmodel *model, int label, char *name) {
  int state = GRBaddvar(model, 0, NULL, NULL, 
                        label_to_obj(label), 
                        0., 1., GRB_BINARY, 
                        name);
  if (state != 0) {
    return state;
  }
  return GRBupdatemodel(model);
}

// Convert index set from GLPK to Gurobi format
void gurobi_indices(sparse_vector_t *v) {
  int vlen = v->len;
  for (int i = 1; i <= vlen; i++) {
    v->ind[i]--;
  }
}

int add_gurobi_sample_constr(
    GRBmodel *model, 
    sample_locator_t locator,
    int label, 
    char *name,
    const env_t *env) {
  // Set coefficients of w
  samples_t *samples = env->samples;
  int dimension = (int) samples->dimension;
  size_t class = locator.class;
  size_t sample_index = locator.index;
  sparse_vector_t *v = 
    to_sparse(dimension, samples->samples[class][sample_index], 2);
  // Set coefficient of c
  append(v, dimension + 1, -1.); 
  // Change sign depending on sample class
  multiply(v, -label);
  // Add sample decision variable
  int col_idx = idx(0, class, sample_index, samples);
  append(v, col_idx, label);
  
  gurobi_indices(v);
  
  return GRBaddconstr(model,
                      v->len, v->ind+1, v->val+1, 
                      GRB_LESS_EQUAL, label_to_bound(label, env->params),
                      name);
}


int add_gurobi_sample(GRBmodel *model, 
                      sample_locator_t locator, 
                      const env_t *env) {
  int label = env->samples->label[locator.class];
  char name[NAME_LEN_MAX];
  snprintf(name, NAME_LEN_MAX, "%c%u", 
          label_to_varname(label), 
          (unsigned int) locator.index + 1);
  
  // Add sample decision variable
  int state = add_gurobi_sample_var(model, label, name);
  if (state != 0) {
    return state;
  }
  
  // Add sample constraint
  return add_gurobi_sample_constr(model, locator, label, name, env);
}

int gurobi_accumulator(
    samples_t *samples, 
    sample_locator_t locator, 
    void *model, 
    void *env) {
  return add_gurobi_sample((GRBmodel *) model, locator, (const env_t *) env);
}

int add_gurobi_samples(GRBmodel *model, const env_t *env) {
  return reduce(env->samples, (void *) model, gurobi_accumulator, (void *) env); 
}


GRBmodel *gurobi_milp(const env_t *env) {
    samples_t *samples = env->samples;
	if (!is_binary(samples)) {
		return NULL;
	}
	GRBmodel *model = init_gurobi_model(env); 
	add_gurobi_hyperplane(model, samples->dimension);
    /* 
	p = add_gurobi_samples(p, env);
	p = add_precision(p, env); */
 	// p = add_valid_constraints(p, env);
	return model;
}
