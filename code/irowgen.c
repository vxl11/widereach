#include "widereach.h"
#include "helper.h"

int is_addable(branch_data_t *branch_data, samples_t *samples) {
    int *directional_cnt = branch_data->directional_cnt;
    return !branch_data->inequality_cnt && 
           samples->dimension == directional_cnt[0] && 
           1 == directional_cnt[1];
}

int is_primary(size_t target, size_t class, double value) {
    return class == target && value == (double) target;
}

sample_locator_t **obstruction_locators(
        sparse_vector_t *p, 
        size_t count, 
        size_t target, 
        samples_t *samples) {
    sample_locator_t **obstruction = CALLOC(count, sample_locator_t *);
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

sample_locator_t **free_obstructions(
            sample_locator_t **obstruction, size_t count) {
    for (size_t i = 0; i < count; i++) {
        free(obstruction[i]);
    }
    return obstruction;
}
    

void add_inequality(glp_tree *t, env_t *env) {
    int curr_node = glp_ios_curr_node(t);
    node_data_t *data = parent_data(curr_node, t);
    branch_data_t branch_data = data->branch_data;
    samples_t *samples = env->samples;
    if (!is_addable(&branch_data, samples)) {
        return;
    }
    
    sparse_vector_t *pth = path(curr_node, t);
    sample_locator_t **source = obstruction_locators(pth, 1, 1, samples);
    size_t dimension = samples->dimension;
    sample_locator_t **obstructions = 
        obstruction_locators(pth, dimension, 0, samples);
    
    glp_prob *p = glp_ios_get_prob(t);
    double *solution = solution_values(curr_node, p);
    traverse(solution, t, env);
    free(solution);
    
    free(free_obstructions(obstructions, dimension));
    free(free_obstructions(source, 1));
    free(pth);
}

void irowgen(glp_tree *t, env_t *env) {
    add_inequality(t, env);
}
