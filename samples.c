#include <stdlib.h>

#include "widereach.h"

void delete_samples(size_t n, struct sample_t *samples) {
	for (size_t j = 0; j < n; j++) {
		free(samples[j].values);
	}
	free(samples);
}



struct sample_t *random_samples(size_t n, int positives, int dimension) {
	if (positives > n) {
		return NULL;
	}
	struct sample_t *samples = CALLOC(n, struct sample_t);
	return samples;
}
