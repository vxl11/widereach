#include <math.h>

#include "widereach.h"
#include "helper.h"

double iheur_round(
		int i, double solution, 
		double *X, double *Y, 
		samples_t *samples) {
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
		glp_printf("%s:\t%g\n",
                                glp_get_col_name(p, i), 
				solution[i]);
	}
}

void iheur(glp_tree *t, env_t *env) {
	glp_prob *p = glp_ios_get_prob(t);

	// Hyperplane and samples
	samples_t *samples = env->samples;
	int idx_max = violation_idx(0, samples);
	double *solution = CALLOC(idx_max + 1, double);
	double X = 0.;
	double Y = 0.;
	// glp_printf("------------- iheur ------\n");
	for (int i = 1; i < idx_max; i++) {
		solution[i] = 
			iheur_round(i, glp_get_col_prim(p, i), &X, &Y, samples);
	}

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
