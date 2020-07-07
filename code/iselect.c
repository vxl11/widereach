#include <float.h>

#include "widereach.h"



void iselect(glp_tree *t, env_t *env) {
    int best_node = glp_ios_best_node(t);
    glp_printf("Best node %i\t", best_node);
    glp_assert(best_node != 0);
    double best_bound = glp_ios_node_bound(t, best_node);
    for (int node = glp_ios_next_node(t, 0);
         node != 0;
         node = glp_ios_next_node(t, node)) {
        double bound = glp_ios_node_bound(t, best_node);
        glp_assert(bound <= best_bound);
        if (bound == best_bound) {
            glp_printf("%i ", node);
        }
    }
    glp_printf("\n");
	return;
        // glp_ios_select_node(t, node_best);
}
