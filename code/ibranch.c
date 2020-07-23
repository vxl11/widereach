#include <math.h>
#include <float.h>

#include "widereach.h"
#include "helper.h"

int wzero(glp_prob *p, int dimension) {
	int w_cnt = 0;
	for (int i = 1; i <= dimension; i++) {
		if (fabs(glp_get_col_prim(p, i)) < 1e-6) {
			w_cnt++;
		}
	}
	return w_cnt;
}

double integer_infeasibility(glp_tree *t, samples_t *samples) {
    glp_prob *p = glp_ios_get_prob(t);
    int idx_min = idx_extreme(0, 1, 0, samples);
    int idx_bnd = idx_extreme(0, 1, 1, samples);
    int idx_max = idx_extreme(0, 0, 1, samples);
    double infeasibility = 0.;
    double iptr;
    for (int i = idx_min; i <= idx_max; i++) {
        double value = modf(glp_get_col_prim(p, i), &iptr);
        /* Commonly accepted definition of infeasibility 328 (1e-1), 412 (1e-12)
        if (value > 0.5) {
            value = 1. - value;
        }*/
        // Closeness to hyperplane 327(1e-1) 327 (1e-2), 412 (1e-12)
        if (i <= idx_bnd) {
            value = 1. - value;
        }
        // Distance from hyperplane 125
        /* if (i > idx_bnd) {
            value = 1. - value;
        } */
        infeasibility += value;
    }
    return infeasibility;
}

int integer_class(int index, double *intobj, glp_tree *t, env_t *env) {
    *intobj = -DBL_MAX;
    samples_t *samples = env->samples;
    int class = index_to_class(index, samples);
    
    solution_data_t *solution_data = env->solution_data;
    double *integer_solution = solution_data->integer_solution;
    if (NULL == integer_solution) {
        return class;
    }
    
    int curr_node = glp_ios_curr_node(t);
    sparse_vector_t *p = path(curr_node, t);
    if (NULL == p) {
        return class;
    }
    if (is_path_consistent(p, integer_solution)) {
        class = (int) integer_solution[index];
        *intobj = solution_data->intopt;
    }
    free(p);
    
    return class; 
}

int class_direction(int class, samples_t *samples) {
	return samples->label[class] > 0 ? GLP_UP_BRNCH : GLP_DN_BRNCH;
}

int class_reverse_direction(int class, samples_t *samples) {
	return samples->label[class] < 0 ? GLP_UP_BRNCH : GLP_DN_BRNCH;
}

branch_data_t *initialize_branch_data(int index, glp_tree *t, env_t *env) {
    int curr_node = glp_ios_curr_node(t);
    samples_t *samples = env->samples;
	node_data_t *data = initialize_data(curr_node, t, samples);
    branch_data_t *branch_data = &(data->branch_data);
    
    // Update branch data that depend on the parent's
    int primary = is_direction_primary(curr_node, 1, t, samples);
    node_data_t *data_parent = parent_data(curr_node, t);
    if (data_parent != NULL) {
        int branching_variable = data_parent->branch_data.branching_variable;
        branch_data->branching_value = 
            glp_get_col_prim(glp_ios_get_prob(t), branching_variable);
        int branching_class = index_to_class(branching_variable, samples);
        branch_data->directional_cnt[branching_class] += primary;
    }
    
    // Update core branch data
    // int class = integer_class(index, &(branch_data->intobj), t, env);
    int class = index_to_class(index, samples);
    int direction = class_direction(class, samples);
    // int direction = class_reverse_direction(class, samples);
    // int direction = GLP_NO_BRNCH;
    branch_data->branching_variable = index;
    branch_data->class_cnt[index_to_class(index, samples)]++;
    branch_data->direction = direction;
    branch_data->ii_sum = integer_infeasibility(t, samples);
    branch_data->primary_direction = primary;
    branch_data->intobj = -DBL_MAX;
    branch_data->is_consistent = 0;
    
    branch_data->initialized = 1;
    return branch_data;
}
    

void branch_on(int index, glp_tree *t, env_t *env) {
    branch_data_t *branch_data = initialize_branch_data(index, t, env);
 
    // Update last branching node
    int curr_node = glp_ios_curr_node(t);
    env->solution_data->branching_node = curr_node;
    #ifdef EXPERIMENTAL
        glp_printf("branch from %i\n", curr_node);
    #endif
 
    /* Note that if this node has been branched from already, 
     * the index is ignored in favor of the previous branching variable */
    int branching_variable = branch_data->branching_variable;
    if (glp_ios_can_branch(t, branching_variable)) {
        glp_ios_branch_upon(t, branching_variable, branch_data->direction); 
    }
}

int random_bounded(int idx_min, int idx_max, glp_tree *t) {
    int *eligible = CALLOC(idx_max - idx_min + 1, int);
	int eligible_cnt = 0;
	for (int i = idx_min; i <= idx_max; i++) {
		if (glp_ios_can_branch(t, i)) {
			eligible[eligible_cnt++] = i;
		}
	}
	int candidate = eligible_cnt > 0 ? eligible[lrand48() % eligible_cnt] : -1;
	free(eligible);
	return candidate;
}

int random_eligible(int class, glp_tree *t, samples_t *samples) {
    int idx_min = idx_extreme(0, class, 0, samples);
    int idx_max = idx_extreme(0, class, 1, samples);
	return random_bounded(idx_min, idx_max, t);
}

void random_branch(int class, glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	// High priority class first
	int candidate = random_eligible(class, t, samples);
	if (candidate < 0) { 
		// No candidate, trying low priority class
//         glp_printf("cannot find it, back to other class\n");
		candidate = random_eligible(!class, t, samples);
	}

	branch_on(candidate, t, env);
}

void random_flat(glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	int dimension = samples->dimension;
	int candidate = 
		random_bounded(
            dimension + 2, 
            dimension + samples_total(samples) + 1, 
            t);
	// int direction = index_direction(candidate, samples);
	// int direction = drand48() > .5 ? GLP_UP_BRNCH : GLP_DN_BRNCH;
	// int direction = index_reverse_direction(candidate, samples);
	// int direction = GLP_NO_BRNCH;

	branch_on(candidate, t, env);
}



void ibranch_LFV(glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	int candidate_idx;
	int idx_max = violation_idx(0, samples);
	for (int i = idx_max; i >= 1; i--) {
		if (glp_ios_can_branch(t, i)) {
			candidate_idx = i;
			break;
		}
	}
	branch_on(candidate_idx, t, env);
}



/* Return the index of the decision variable on which branching is allowed
 * and that ranks the first in the solution data. 
 * If no suitable index is found, it returns -1. */
int highest_rank_index(glp_tree *t, env_t *env) {
	solution_data_t *data = env->solution_data;
	size_t rank_significant = data->rank_significant;
	int *rank = data->rank;
	for (size_t i = 0; i < rank_significant; i++) {
		int idx = rank[i];
		if (glp_ios_can_branch(t, idx)) {
			return (int) idx;
		}
	}
	return -1;
}


/* Return the index with the highest score, where indexes are only
 * considered for variables on which branching is allowed, and are ranked
 * by lower label first, and ties are broken with the absolute difference
 * between variable values and branch target. 
 * The suggested branching direction is stored in the direction argument. */
int highest_score_index(glp_tree *t, env_t *env) {
	samples_t *samples = env->samples;
	double branch_target = env->params->branch_target;

	glp_prob *p = glp_ios_get_prob(t);

	/* Possible diagnostics */
	// int w_zero_cnt = wzero(p, samples->dimension); 
	// int positive_cnt = 0;
	// int negative_cnt = 0;

	double candidate_frac = DBL_MAX;
	int candidate_idx;
	int candidate_label = 2;
	int idx_max = violation_idx(0, samples);
	for (int i = idx_max; i > 0; i--) {
		if (glp_ios_can_branch(t, i)) {
			int label = index_label(i, samples);
			if (label > candidate_label) {
				/* Since negatives take precendece over
				 * positives, there is no point continuing
				 * the search */
				break;
			}
			double value = glp_get_col_prim(p, i);
			if (label > 0) {
				// positive_cnt++;
				value = 1. - value;
			} else {
				// negative_cnt++;
			}
			value = fabs(value - branch_target);
			if (value <= candidate_frac) {
				candidate_frac = value;
				candidate_idx = i;
				candidate_label = label;
			}
		}
	}
	return candidate_idx;
}

/* Returns the eligible decision variable that is closest to the current
 * hyperplane (minus branch target). 
 * TODO: should break ties by directional count */
void branch_closest(glp_tree *t, env_t *env) {
	glp_prob *p = glp_ios_get_prob(t);
    double branch_target = env->params->branch_target;
	double candidate_frac = DBL_MAX;
	int candidate_idx = 0;
    samples_t *samples = env->samples;
	int idx_max = violation_idx(0, samples);
	for (int i = 1; i < idx_max; i++) {
		if (!glp_ios_can_branch(t, i)) {
            continue;
        }
        double value = glp_get_col_prim(p, i);
        if (index_label(i, samples) > 0) {
            value = 1. - value;
        }
        value = fabs(value - branch_target);
        if (value <= candidate_frac) {
            candidate_frac = value;
            candidate_idx = i;
		}
	}
	branch_on(candidate_idx, t, env);
}

void branch_by_violation(glp_tree *t, env_t *env) {
    int *violation_index = env->solution_data->violation_index;
    if (NULL == violation_index) {
        return;
    }
    
    #ifdef EXPERIMENTAL
        glp_printf("branching variable: ");
    #endif
    int samples_cnt = samples_total(env->samples);
    int candidate_idx = 0;
    for (int i = 0; i < samples_cnt; i++) {
        int idx = violation_index[i];
        if (!idx) {
            #ifdef EXPERIMENTAL
                glp_printf(" -> rnd\n");
            #endif
            random_flat(t, env);
            return;
        }
        #ifdef EXPERIMENTAL
            glp_printf("%i ", idx);
        #endif
        if (glp_ios_can_branch(t, idx)) {
            candidate_idx = idx;
            break;
        }
    }
    #ifdef EXPERIMENTAL
        glp_printf("-> %i\n", candidate_idx);
    #endif
    branch_on(candidate_idx, t, env);
}

int is_first_deficient(int a, int b, int threshold) {
    return a < threshold && threshold <= b;
}

void branch_even(glp_tree *t, env_t *env) {
    node_data_t *data = parent_data(glp_ios_curr_node(t), t);
    if (NULL == data) {
        // At the root, branch on a random node
        random_flat(t, env);
        return;
    }

    // int *class_cnt = data->class_cnt;
    int *class_cnt = data->branch_data.directional_cnt;
    int threshold = 1;
    // int threshold = (int) env->samples->dimension; // next
    // glp_printf("count %i,%i\n", data->class_cnt[0], data->class_cnt[1]);
    if (is_first_deficient(class_cnt[0], class_cnt[1], threshold)) { 
        /* Deficient negative sample set:
            Branch on a negative sample if at all possible */
        // glp_printf("attempting a random negative\n");
        random_branch(0, t, env);
    } else if (is_first_deficient(class_cnt[1], class_cnt[0], threshold)) { 
        /* Deficient positive sample set
            Branch on a positive sample if at all possible */
        // glp_printf("attempting a random positive\n");
        random_branch(1, t, env);
    } else {
        // glp_printf("attempting a random flat\n");
        random_flat(t, env);
    }
    return;
}

void ibranch(glp_tree *t, env_t *env) {
	// glp_printf("Chosen node (at ibranch)  %i\n", glp_ios_curr_node(t));
    /*
    node_data_t *data = glp_ios_node_data(t, glp_ios_curr_node(t));
    if (data->branch_data.initialized) {
        glp_printf("rebranching\n");
    }*/
	/*
	ibranch_LFV(t, env); 
	*/
	// random_flat(t, env); 
	branch_even(t, env); 
	// branch_closest(t, env);
	// branch_by_violation(t, env);
	return;

	/* Choice of branching index: try high rank first, and if that
	 * fails move one to random flat (which then becomes the next ranked
	 * index) */
	int idx = highest_rank_index(t, env);
	if (idx < 0) { 
		// idx = highest_score_index(t, env);
        samples_t *samples = env->samples;
        int dimension = samples->dimension;
        idx = 
            random_bounded(
                dimension + 2, 
                dimension + samples_total(samples) + 1, 
                t);
		append_data(env->solution_data, idx);
	}
	glp_assert(idx > 0);

	branch_on(idx, t, env);
}
