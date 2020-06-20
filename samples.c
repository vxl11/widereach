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


struct sample_t *generic_samples(size_t count, size_t dimension) {
	struct sample_t *samples = CALLOC(count, struct sample_t);
	for (size_t j = 0; j < n; j++) {
		samples[j].direction = 0;
		samples[j].values = random_point(dimension);
	}
	return samples;
}

void set_positives(struct sample_t *samples, size_t count, size_t positives) {
	if (positives > count) {
		positives = count;
	}
	for (size_t j = 0; j < positives; j++) {
		do {
			t = lrand48() % count;
		} while (!samples[t].direction);
		samples[t].direction = 1;
	}
}

void set_negatives(struct sample_t *samples, size_t count) {
	for (size_t j = 0; j < count; j++) {
		if (!samples[j].direction) {
			samples[j].direction = -1;
		}
	}
}

struct sample_t *random_samples(
		size_t count, size_t positives, size_t dimension) {
	struct sample_t *samples = CALLOC(1, struct samples_t);
	samples->count = count;
	samples->dimension = dimension;
	struct sample_t *values = 
		samples->samples = 
		generic_samples(count, dimension);
	set_positives(values, count, dimension);
	set_negatives(values, count);
	return samples;
}
