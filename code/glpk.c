/*
 * Convert a problem instance (env) into GLPK problem (glp_prob).
 *
 * Numbering Conventions
 *
 * Variables (columns)
 * 1 to dimension	w
 * dimension+1	c
 * dimension+2 to dimension+positives+1 xi
 * dimension+positives+2 to dimension+samples+1	yj
 * dimension+samples+2	V
 *
 * Constrains (rows)
 * 1 to positives	xi
 * positives+1 to samples	yj
 * samples+1	V 
 */

#include <stdio.h>

#include "widereach.h"
#include "helper.h"

#define NAME_LEN_MAX 255

glp_iocp *iocp(const env_t *env)  {
	params_t *params = env->params;
	glp_iocp *parm = CALLOC(1, glp_iocp);
	glp_init_iocp(parm);
	parm->msg_lev = params->verbosity;
	parm->pp_tech = GLP_PP_NONE;
	parm->sr_heur = GLP_OFF;
	parm->binarize= GLP_ON;
	parm->cb_func = callback;
	parm->cb_info = (void *) env;
	parm->cb_size = sizeof(node_data_t);
	return parm;
}


glp_prob *init_prob(const env_t *env) {
	glp_prob *p = glp_create_prob();
	params_t *params = env->params;
	glp_set_prob_name(p, params->name);
	glp_set_obj_dir(p, GLP_MAX);
	samples_t *samples = env->samples;
	glp_add_cols(p, violation_idx(0, samples));
	glp_add_rows(p, violation_idx(1, samples));
	return p;
}


glp_prob *add_hyperplane(glp_prob *p, const env_t *env) {
	char name[NAME_LEN_MAX];
	int hyperplane_cnt = env->samples->dimension + 1;
	for (int i = 1; i <= hyperplane_cnt; i++) {
		glp_set_col_kind(p, i, GLP_CV);
		glp_set_col_bnds(p, i, GLP_FR, 0., 0.);
	        snprintf(name, NAME_LEN_MAX, "w%u", i);
		glp_set_col_name(p, i, name);
	}
	glp_set_col_name(p, hyperplane_cnt, "c");
	return p;
}

char label_to_varname(int label) {
	return label > 0 ? 'x' : 'y';
}

double label_to_obj(int label) {
	return label > 0 ? 1. : 0.;
}

double label_to_bound(int label, params_t *params) {
	return label > 0 ? 
		1. - params->epsilon_positive : 
		-params->epsilon_negative;
}

void add_sample(glp_prob *p, sample_locator_t locator, const env_t *env) {
	samples_t *samples = env->samples;
	size_t class = locator.class;
	int label = samples->label[class];
	size_t sample_index = locator.index;
	char name[NAME_LEN_MAX];
	snprintf(name, NAME_LEN_MAX, "%c%u", 
			label_to_varname(label), 
			(unsigned int) sample_index + 1); 

	// Column
	int col_idx = idx(0, class, sample_index, samples);
	glp_set_col_name(p, col_idx, name);
	glp_set_col_kind(p, col_idx, GLP_BV);
	glp_set_obj_coef(p, col_idx, label_to_obj(label));

	// Row
	int row_idx = idx(1, class, sample_index, samples);
	glp_set_row_name(p, row_idx, name);
	params_t *params = env->params;
	glp_set_row_bnds(p, row_idx, GLP_UP, 0., label_to_bound(label, params));

	int dimension = samples->dimension;
	// Set coefficients of w
	sparse_vector_t *v = to_sparse(dimension, 
			samples->samples[class][sample_index], 2); 
	// Set coefficient of c
	append(v, dimension + 1, -1.); 
	// Change sign depending on sample class
	multiply(v, -label);
	// Add sample decision variable
	append(v, col_idx, label); 
	glp_set_mat_row(p, row_idx, v->len, v->ind, v->val);

	free(delete_sparse_vector(v));
}

glp_prob *add_samples(glp_prob *p, const env_t *env) {
	samples_t *samples = env->samples;
	for (size_t class = 0; class < samples->class_cnt; class++) {
		int cnt = samples->count[class];
		for (size_t idx = 0; idx < cnt; idx++) {
			sample_locator_t locator = { class, idx };
			add_sample(p, locator , env);
		}
	}

	return p;
}


void set_rhs(glp_prob *p, int row_idx, sparse_vector_t *constraint) {
	glp_set_mat_row(p, row_idx, 
			constraint->len, constraint->ind, constraint->val);
}


glp_prob *add_precision(glp_prob *p, const env_t *env) {
	samples_t *samples = env->samples;
	int col_idx = violation_idx(0, samples);
	glp_set_col_name(p, col_idx, "V");
	glp_set_col_kind(p, col_idx, GLP_CV);
	params_t *params = env->params;
	glp_set_col_bnds(p, col_idx, params->violation_type, 0., 0.);
	glp_set_obj_coef(p, col_idx, -params->lambda);

	int row_idx = violation_idx(1, samples);
	glp_set_row_name(p, row_idx, "V");
	double theta = params->theta;
	glp_set_row_bnds(p, row_idx, GLP_UP, 0., 
			-theta * params->epsilon_precision);
	sparse_vector_t *constraint = precision_row(samples, theta);
	set_rhs(p, row_idx, constraint);
	free(delete_sparse_vector(constraint));

	return p;
}


glp_prob *add_valid_constraints(glp_prob *p, const env_t *env) {
	glp_add_rows(p, 2);

	// Positive cover
	samples_t *samples = env->samples;
	int row_idx = violation_idx(1, samples) + 1;
	glp_set_row_name(p, row_idx, "X");
	glp_set_row_bnds(p, row_idx, GLP_LO, 1., 1.);
	sparse_vector_t *constraint = sparse_vector_blank(positives(samples));
	cover_row(constraint, 1, 1., samples);
	set_rhs(p, row_idx, constraint);
	free(constraint);

	// Negative cover
	row_idx++;
	glp_set_row_name(p, row_idx, "Y");
	int negative_cnt = negatives(samples);
	glp_set_row_bnds(p, row_idx, GLP_UP, 0., negative_cnt - 1);
	constraint = sparse_vector_blank(negatives(samples));
	cover_row(constraint, 0, 1., samples);
	set_rhs(p, row_idx, constraint);
	free(constraint);

	return p;
}


glp_prob *milp(const env_t *env) {
	if (!is_binary(env->samples)) {
		return NULL;
	}
	glp_prob *p = init_prob(env);
	p = add_hyperplane(p, env);
	p = add_samples(p, env);
	p = add_precision(p, env);
 	// p = add_valid_constraints(p, env);
	return p;
}


int is_direction_primary(
        int branching_variable, 
        glp_tree *t, 
        samples_t *samples) {
    return glp_get_col_prim(glp_ios_get_prob(t), branching_variable) == 
        primary_value(index_label(branching_variable, samples));
}


void initialize_count(int *cnt, int *parent_cnt) {
    cnt[0] = parent_cnt[0];
    cnt[1] = parent_cnt[1];
}


node_data_t *initialize_data(int node, glp_tree *t, samples_t *samples) {
    // Find and initialize node data
    node_data_t *data = 
        (node_data_t *) glp_ios_node_data(t, node);
    if (data->initialized) {
        return data;
    }
    branch_data_t *branch_data = &(data->branch_data);
    
    // Copy parent data
    int parent = glp_ios_up_node(t, node);
    if (!parent) {
        data->primary_direction = 1;
        data->initialized = 1;
        return data;
    }
    node_data_t *data_parent = (node_data_t *) glp_ios_node_data(t, parent);
    branch_data_t *branch_data_parent = &(data_parent->branch_data);
    initialize_count(branch_data->class_cnt, branch_data_parent->class_cnt);
    
    // Update counts
    initialize_count(data->directional_cnt, data_parent->directional_cnt);
    int branching_variable = branch_data_parent->branching_variable;
    int primary = is_direction_primary(branching_variable, t, samples);
    data->primary_direction = primary;
    data->directional_cnt[index_to_class(branching_variable, samples)] +=
        primary;
        
    data->initialized = 1;
    return data;
}
