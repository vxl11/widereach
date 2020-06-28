#include "widereach.h"

void print_branch_variable(int idx, samples_t *samples) {
	sample_locator_t *loc = locator(idx, samples);
	print_sample(*loc, samples);
	free(loc);
}

void traverse(glp_tree *t, env_t *env) {
	int curr_node = glp_ios_curr_node(t);
        node_data_t *data =
                (node_data_t *) glp_ios_node_data(t, curr_node);
	if (data->initialized) {
		samples_t *samples = env->samples;
		glp_printf("%i\t", data->direction);
		print_branch_variable(data->branching_variable, samples);
	}
}
