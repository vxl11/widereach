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
