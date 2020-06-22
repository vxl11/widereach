#include <math.h>
#include <float.h>

#include "widereach.h"

void ibranch(glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	int idx_max = violation_idx(0, samples);
	int dimension = samples->dimension;
	int wzero = 0;
	glp_prob *p = glp_ios_get_prob(t);
	for (int i = 1; i <= dimension; i++) {
		if (fabs(glp_get_col_prim(p, i)) < 1e-6) {
			wzero++;
		}
	}
	int candidate_idx;
	double candidate_frac = DBL_MAX;
	int candidate_sel;
	int sel;
	int positive_cnt = 0;
	int negative_cnt = 0;
	for (int i = 1; i <= idx_max; i++) {
		if (glp_ios_can_branch(t, i)) {
			sample_locator_t *loc = locator(i, samples);
			int class = loc->class;
			free(loc);
			glp_assert(class >= 0);
			double value = glp_get_col_prim(p, i);
			if (samples->label[class] > 0) {
				positive_cnt++;
				value = 1 - value;
				sel = GLP_UP_BRNCH;
			} else {
				negative_cnt++;
				sel = GLP_DN_BRNCH;
			}
			if (value < candidate_frac) {
				candidate_frac = value;
				candidate_idx = i;
				candidate_sel = sel;
			}
		}
	}
	// if (wzero == dimension && positive_cnt > 0 && negative_cnt > 0) {
	/*
	if (wzero == dimension) {
	  glp_printf("ibranch: wzero=%i x=%i y=%i\n", 
			wzero, positive_cnt, negative_cnt);
	}
	*/
        glp_ios_branch_upon(t, candidate_idx, candidate_sel);
}
