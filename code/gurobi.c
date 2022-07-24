#include "widereach.h"
#include "helper.h"

#define NAME_LEN_MAX 255

#define TRY_MODEL(condition) TRY(, condition, NULL)
#define TRY_STATE(body) TRY(int state = body, state != 0, state)
#define TRY_MODEL_ON(premise) TRY(*state = premise, *state != 0, NULL)

GRBmodel *init_gurobi_model(int *state, const env_t *env) {
  GRBenv *p = NULL;
  
  TRY_MODEL_ON(GRBemptyenv(&p));
  
  TRY_MODEL_ON(GRBstartenv(p));
  
  params_t *params = env->params;
  GRBmodel *model = NULL;
  TRY_MODEL_ON(
    GRBnewmodel(p, &model, params->name, 0, NULL, NULL, NULL, NULL, NULL));
  
  TRY_MODEL_ON(
    GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE));

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
  TRY_STATE(
    GRBaddvar(model, 0, NULL, NULL, 
              label_to_obj(label), 
              0., 1., GRB_BINARY, 
              name));
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
  TRY_STATE(add_gurobi_sample_var(model, label, name));
  
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

int add_gurobi_precision(GRBmodel *model, const env_t *env) {
  params_t *params = env->params;
  TRY_STATE(
    GRBaddvar(model, 0, NULL, NULL, 
              -params->lambda, 
              params->violation_type ? 0. : -GRB_INFINITY, 
              GRB_INFINITY,
              GRB_CONTINUOUS, "V"));
  
  double theta = params->theta;
  sparse_vector_t *constraint = precision_row(env->samples, theta);
  gurobi_indices(constraint);
  return GRBaddconstr(model, 
                      constraint->len, constraint->ind+1, constraint->val+1, 
                      GRB_LESS_EQUAL, -theta * params->epsilon_precision, 
                      "V");
}

GRBmodel *gurobi_milp(int *state, const env_t *env) {
    samples_t *samples = env->samples;
	TRY_MODEL(!is_binary(samples))
	
	GRBmodel *model = init_gurobi_model(state, env); 
    TRY_MODEL(NULL == model)
	
    TRY_MODEL_ON(add_gurobi_hyperplane(model, samples->dimension))
    
	TRY_MODEL_ON(add_gurobi_samples(model, env))
    
    TRY_MODEL_ON(add_gurobi_precision(model, env))
    
 	// p = add_valid_constraints(p, env);
	return model;
}
