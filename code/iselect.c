#include <math.h>
#include <float.h>
#include <limits.h>

#include "widereach.h"

#define TOLERANCE 1e-10

double ii_sum_parent(int node, glp_tree *t) {
    glp_assert(node);
    node_data_t *data = parent_data(node, t);
    return data != NULL && data->branch_data.initialized ? 
        data->branch_data.ii_sum : -1.;
}

int direction_to_class(int direction) {
    if (GLP_DN_BRNCH == direction) {
        return 0;
    } else if (GLP_UP_BRNCH == direction) {
        return 1;
    }
    return -1;
}

int is_active(int node, glp_tree *t) {
    for (int i = glp_ios_prev_node(t, 0); i != 0; i = glp_ios_prev_node(t, i)) {
        if (node == i) {
            return 1;
        }
    }
    return 0;
}

int validate_last_branching(glp_tree *t, env_t *env) {
    int *last_branching = &(env->solution_data->branching_node);
    // Checks whether the last branching node has been pruned
    int node_cnt;
    glp_ios_tree_size(t, NULL, &node_cnt, NULL);
    if (*last_branching >= node_cnt) {
        *last_branching = 0;
    }
    return *last_branching;
}

int next_depth_node(int last_branching, glp_tree *t) {
    if (!last_branching) {
        return 0;
    }
    node_data_t *data = (node_data_t *) glp_ios_node_data(t, last_branching);
    child_data_t *child_data = &(data->child_data);
    if (child_data->child_cnt < 2) {
        return 0;
    }
    int next_node = 
        child_data->child[direction_to_class(data->branch_data.direction)];
    return is_active(next_node, t) ? next_node : 0;
}
    

void node_to_signature(node_signature_t *signature, 
                       int node, 
                       glp_tree *t, 
                       env_t *env) {
    samples_t *samples = env->samples;
    set_signature(signature, 
                  glp_ios_node_level(t, node), 
                  is_direction_primary(node, 0, t, samples), 
                  glp_ios_node_bound(t, node),
                  ii_sum_parent(node, t),
                  node,
                  0);
}

void update_parent(int node, glp_tree *t) {
    node_data_t *data = parent_data(node, t);
    if (data != NULL) {
        add_child(&(data->child_data), node);
    }
}

int update_active_nodes(int last_branching, glp_tree *t) {
    int active_children = 0;
    for (int node = glp_ios_next_node(t, 0);
         node != 0;
         node = glp_ios_next_node(t, node)) {
        update_parent(node, t);
        active_children = active_children || 
            (last_branching == glp_ios_up_node(t, node));
    }
    return active_children;
}
    
    

// glpk breaks ties by smallest value of sum of integer infeasibilities
void iselect(glp_tree *t, env_t *env) {
    // return;
    int last_branching = env->solution_data->branching_node;
    if (update_active_nodes(last_branching, t)) {
      // int last_branching = validate_last_branching(t, env);
      glp_printf("Branching %i <- %i\n", 
                 next_depth_node(last_branching, t), last_branching);
    }
    
    int best_node = glp_ios_best_node(t);
    double best_bound = glp_ios_node_bound(t, best_node);
    glp_printf("Best node %i(%g)\t", best_node, best_bound);
    // Bound similar to glpk-4.65 glpios12.c:best_node
    best_bound -= TOLERANCE * (1. + fabs(best_bound));
    node_signature_t best_signature;
    node_to_signature(&best_signature, best_node, t, env); 
    
    // glp_printf("------- iselect ----------- \n");
    node_signature_t signature_current;
    for (int node = glp_ios_next_node(t, 0);
         node != 0;
         node = glp_ios_next_node(t, node)) {
        double bound = glp_ios_node_bound(t, node);
        if (bound >= best_bound) {
            glp_printf("%i(%g", node, bound);
            node_to_signature(&signature_current, node, t, env);
            glp_printf(",%g", signature_current.ii_sum);
            // glp_printf("%i,%i", 
                       //signature_current.level, signature_current.primary); 
            if (compare_signature(&best_signature, &signature_current) < 0) {
                copy_signature(&best_signature, &signature_current);
            }
            glp_printf(") ");
        }
    }
    glp_printf("\n");
    
    // glp_printf("(%i)\n", best_signature.seqno);
    // glp_ios_select_node(t, best_signature.seqno);
}
