#include <stdio.h> // TODO
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

void iheur(glp_tree *t, env_t *env) {
	glp_prob *p = glp_ios_get_prob(t);
	samples_t *samples = env->samples;
	int idx_max = violation_idx(0, samples);
	double *solution = CALLOC(idx_max, double);
	double X = 0.;
	double Y = 0.;
	printf("-- iheur -- \n");
	for (int i = 1; i < idx_max; i++) {
		printf("%i\t%g\n", i, glp_get_col_prim(p, i));
		solution[i] = 
			iheur_round(i, glp_get_col_prim(p, i), &X, &Y, samples);
		printf("%i\t%g\n", i, solution[i]);
	}
	// Violation
	free(solution);
}
