#include "widereach.h"
#include "helper.h"

double **random_simplex_points(size_t count, double side, size_t dimension) {
	double **samples = CALLOC(count, double *);
	for (size_t j = 0; j < count; j++) {
		samples[j] = random_simplex_point(side, dimension);
	}
	return samples;
}

void set_sample_class_simplex(
		samples_t *samples, 
		size_t class, 
		int label, 
		size_t count,
        double side) {
	samples->label[class] = label;
	samples->count[class] = count;
	samples->samples[class] = 
      random_simplex_points(count, side, samples->dimension);
}

samples_t *random_simplex_samples(
		size_t count, size_t positives, size_t dimension, double side) {
	samples_t *samples = CALLOC(1, samples_t);
	samples->dimension = dimension;
	samples->class_cnt = 2;
	samples->label = CALLOC(2, int);
	samples->count = CALLOC(2, size_t);
	samples->samples = CALLOC(2, double **);
	if (positives > count) {
		positives = count;
	}
	set_sample_class(samples, 0, -1, count - positives);
	set_sample_class_simplex(samples, 1,  1, positives, side);
	return samples;
}
