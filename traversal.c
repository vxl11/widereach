#include "widereach.h"

void print_branch_variable(int idx, samples_t *samples) {
	sample_locator_t *loc = locator(idx, samples);
	print_sample(*loc, samples);
	free(loc);
}

void traverse(glp_tree *t, env_t *env) {
	glp_printf("------- Traversal -------\n");
        node_data_t *data;
	for (int curr_node = glp_ios_curr_node(t);
			curr_node != 0;
			curr_node = glp_ios_up_node(t, curr_node)) {
        	data = (node_data_t *) glp_ios_node_data(t, curr_node);
		if (data->initialized) {
			samples_t *samples = env->samples;
			glp_printf("%i(%i)\t", 
					data->branching_variable,
					data->direction);
			print_branch_variable(data->branching_variable, 
					samples);
		}
	}
}
