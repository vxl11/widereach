#include <stdlib.h>

#include "widereach.h"

/* -------------------- Samples ------------------------------------------ */

struct samples_t *delete_samples(struct samples_t *samples) {
	free(samples->count);
	free(samples->label);
	size_t class_cnt = samples->class_cnt;
	for (size_t j = 0; j < class_cnt; j++) {
		free(samples->samples + j);
	}
	free(samples->samples);
	return samples;
}


double *random_point(size_t dimension) {
	double* values = CALLOC(dimension, double);
	for (size_t i = 0; i < dimension; i++) {
		values[i] = drand48();
	}
	return values;
}


double **generic_samples(size_t count, size_t dimension) {
	double **samples = CALLOC(count, double *);
	for (size_t j = 0; j < count; j++) {
		samples[j] = random_point(dimension);
	}
	return samples;
}


void set_sample_class(
		struct samples_t *samples, 
		size_t class, 
		int label, 
		size_t count) {
	samples->label[class] = label;
	samples->count[class] = count;
	samples->samples[class] = generic_samples(count, samples->dimension);
}


struct samples_t *random_samples(
		size_t count, size_t positives, size_t dimension) {
	struct samples_t *samples = CALLOC(1, struct samples_t);
	samples->dimension = dimension;
	samples->class_cnt = 2;
	samples->label = CALLOC(2, int);
	samples->count = CALLOC(2, size_t);
	samples->samples = CALLOC(2, double **);
	if (positives > count) {
		positives = count;
	}
	set_sample_class(samples, 0, -1, count - positives);
	set_sample_class(samples, 1,  1, positives);
	return samples;
}
