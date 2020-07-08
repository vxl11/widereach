#include <math.h>

#include "widereach.h"
#include "helper.h"

double rounded_positive(
		sample_locator_t *loc, 
		double solution, 
		double *plane, 
		env_t *env) {
	params_t *params = env->params;
	switch (params->iheur_method) {
		case simple:
			return floor(solution);
		case deep:
			return side(loc, env->samples, plane, 
					params->epsilon_positive);
	}
	return -1.;
}



double rounded_negative(
		sample_locator_t *loc, 
		double solution, 
		double *plane, 
		env_t *env) {
	params_t *params = env->params;
	switch (params->iheur_method) {
		case simple:
			return ceil(solution);
		case deep:
			return side(loc, env->samples, plane, 
					params->epsilon_negative);
	}
	return -1.;
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
	if (loc->class < 0) {
		return solution;
	}

	double solution_rounded = rounded(loc, solution, plane, X, Y, env);
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
	glp_printf("Chosen node (at iheur)  %i\n", glp_ios_curr_node(t));
	// glp_printf("------------- iheur ------\n");

	glp_prob *p = glp_ios_get_prob(t);

	samples_t *samples = env->samples;
	double *plane = hyperplane(p, samples);

	// Samples
	int idx_max = violation_idx(0, samples);
	double *solution = CALLOC(idx_max + 1, double);
	double X = 0.;
	double Y = 0.;
	for (int i = 1; i < idx_max; i++) {
		solution[i] = 
			iheur_round(i, glp_get_col_prim(p, i), plane, 
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
