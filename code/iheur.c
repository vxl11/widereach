#include <math.h>
#include <float.h>

#include "widereach.h"
#include "helper.h"

double rounded_positive(
		sample_locator_t *loc, 
		double solution, 
		double *plane, 
		env_t *env) {
    if (1. == solution) {
        return solution;
    }
    
    double solution_rounded;
	params_t *params = env->params;
	switch (params->iheur_method) {
		case simple:
			solution_rounded = floor(solution);
            break;
		case deep:
			solution_rounded = side(loc, env->samples, plane, 
                                    params->epsilon_positive);
            break;
        default:
            solution_rounded = -1.;
	}
    return solution_rounded;
}



double rounded_negative(
		sample_locator_t *loc, 
		double solution, 
		double *plane, 
		env_t *env) {
    if (0. == solution) {
        return solution;
    }
    
    double solution_rounded;
	params_t *params = env->params;
	switch (params->iheur_method) {
		case simple:
			solution_rounded = ceil(solution);
            break;
		case deep:
			solution_rounded = side(loc, env->samples, plane, 
                                    params->epsilon_negative);
            break;
        default:
            solution_rounded = -1.;
	}
	// Offset rounding errors
	if (ceil(solution) == solution && solution < solution_rounded) {
            solution_rounded = solution;
    }
    return solution_rounded;
}


double rounded(
		sample_locator_t *loc, 
		double solution, 
		double *plane, 
		double *X, 
		double *Y, 
		env_t *env) {
	samples_t *samples = env->samples;
	size_t class = loc->class;
	glp_assert(class >= 0);
	int label = samples->label[class];
	double solution_rounded;
	if (label > 0) {
		solution_rounded = rounded_positive(loc, solution, plane, env);
		(*X) += solution_rounded;
	} else {
		solution_rounded = rounded_negative(loc, solution, plane, env);
		(*Y) += solution_rounded;
	}
	return solution_rounded;
}


double iheur_round(
		int i, double solution, 
		double *plane,
		double *X, double *Y, 
		env_t *env) {
	samples_t *samples = env->samples;
	sample_locator_t *loc = locator(i, samples);
    double solution_rounded;
	if (loc->class < 0) {
		solution_rounded = solution;
	} else {
      solution_rounded = rounded(loc, solution, plane, X, Y, env);
    }
	free(loc); 
	return solution_rounded;
}

double iheur_violation(double X, double Y, params_t *params) {
	double theta = params->theta;
	double violation = (theta - 1.) * X + theta * Y + 
		theta * params->epsilon_precision;
	if (violation < 0. && GLP_LO == params->violation_type) {
		violation = 0.;
	}
	return violation;
}


void print_solution(int idx_max, double *solution, glp_prob *p) {
	glp_printf("Solution\n");
	for (int i = 1; i <= idx_max; i++) {
		glp_printf("%s:\t%g(%.18g)\n",
                glp_get_col_name(p, i), 
				solution[i],
				glp_get_col_prim(p, i));
	}
}


double *hyperplane(glp_prob *p, samples_t *samples) {
	size_t size = samples->dimension + 1;
	double *hyperplane = CALLOC(size, double);
	for (int i = 1; i <= size; i++) {
		hyperplane[i-1] = glp_get_col_prim(p, i);
	}
	return hyperplane;
}


double *fractional_solution(glp_prob *p, samples_t *samples) {
    int idx_max = violation_idx(0, samples);
    double *solution = CALLOC(idx_max + 1, double);
    for (int i = 1; i <= idx_max; i++) {
        solution[i] = glp_get_col_prim(p, i);
    }
    return solution;
}

/* This function should be refactored because it had way too many memory
 * leaks. */
void iheur(glp_tree *t, env_t *env) {
    int curr_node = glp_ios_curr_node(t);
    #ifdef EXPERIMENTAL
        glp_printf("Chosen node (at iheur)  %i\n", curr_node);
    #endif
    node_data_t *data = (node_data_t *) glp_ios_node_data(t, curr_node);
    if (data->iheur) {
        return;
    }

	glp_prob *p = glp_ios_get_prob(t);
	samples_t *samples = env->samples;
	double *solution = fractional_solution(p, samples);
    double value = hyperplane_to_solution(solution + 1, solution, env);
    
    // Attempt to find an initial solution based on a random hyperplane
    solution_data_t *solution_data = env->solution_data;
    double *random_solution = blank_solution(samples);
    double *h = best_random_hyperplane(NULL == solution_data->integer_solution, 
                                       env);
    double random_objective_value = 
      hyperplane_to_solution(h, random_solution, env);
    free(h);
    if (random_objective_value > value) {
        free(solution);
        solution = random_solution;
        value = random_objective_value;
    } else {
        free(random_solution);
    }

	int status = glp_ios_heur_sol(t, solution);
	if (!status) {
		print_solution(violation_idx(0, samples), solution, p);
		traverse(solution, t, env);
        int *violation_indexes = sorted_by_violation(solution + 1, env);
        update_solution(solution_data, solution, value, violation_indexes);
	} else { 
      free(solution);
    }
    
    data->iheur = 1;
}
