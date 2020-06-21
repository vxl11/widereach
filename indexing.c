#include "widereach.h"

/*
 * Translates indexing in a problem instance (env) into GLPK problem (glp_prob).
 *
 * Numbering Conventions
 *
 * Variables (columns)
 * 1 to dimension w
 * dimension+1c
 * dimension+2 to dimension+positives+1 xi
 * dimension+positives+2 to dimension+samples+1	yj
 * dimension+samples+2V
 *
 * Constrains (rows)
 * 1 to positives xi
 * positives+1 to samples yj
 * samples+1V 
 */

int directional_offset(int index, int direction, size_t dimension) {
	int index_shifted = index;
	if (!direction) {
		index_shifted += dimension + 1;
	}
	return index_shifted;
}


int idx(int direction, int class, size_t sample_index, samples_t *samples) { 
	int idx = (int) sample_index + 1;
	idx = directional_offset(idx, direction, samples->dimension);
	if (samples->label[class] < 0) {
		idx += positives(samples);
	}
	return idx;
}


int violation_idx(int direction, samples_t *samples) {
	int idx = samples_total(samples) + 1;
	idx = directional_offset(idx, direction, samples->dimension);
	return idx;
}

int idx_extreme(int direction, int class, int extreme, samples_t *samples) {
	int label = samples->label[class];
	int idx;
	if (label > 0) {
		idx = !extreme ? 1: positives(samples);
	} else {
		idx = !extreme ? 
			positives(samples) + 1 : samples_total(samples);
	}
	idx = directional_offset(idx, direction, samples->dimension);
	return idx;
}


double label_to_penalty(int label, double theta) {
	return label > 0 ? theta - 1. : theta;
}

sparse_vector_t *precision_row(samples_t *samples, double theta) {
	size_t len = samples_total(samples) + 2;
	sparse_vector_t *constraint = sparse_vector_blank(len);
	size_t class_cnt = samples->class_cnt;
	for (size_t class = 0; class < class_cnt; class++) {
		int label = samples->label[class];
		int count = samples->count[class];
		for (size_t i = 0; i < count; i++) {
			append(constraint, 
				idx(0, class, i, samples), 
				label_to_penalty(label, theta));
		}
	}
	append(constraint, violation_idx(0, samples), -1.);
	return constraint;
}
