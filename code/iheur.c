#include <math.h>

#include "widereach.h"
#include "helper.h"

double iheur_deep_round(
		int i, double solution, 
		double *hyperplane,
		double *X, double *Y, 
		env_t *env) {
	samples_t *samples = env->samples;

	sample_locator_t *loc = locator(i, samples);
	int class = loc->class;
	free(loc); 
	if (class < 0) {
		return solution;
	}
	
	params_t *params = env->params;
	int label = samples->label[class];
	double solution_rounded;
	if (label > 0) {
		solution_rounded = 
			side(loc, samples, hyperplane, 
					params->epsilon_positive);
		(*X) += solution_rounded;
	} else {
		solution_rounded =
		       	side(loc, samples, hyperplane, 
					params->epsilon_negative);
		(*Y) += solution_rounded;
	}

	return solution_rounded;
}


double iheur_round(
		int i, double solution, 
		double *hyperplane,
		double *X, double *Y, 
		env_t *env) {
	samples_t *samples = env->samples;
	sample_locator_t *loc = locator(i, samples);
	int class = loc->class;
	free(loc); 

	if (class < 0) {
		return solution;
	}

	int label = samples->label[class];
	double solution_rounded;
	if (label > 0) {
		solution_rounded = floor(solution);
		(*X) += solution_rounded;
	} else {
		solution_rounded = ceil(solution);
		(*Y) += solution_rounded;
	}
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
		glp_printf("%s:\t%g(%g)\n",
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

void iheur(glp_tree *t, env_t *env) {
	// glp_printf("Chosen node (at iheur)  %i\n", glp_ios_curr_node(t));

	glp_prob *p = glp_ios_get_prob(t);

	samples_t *samples = env->samples;
	double *plane = hyperplane(p, samples);

	// Samples
	int idx_max = violation_idx(0, samples);
	double *solution = CALLOC(idx_max + 1, double);
	double X = 0.;
	double Y = 0.;
	// glp_printf("------------- iheur ------\n");
	for (int i = 1; i < idx_max; i++) {
		solution[i] = 
			/*
			iheur_round(i, glp_get_col_prim(p, i), plane, 
					&X, &Y, env);*/
			iheur_deep_round(i, glp_get_col_prim(p, i), plane, 
					&X, &Y, env);
	}
	free(plane);

	// Violation
	params_t *params = env->params;
	solution[idx_max] = iheur_violation(X, Y, params);

	int status = glp_ios_heur_sol(t, solution);
	if (!status) {
		print_solution(idx_max, solution, p);
		traverse(solution, t, env);
	}
	free(solution);
}
