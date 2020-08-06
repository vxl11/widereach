#include "widereach.h"
#include "helper.h"

int is_branch_addable(
        int inequality_cnt, 
        branch_data_t *branch_data, 
        samples_t *samples) {
    int *directional_cnt = branch_data->directional_cnt;
    return !inequality_cnt && 
           samples->dimension == directional_cnt[0] && 
           1 == directional_cnt[1];
}

int is_addable(int inequality_cnt, glp_tree *t, env_t *env) {
    int curr_node = glp_ios_curr_node(t);
    node_data_t *data_parent = parent_data(curr_node, t);
    if (NULL == data_parent) {
        return 0;
    }
    return is_branch_addable(inequality_cnt, 
                             &(data_parent->branch_data), 
                             env->samples);
}

int is_primary_rowgen(size_t target, size_t class, double value) {
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
        if (is_primary_rowgen(target, class, p->val[i])) {
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

void print_obstruction(
            sample_locator_t **source, 
            size_t obstruction_cnt, 
            sample_locator_t **obstructions,
            glp_tree *t, env_t *env) {    
    glp_prob *p = glp_ios_get_prob(t);
    int curr_node = glp_ios_curr_node(t);
    double *solution = solution_values(curr_node, p);
    traverse(solution, t, env);
    free(solution);
    
    samples_t *samples = env->samples;
    glp_printf("source %i(%i): ", source[0]->class, source[0]->index);
    print_sample(*source[0], samples);
    for (size_t i = 0; i < obstruction_cnt; i++) {
        glp_printf("obstruction %i(%i): ", 
                   obstructions[i]->class, obstructions[i]->index);
        print_sample(*obstructions[i], samples);
    }
}


void add_obstructed(
        sparse_vector_t *constraint,
        sample_locator_t *source, 
        sample_locator_t **obstructions, 
        samples_t *samples) {
    size_t dimension = samples->dimension;
    sample_locator_t target;
    target.class = 1;
    // glp_printf("--- Obstructions ---\n");
    for (size_t i = 0; i < samples->count[1]; i++) {
        target.index = i;
        // print_sample(target, samples);
        if (is_obstructed(&target, source, dimension, obstructions, samples)) {
            append_locator(constraint, &target, 1., samples);
            // glp_printf("obstructed x%i\n", i + 1);
        }
    }
}

void add_obstructions(
        sparse_vector_t *constraint, 
        size_t len, 
        sample_locator_t **obstructions, 
        samples_t *samples) {
    double bound = 1. - (double) constraint->len;
    for (size_t i = 0; i < len; i++) {
        append_locator(constraint, obstructions[i], bound, samples);
    }
}


void initialize_cuts_data(int curr_node, cuts_data_t *cuts_data, glp_tree *t) {
    if (cuts_data->inequality_cnt > 0) {
        return;
    }
    node_data_t *data_parent = parent_data(curr_node, t);
    cuts_data->inequality_cnt = 
        data_parent != NULL ? data_parent->cuts_data.inequality_cnt : 0;
}
    

void add_inequality(glp_tree *t, env_t *env) {
    int curr_node = glp_ios_curr_node(t);
    node_data_t *data = glp_ios_node_data(t, curr_node);
    cuts_data_t *cuts_data = &(data->cuts_data);
    initialize_cuts_data(curr_node, cuts_data, t);
    
    if (!is_addable(cuts_data->inequality_cnt, t, env)) {
        return;
    }
    
    sparse_vector_t *pth = path(glp_ios_up_node(t, curr_node), t);
    samples_t *samples = env->samples;
    sample_locator_t **sources = obstruction_locators(pth, 1, 1, samples);
    sample_locator_t *source = sources[0];
    size_t dimension = samples->dimension;
    sample_locator_t **obstructions = 
        obstruction_locators(pth, dimension, 0, samples);
    free(pth);
    
    // print_obstruction(sources, dimension, obstructions, t, env);
    
    sparse_vector_t *constraint = 
        sparse_vector_blank(positives(samples) + dimension + 1);
    add_obstructed(constraint, source, obstructions, samples);
    double bound = (double) constraint->len;
    append_locator(constraint, source, bound, samples);
    add_obstructions(constraint, dimension, obstructions, samples);
    free(free_obstructions(obstructions, dimension));
    free(free_obstructions(sources, 1));
    
    glp_prob *p = glp_ios_get_prob(t);
    /*
    for (int i = 1; i <= constraint->len; i++) {
        glp_printf("%g %s\n", 
                   constraint->val[i], 
                   glp_get_col_name(p, constraint->ind[i]));
    } */
    int row_idx = glp_add_rows(p, 1);
    glp_set_mat_row(p, 
                    row_idx, 
                    constraint->len, 
                    constraint->ind, 
                    constraint->val);
    glp_set_row_bnds(p, row_idx, GLP_UP, 0, bound);
    free(delete_sparse_vector(constraint));
    cuts_data->inequality_cnt++;
    #ifdef EXPERIMENTAL
        glp_printf("Added lazy constraint %i at node %i.%i\n", 
                   row_idx, curr_node, cuts_data->inequality_cnt);
    #endif
}

void irowgen(glp_tree *t, env_t *env) {
    #ifdef EXPERIMENTAL
        glp_printf("IROWGEN\n");
    #endif
    add_inequality(t, env);
}
