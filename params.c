#include "widereach.h"
#include "helper.h"

params_t *params_default() {
	params_t *params = CALLOC(1, params_t);
	params->name = "DEFAULT";
	params->verbosity = GLP_MSG_ALL;
	params->theta = .5;
	params->epsilon_positive = 1e-3;
	params->epsilon_negative = 2e-3;
	params->epsilon_precision = 3e-3;
	params->lambda = 7.;
	params->violation_type = GLP_LO;
	return params;
}
