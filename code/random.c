#include <stdlib.h>
#include <math.h> 

#include "helper.h"

double length_squared(size_t d, double *w) {
	double s = 0.;
	for (size_t i = 0; i < d; i++) {
		s += w[i] * w[i];
	}
	return s;
}

double basic_normal_component() {
 	double x = drand48();
	return x + x - 1.;
}

void normal_pair(double *w) {
	double v[2], s;
	do {
		v[0] = basic_normal_component();
		v[1] = basic_normal_component();
		s = length_squared(2, v);
	} while (s > 1.);
	double s_polarized = sqrt(-2. * log (s) / s);
	w[0] = s_polarized * v[0];
	w[1] = s_polarized * v[1];
}



void multiply_scalar(double a, size_t d, double *w) {
	for (size_t i = 0; i < d; i++) {
		w[i] *= a;
	}
}

double *random_point(size_t dimension) {
	double* values = CALLOC(dimension, double);
	for (size_t i = 0; i < dimension; i++) {
		values[i] = drand48();
	}
	return values;
}


void random_unit_vector(size_t d, double *w) {
	double x[2];
	for (size_t i = 0; i < d; i += 2) {
		normal_pair(x);
		w[i] = x[0];
		if (i + 1 < d) {
			w[i + 1] = x[1];
		}
	}
	multiply_scalar(1. / sqrt(length_squared(d, w)), d, w);
}
