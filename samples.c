#include <stdlib.h>

#include "widereach.h"

/* --------------------- Sample ------------------------------------------ */

/* Free the values array in the given individual sample */
void delete_values(struct sample_t *sample) {
	free(sample->values);
}

/* Generate random values and returns it in a newly allocated array. */
double *random_point(size_t dimension) {
	double* values = CALLOC(dimension, double);
	for (size_t i = 0; i < dimension; i++) {
		values[i] = drand48();
	}
	return values;
}


/* -------------------- Samples ------------------------------------------ */

void delete_samples(struct samples_t *samples) {
	size_t count = samples->count;
	for (size_t j = 0; j < count; j++) {
		delete_values(samples + j);
	}
	free(samples);
}

struct sample_t *random_samples(
		size_t count, size_t positives, size_t dimension) {
	if (positives > count) {
		return NULL;
	}
	struct sample_t *samples = CALLOC(n, struct sample_t);
	samples->count = count;
	for (size_t j = 0; j < n; j++) {
		samples[j].direction = 0;
		samples[j].values = random_point(dimension);
	}
	return samples;
}
