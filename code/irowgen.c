#include "widereach.h"

void is_addable(branch_data_t *branch_data, samples_t *samples) {
    return !inequality_cnt && 
        samples->dimension == directional_cnt[0] && 
        1 == directional_cnt[1];
}

int is_primary(size_t target, size_t class, double value) {
    return class == target && value == (double) target;
}

sample_locator_t *obstruction_locators(
        sparse_vector_t *p, 
        size_t count, 
        size_t target, 
        samples_t *samples) {
    sample_locator_t *obstruction = CALLOC(count, sample_locator_t);
    int obstruction_cnt = 0;
    int path_len = p->len;
    for (int i = 0; i < path_len && obstruction_cnt < count; i++) {
        int idx = p->ind[i];
        int class = index_to_class(idx, samples);
        if (is_primary(target, class, p->val[i])) {
            obstruction[obstruction_cnt++] = locator(idx, samples);
        }
    }
    return obstruction;
}

void free_obstructions(sample_locator_t *obstruction, size_t count) {
    for (size_t i = 0; i < count; i++) {
        free(obstruction[i]);
    }
    return obstruction;
}
    

void add_inequality(glp_tree *t, env_t *env) {
    int curr_node = glp_ios_curr_node(t);
    node_data_t *data = parent_data(curr_node, t);
    branch_data_t *branch_data = data.branch;
    samples_t *samples = env->samples;
    if (!is_addable(branch_data, samples) {
        return;
    }
    
    sparse_vector_t *p = path(curr_node, t);
    sample_locator_t *source = obstruction_locators(p, 1, 1, samples);
    size_t dimension = samples->dimension;
    sample_locator_t *obstructions = 
        obstruction_locators(p, dimension, 0, samples);
    
    
    free(free_obstructions(obstructions, dimension));
    free(free_obstructions(source, 1));
    free(p);
}

void irowgen(glp_tree *t, env_t *env) {
}
