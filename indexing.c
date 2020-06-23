#include "widereach.h"
#include "helper.h"

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


sample_locator_t *locator(int index, samples_t *samples) {
	sample_locator_t *locator = CALLOC(1, sample_locator_t);
	int index_min = samples->dimension + 2;
	if (index < index_min) {
		locator->class = -1;
		return locator;
	}
	size_t offset = (size_t) (index - index_min);
	if (offset >= samples_total(samples)) {
		locator->class = -1;
		return locator;
	}

	size_t threshold = positives(samples);
	if (offset > threshold) {
		locator->class = 0;
		locator->index = offset - threshold;
	} else {
		locator->class = 1;
		locator->index = offset;
	}

	return locator;
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
		double penalty = label_to_penalty(samples->label[class], theta);
		cover_row(constraint, class, penalty, samples);
	}
	append(constraint, violation_idx(0, samples), -1.);
	return constraint;
}

sparse_vector_t *cover_row(
		sparse_vector_t *constraint, 
		size_t class, 
		double coef, 
		samples_t *samples) {
	int count = samples->count[class];
	for (size_t i = 0; i < count; i++) { 
		append(constraint, idx(0, class, i, samples), coef);
	}
	return constraint;
}
