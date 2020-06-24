#include <math.h>
#include <float.h>

#include "widereach.h"

int wzero(glp_prob *p, int dimension) {
	int w_cnt = 0;
	for (int i = 1; i <= dimension; i++) {
		if (fabs(glp_get_col_prim(p, i)) < 1e-6) {
			w_cnt++;
		}
	}
	return w_cnt;
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
        glp_ios_branch_upon(t, candidate_idx, candidate_sel);
}


int index_label(int i, samples_t *samples) {
	int class = index_to_class(i, samples);
	glp_assert(class >= 0);
	return samples->label[class];
}


/* Return the index with the highest score, where indexes are only
 * considered for variables on which branching is allowed, and are ranked
 * by lower label first, and ties are broken with the absolute difference
 * between variable values and branch target. 
 * The suggested branching direction is stored in the direction argument. */
int highest_score_index(int *direction, glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	double branch_target = env->params->branch_target;

	glp_prob *p = glp_ios_get_prob(t);

	/* Possible diagnostics */
	// int w_zero_cnt = wzero(p, samples->dimension); 
	// int positive_cnt = 0;
	// int negative_cnt = 0;

	double candidate_frac = DBL_MAX;
	int candidate_idx;
	int candidate_sel;
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
			int sel;
			if (label > 0) {
				// positive_cnt++;
				value = 1. - value;
				sel = GLP_UP_BRNCH;
			} else {
				// negative_cnt++;
				sel = GLP_DN_BRNCH;
			}
			value = fabs(value - branch_target);
			if (value <= candidate_frac) {
				candidate_frac = value;
				candidate_idx = i;
				candidate_label = label;
				candidate_sel = sel;
			}
		}
	}
	*direction = candidate_sel;
	return candidate_idx;
}

void ibranch(glp_tree *t, env_t *env) {
	int curr_node = glp_ios_curr_node(t);
	node_data_t *data = 
		(node_data_t *) glp_ios_node_data(t, curr_node);
	data->initialized = 1;

	int direction;
	int idx = highest_score_index(&direction, t, env);

        glp_ios_branch_upon(t, idx, direction); 
}
