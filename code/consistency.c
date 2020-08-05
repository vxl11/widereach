#include "widereach.h"

#define PROBLEM_NAME "consistency"

glp_prob *init_consistency_problem(size_t dimension) {
    glp_prob *p = glp_create_prob();
	glp_set_prob_name(p, PROBLEM_NAME);
	glp_set_obj_dir(p, GLP_MIN);
    int hyperplane_cnt = 1 + (int) dimension;
	glp_add_cols(p, hyperplane_cnt);
    add_hyperplane(p, dimension);
    glp_set_obj_coef(p, hyperplane_cnt, 1.);
    return p;
}

glp_prob *append_sample(glp_prob *p, sample_locator_t *loc, env_t *env) {
    // Copy parameters to local variables
    samples_t *samples = env->samples;
    size_t dimension = samples->dimension;
    int sample_class = loc->class;
    
    // Set coefficients of w
    sparse_vector_t *v = 
        to_sparse(dimension, samples->samples[sample_class][loc->index], 1);
    // Set coefficient of c
	append(v, dimension + 1, -1.);
    
    // Add the right hand side
    int row_idx = glp_add_rows(p, 1);
    glp_set_mat_row(p, row_idx, v->len, v->ind, v->val);
    
    // Add the left hand side
    params_t *params = env->params;
    if (sample_class) {
        glp_set_row_bnds(p, row_idx, GLP_LO, params->epsilon_positive, 0.);
    } else {
        glp_set_row_bnds(p, row_idx, GLP_UP, 0, -params->epsilon_negative);
    }
    
    return p;
}

glp_prob *remove_last_sample(glp_prob *p) {
    int num[2];
    num[1] = glp_get_num_rows(p);
    glp_del_rows(p, 1, num);
    return p;
}

int is_consistent_with(glp_prob *p, sample_locator_t *loc, env_t *env) {
    glp_smcp parm;
    glp_init_smcp(&parm);
    parm.msg_lev = GLP_MSG_OFF;
    
    int status = glp_simplex(append_sample(p, loc, env), &parm);
    int solvable = !status && glp_get_status(p) == GLP_OPT;
    remove_last_sample(p);
    
    return solvable;
}
