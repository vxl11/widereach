#include <float.h>
#include <limits.h>

#include "widereach.h"

#define TOLERANCE 1e-2

int is_node_primary(int node, glp_tree *t, samples_t *samples) {
    node_data_t *data = (node_data_t *) glp_ios_node_data(t, node);
    glp_printf("(node %i initialized: %i)", node, data->initialized);
    if (data->initialized) {
        return data->primary_direction;
    }
    return is_direction_primary(node, t, samples);
}

void node_to_signature(node_signature_t *signature, 
                       int node, 
                       glp_tree *t, 
                       samples_t *samples) {
    set_signature(signature, 
                  glp_ios_node_level(t, node), 
                  is_node_primary(node, t, samples), 
                  node);
}

void iselect(glp_tree *t, env_t *env) {
    // return;
    
    int best_node = glp_ios_best_node(t);
    double best_bound = glp_ios_node_bound(t, best_node);
    // glp_printf("Best node %i(%g)\t", best_node, best_bound);
    best_bound *= 1. - TOLERANCE;
    node_signature_t best_signature;
    samples_t *samples = env->samples;
    node_to_signature(&best_signature, best_node, t, samples);
    
    glp_printf("------- iselect ----------- \n");
    node_signature_t signature_current;
    for (int node = glp_ios_next_node(t, 0);
         node != 0;
         node = glp_ios_next_node(t, node)) {
        double bound = glp_ios_node_bound(t, node);
        glp_printf("%i(%g,", node, bound);
        if (bound >= best_bound) {
            node_to_signature(&signature_current, node, t, samples);
            glp_printf("%i,%i", 
                       signature_current.level, signature_current.primary);
            if (compare_signature(&best_signature, &signature_current) < 0) {
                copy_signature(&best_signature, &signature_current);
            }
        }
        glp_printf(") ");
    }
    glp_printf("\n");
    
    // glp_printf("(%i)\n", best_signature.seqno);
    glp_ios_select_node(t, best_signature.seqno);
}
