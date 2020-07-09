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

int class_direction(int class, samples_t *samples) {
	return samples->label[class] > 0 ? GLP_UP_BRNCH : GLP_DN_BRNCH;
}

int class_reverse_direction(int class, samples_t *samples) {
	return samples->label[class] < 0 ? GLP_UP_BRNCH : GLP_DN_BRNCH;
}

void branch_on(int index, glp_tree *t, samples_t *samples) {
	node_data_t *data = initialize_data(t, samples);
    branch_data_t *branch_data = &(data->branch_data);
	branch_data->branching_variable = index;
    int class = index_to_class(index, samples);
    branch_data->class_cnt[class]++;
    int direction = class_direction(class, samples);
    // int direction = class_reverse_direction(class, samples);
    /* glp_printf("%s of %i to %i\n", 
               glp_get_col_name(glp_ios_get_prob(t), index), 
               class, direction); */
	branch_data->direction = direction;
 
    glp_ios_branch_upon(t, index, direction); 
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

	branch_on(candidate, t, samples);
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

	branch_on(candidate, t, samples);
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
	branch_on(candidate_idx, t, samples);
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

int is_first_deficient(int a, int b, int threshold) {
    return a < threshold && b >= threshold;
}

void branch_even(glp_tree *t, env_t *env) {
    int parent = glp_ios_up_node(t, glp_ios_curr_node(t));
    if (!parent) {
        // At the root, branch on a random node
        random_flat(t, env);
        return;
    }
    
    node_data_t *data = (node_data_t *) glp_ios_node_data(t, parent);
    // int *class_cnt = data->class_cnt;
    int *class_cnt = data->directional_cnt;
    int threshold = 1;
    // int threshold = (int) env->samples->dimension;
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
	ibranch_LFV(t, env);
	*/
	// random_flat(t, env);
	branch_even(t, env);
	return;

	/* Choice of branching index: try high rank first, and if that
	 * fails move one to high score (which then becomes the next ranked
	 * index) */
	int idx = highest_rank_index(t, env);
	if (idx < 0) { 
		idx = highest_score_index(t, env);
		append_data(env->solution_data, idx);
	}
	glp_assert(idx > 0);

	branch_on(idx, t, env->samples);
}
