#include "widereach.h"
#include "helper.h"

void mirror_sample(size_t dimension, double *sample) {
  for (size_t i = 0; i < dimension; i++) {
    sample[i] = 1. - sample[i];
  }
}

double **random_simplex_points(size_t count, double side, size_t dimension) {
	double **samples = CALLOC(count, double *);
    size_t count_simplex = count / 2;
    size_t mirror_count = count_simplex / 2;
	for (size_t j = 0; j < count_simplex; j++) {
		samples[j] = random_simplex_point(side, dimension);
        if (j >= mirror_count) {
          // mirror_sample(dimension, samples[j]);
        }
	}
	for (size_t j = count_simplex; j < count; j++) {
      samples[j] = random_point(dimension);
    }
	return samples;
}

void set_sample_class_simplex(
		samples_t *samples, 
		size_t class, 
		int label, 
        size_t count,
		simplex_info_t *simplex_info) {
	samples->label[class] = label;
	samples->count[class] = count;
	samples->samples[class] = 
      random_simplex_points(count, simplex_info->side, samples->dimension);
}

samples_t *random_simplex_samples(simplex_info_t *simplex_info) {
	samples_t *samples = CALLOC(1, samples_t);
	samples->dimension = simplex_info->dimension;
	samples->class_cnt = 2;
	samples->label = CALLOC(2, int);
	samples->count = CALLOC(2, size_t);
	samples->samples = CALLOC(2, double **);
    size_t *positives = &(simplex_info->positives);
    size_t count = simplex_info->count;
	if (*positives > count) {
		*positives = count;
	}
	set_sample_class(samples, 0, -1, count - *positives);
	set_sample_class_simplex(samples, 1,  1, *positives, simplex_info);
	return samples;
}
