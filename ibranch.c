#include <math.h>
#include <float.h>

#include "widereach.h"
#include "helper.h"

int wzero(glp_prob *p, int dimension) {
	int w_cnt = 0;
	for (int i = 1; i <= dimension; i++) {
		if (fabs(glp_get_col_prim(p, i)) < 1e-6) {
			w_cnt++;
		}
	}
	return w_cnt;
}


int index_label(int i, samples_t *samples) {
	int class = index_to_class(i, samples);
	glp_assert(class >= 0);
	return samples->label[class];
}

int index_direction(int i, samples_t *samples) {
	return index_label(i, samples) > 0 ? GLP_UP_BRNCH : GLP_DN_BRNCH;
}

void branch_on(int index, int direction, glp_tree *t) {
	int curr_node = glp_ios_curr_node(t);
	node_data_t *data = 
		(node_data_t *) glp_ios_node_data(t, curr_node);
	data->initialized = 1;
	data->branching_variable = index;
	data->direction = direction;
        glp_ios_branch_upon(t, index, direction); 
}


int random_eligible(int idx_min, int idx_max, glp_tree *t) {
	int *eligible = CALLOC(idx_max - idx_min + 1, int);
	int eligible_cnt = 0;
	for (int i = idx_min; i <= idx_max; i++) {
		if (glp_ios_can_branch(t, i)) {
			eligible[eligible_cnt++] = i;
		}
	}
	int candidate = 
		eligible_cnt > 0 ? eligible[lrand48() % eligible_cnt] : -1;
	free(eligible);
	return candidate;
}

void random_branch(glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	int dimension = samples->dimension;
	int positive_cnt = positives(samples);
	// Negative candidate
	int candidate = 
		random_eligible(dimension + positive_cnt + 2, 
				dimension + samples_total(samples) + 1, 
				t);
	if (candidate < 0) { 
		// Positive candidate
		candidate = 
			random_eligible(dimension + 2, 
				dimension + positive_cnt + 1, 
				t);
	}

	branch_on(candidate, index_direction(candidate, samples), t);
}


void ibranch_LFV(glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	int candidate_idx;
	int candidate_sel;
	int idx_max = violation_idx(0, samples);
	for (int i = idx_max; i >= 1; i--) {
		if (glp_ios_can_branch(t, i)) {
			candidate_idx = i;
			sample_locator_t *loc = locator(i, samples);
			int class = loc->class;
			free(loc);
			glp_assert(class >= 0);
			candidate_sel = samples->label[class] > 0 ? 
				GLP_UP_BRNCH : GLP_DN_BRNCH;
			break;
		}
	}
	branch_on(candidate_idx, candidate_sel, t);
}



/* Return the index of the decision variable on which branching is allowed
 * and that ranks the first in the solution data. 
 * If no suitable index is found, it returns -1. */
int highest_rank_index(glp_tree *t, env_t *env) {
	solution_data_t *data = env->solution_data;
	size_t rank_significant = data->rank_significant;
	int *rank = data->rank;
	for (size_t i = 0; i < rank_significant; i++) {
		int idx = rank[i];
		if (glp_ios_can_branch(t, idx)) {
			return (int) idx;
		}
	}
	return -1;
}


/* Return the index with the highest score, where indexes are only
 * considered for variables on which branching is allowed, and are ranked
 * by lower label first, and ties are broken with the absolute difference
 * between variable values and branch target. 
 * The suggested branching direction is stored in the direction argument. */
int highest_score_index(glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	double branch_target = env->params->branch_target;

	glp_prob *p = glp_ios_get_prob(t);

	/* Possible diagnostics */
	// int w_zero_cnt = wzero(p, samples->dimension); 
	// int positive_cnt = 0;
	// int negative_cnt = 0;

	double candidate_frac = DBL_MAX;
	int candidate_idx;
	int candidate_label = 2;
	int idx_max = violation_idx(0, samples);
	for (int i = idx_max; i > 0; i--) {
		if (glp_ios_can_branch(t, i)) {
			int label = index_label(i, samples);
			if (label > candidate_label) {
				/* Since negatives take precendece over
				 * positives, there is no point continuing
				 * the search */
				break;
			}
			double value = glp_get_col_prim(p, i);
			if (label > 0) {
				// positive_cnt++;
				value = 1. - value;
			} else {
				// negative_cnt++;
			}
			value = fabs(value - branch_target);
			if (value <= candidate_frac) {
				candidate_frac = value;
				candidate_idx = i;
				candidate_label = label;
			}
		}
	}
	return candidate_idx;
}

void ibranch(glp_tree *t, env_t *env) {
	/*
	ibranch_LFV(t, env);
	*/
	random_branch(t, env);
	return;

	/* Choice of branching index: try high rank first, and if that
	 * fails move one to high score (which then becomes the next ranked
	 * index) */
	int idx = highest_rank_index(t, env);
	if (idx < 0) { 
		idx = highest_score_index(t, env);
		append_data(env->solution_data, idx);
	}
	glp_assert(idx > 0);

	branch_on(idx, index_direction(idx, env->samples), t);
}
