#include "widereach.h"

void ibingo(glp_tree *t, env_t *env) {
	glp_printf("IBINGO\n");
	traverse(NULL, t, env);
}
