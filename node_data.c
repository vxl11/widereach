#include <math.h>
#include <float.h>

#include "widereach.h"

node_data_t *set_distance(node_data_t *data, glp_tree *t, samples_t *samples) {
	glp_prob *p = glp_ios_get_prob(t);
	int closest_index = -1;
	double closest_distance = DBL_MAX;
	int idx_max = violation_idx(0, samples);
	for (int i = samples->dimension + 2; i < idx_max; i++) { 
		double value = glp_get_col_prim(p, i);
		if (glp_ios_can_branch(t, i)) {
			glp_assert(round(value) != value);
			// Fractional variable
			sample_locator_t *loc = locator(i, samples);
			int class = loc->class;
			free(loc);
			glp_assert(class >= 0);
			if (samples->label[class] > 0) {
				value = 1 - value;
			}
			if (value < closest_distance) {
				closest_distance = value;
				closest_index = i;
			}
		}
	}
	data->distance = closest_distance;
	data->index_closest = closest_index;

	return data;
}

void initialize_node_data(glp_tree *t, env_t *env) {
	node_data_t *data = 
		(node_data_t *) glp_ios_node_data(t, glp_ios_curr_node(t));
	if (data->initialized) {
		return;
	}
	set_distance(data, t, env->samples);

	data->initialized = 1;
}
