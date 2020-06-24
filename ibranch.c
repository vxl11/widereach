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

void ibranch(glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	double branch_target = env->params->branch_target;

	glp_prob *p = glp_ios_get_prob(t);
	// int w_zero_cnt = wzero(p, samples->dimension);

	double candidate_frac = DBL_MAX;
	int candidate_idx;
	int candidate_sel;
	int candidate_label = 2;
	int sel;
	int positive_cnt = 0;
	int negative_cnt = 0;
	int idx_max = violation_idx(0, samples);
	for (int i = idx_max; i > 0; i--) {
		if (glp_ios_can_branch(t, i)) {
			sample_locator_t *loc = locator(i, samples);
			int class = loc->class;
			free(loc);
			glp_assert(class >= 0);
			int label = samples->label[class];
			if (label > candidate_label) {
				/* Since negatives take precendece over
				 * positives, there is no point continuing
				 * the search */
				break;
			}
			double value = glp_get_col_prim(p, i);
			if (label > 0) {
				positive_cnt++;
				value = 1 - value;
				sel = GLP_UP_BRNCH;
			} else {
				negative_cnt++;
				sel = GLP_DN_BRNCH;
			}
			value = fabs(value - branch_target);
			if (value <= candidate_frac) {
				candidate_frac = value;
				candidate_idx = i;
				candidate_sel = sel;
				candidate_label = label;
			}
		}
	}

	int curr_node = glp_ios_curr_node(t);
	node_data_t *data = 
		(node_data_t *) glp_ios_node_data(t, curr_node);
	data->initialized = 1;

        glp_ios_branch_upon(t, candidate_idx, candidate_sel);
}
