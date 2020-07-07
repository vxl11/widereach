#include <float.h>
#include <limits.h>

#include "widereach.h"

#define TOLERANCE 1e-2

void node_to_signature(node_signature_t *signature, glp_tree *t, int node) {
    set_signature(signature, glp_ios_node_level(t, node), node);
}

void iselect(glp_tree *t, env_t *env) {
    int best_node = glp_ios_best_node(t);
    double best_bound = glp_ios_node_bound(t, best_node);
    // glp_printf("Best node %i(%g)\t", best_node, best_bound);
    best_bound *= 1. - TOLERANCE;
    node_signature_t best_signature;
    node_to_signature(&best_signature, t, best_node);
    
    node_signature_t signature_current;
    for (int node = glp_ios_next_node(t, 0);
         node != 0;
         node = glp_ios_next_node(t, node)) {
        double bound = glp_ios_node_bound(t, node);
        if (bound >= best_bound) {
            // glp_printf("%i(%g) ", node, bound);
            node_to_signature(&signature_current, t, node);
            if (compare_signature(&best_signature, &signature_current) < 0) {
                copy_signature(&best_signature, &signature_current);
            }
        }
    }
    
    // glp_printf("(%i)\n", best_signature.seqno);
    
	return;
        // glp_ios_select_node(t, best_signature.seqno);
}
