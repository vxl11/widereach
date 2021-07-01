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
