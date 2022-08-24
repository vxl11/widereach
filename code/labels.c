#include "widereach.h"

char label_to_varname(int label) {
	return label > 0 ? 'x' : 'y';
}

double label_to_obj(int label) {
	return label > 0 ? 1. : 0.;
}

double label_to_bound(int label, params_t *params) {
	return label > 0 ? 
		1. - params->epsilon_positive : 
		-params->epsilon_negative;
}
