#include "widereach.h"

void callback(glp_tree *t, void *info) {
	env_t *env = (env_t *) info;
	switch (glp_ios_reason(t)) {
		case GLP_IHEUR:
			iheur(t, env);
			break;
		case GLP_IBRANCH:
			break;
		case GLP_ISELECT:
			break;
	}
}
