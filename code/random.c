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

double *random_point_affine(size_t dimension, double shift, double side) {
	double *values = CALLOC(dimension, double);
	for (size_t i = 0; i < dimension; i++) {
		values[i] = side * drand48() + shift;
	}
	return values;
}

double *random_point(size_t dimension) {
  return random_point_affine(dimension, 0., 1.);
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

void cumulative2density(
    double side,
    size_t dimension, 
    double *cumulative, 
    double *density) {
  size_t cumulative_size = dimension - 1;
  density[0] = cumulative[0];
  for (size_t i = 1; i < cumulative_size; i++) {
    density[i] = cumulative[i] - cumulative[i - 1];
  }
  density[cumulative_size] = side - cumulative[cumulative_size - 1];
}

int has_zero(size_t dimension, double *w) {
  for (size_t i = 0; i < dimension; i++) {
    if (0. == w[i]) {
      return 1;
    }
  }
  return 0;
}

void random_simplex_point(double side, size_t dimension, double *w) {
  size_t cumulative_size = dimension - 1;
  double *cumulative;
  while (1) {
    cumulative = random_point_affine(cumulative_size, 0., side);
    cumulative2density(side, dimension, cumulative, w);
    free(cumulative);
    if (!has_zero(dimension, w)) {
      break;
    }
  }
}
