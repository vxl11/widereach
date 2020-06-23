#include <float.h>

#include "widereach.h"

void iselect(glp_tree *t, env_t *env) {
	int node = glp_ios_next_node(t, 0);
	int node_best = node;
	double distance_min = DBL_MAX;
	// double distance_min = -DBL_MAX;
	for (int node = glp_ios_next_node(t, 0); 
	     node != 0; 
	     node = glp_ios_next_node(t, node)) {

		int parent = glp_ios_up_node(t, node);
		if (!parent) {
			continue;
		}
		node_data_t *data = 
			(node_data_t *) glp_ios_node_data(t, parent);
		if (!data->initialized) {
			continue;
		}

		double distance = data->distance;
		glp_assert(distance > 0.);
		// if (distance >= distance_min) {
		if (distance <= distance_min) {
			distance_min = distance;
			node_best = node;
		}
	}

	/*
	if (drand48() < 1e-3) {
	  glp_printf("best node %i\t%g\n", node_best, distance_min);
	}
	*/
        glp_ios_select_node(t, node_best);
}
