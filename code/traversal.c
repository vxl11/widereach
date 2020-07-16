#include <string.h>

#include "widereach.h"
#include "helper.h"

void print_branch_variable(int idx, samples_t *samples) {
	sample_locator_t *loc = locator(idx, samples);
	print_sample(*loc, samples);
	free(loc);
}

void traverse(double *solution, glp_tree *t, env_t *env) {
	glp_printf("------- Traversal -------\n");
	glp_prob *p = glp_ios_get_prob(t);
	samples_t *samples = env->samples;
    node_data_t *data;
    branch_data_t *branch_data;
	int branching_variable;
	for (int curr_node = glp_ios_curr_node(t);
			curr_node != 0;
			curr_node = glp_ios_up_node(t, curr_node)) {
        data = (node_data_t *) glp_ios_node_data(t, curr_node);
        branch_data = &(data->branch_data);
		if (data->initialized && branch_data->initialized) {
			branching_variable = data->branch_data.branching_variable;
			glp_printf("%s (%i)@%i[%i,%i][%i,%i]\t", 
					glp_get_col_name(p, branching_variable), 
					branching_variable,
					branch_data->direction,
                    branch_data->class_cnt[0], 
                    branch_data->class_cnt[1],
                    branch_data->directional_cnt[0], 
                    branch_data->directional_cnt[1]);
			if (NULL != solution) {
				glp_printf("%g\t", 
						solution[branching_variable]);
			}
			print_branch_variable(branching_variable, samples);
		}
	}
}

int *branching_variables(int node, glp_tree *t, samples_t *samples) {
    size_t n = samples_total(samples);
    int *bv = CALLOC(n, int);
    memset(bv, 0, sizeof(*bv));
    int j = 0;
    for (int curr_node = glp_ios_up_node(t, glp_ios_curr_node(t));
         curr_node != 0;
         curr_node = glp_ios_up_node(t, curr_node)) {
        node_data_t *data = (node_data_t *) glp_ios_node_data(t, curr_node);
        glp_assert(data != NULL && data->initialized);
        branch_data_t *branch_data = &(data->branch_data);
        glp_assert(branch_data->initialized);
        bv[j++] = branch_data->branching_variable;
    }
    return bv;
}


