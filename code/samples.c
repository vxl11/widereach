#include "widereach.h"
#include "helper.h"

/* -------------------- Samples ------------------------------------------ */

int is_binary(const samples_t *samples) {
	return 	 2 == samples->class_cnt &&
		-1 == samples->label[0] &&
		 1 == samples->label[1];
}


size_t samples_total(const samples_t *samples) {
	size_t *count = samples->count;
	int cnt = 0;
	for (size_t class = 0; class < samples->class_cnt; class++) {
		cnt += count[class];
	}
	return cnt;
}


size_t positives(const samples_t *samples) {
	return samples->count[1];
}

size_t negatives(const samples_t *samples) {
	return samples->count[0];
}

samples_t *delete_samples(samples_t *samples) {
	free(samples->label);
	size_t class_cnt = samples->class_cnt;
	for (size_t i = 0; i < class_cnt; i++) {
		size_t cnt = samples->count[i];
		for (size_t j = 0; j < cnt; j++) {
			free(samples->samples[i][j]);
		}
		free(samples->samples[i]);
	}
	free(samples->samples);
	free(samples->count);
	return samples;
}

double primary_value(int label) {
    return label > 0 ? 1. : 0.;
}

double **random_point_cluster(
        size_t count, 
        size_t dimension, 
        double shift, 
        double side,
        double **samples) {
    for (size_t j = 0; j < count; j++) {
		samples[j] = random_point_affine(dimension, shift, side);
	}
	return samples + count;
}

double **random_points(size_t count, size_t dimension) {
	double **samples = CALLOC(count, double *);
	for (size_t j = 0; j < count; j++) {
		samples[j] = random_point(dimension);
	}
	return samples;
}

/*
double **random_point_clusters(
        size_t cluster_cnt, 
        size_t *count, 
        double *shift;
        double *size,
        size_t dimension) {
    size_t count_total = 0;
    for (size_t h = 0; h < cluster_cnt; h++) {
      count_total += count[h];
    }
    double **samples = double **samples_pointer = CALLOC(count_total, double *);
    for (size_t h = 0; h < cluster_cnt; h++) {
      samples_pointer = random_point_cluster(count[h], dimension, shift, side, samples_pointer);
    }
}
*/

void set_sample_class(
		samples_t *samples, 
		size_t class, 
		int label, 
		size_t count) {
	samples->label[class] = label;
	samples->count[class] = count;
	samples->samples[class] = random_points(count, samples->dimension);
}

/*
void set_sample_class_clusters(
		samples_t *samples, 
		size_t class, 
		int label, 
        size_t cluster_cnt,
		size_t *count) {
	samples->label[class] = label;
	samples->count[class] = count;
	samples->samples[class] = random_points(count, samples->dimension);
}*/


samples_t *random_samples(
		size_t count, size_t positives, size_t dimension) {
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
	set_sample_class(samples, 1,  1, positives);
	return samples;
}


void print_sample(sample_locator_t loc, samples_t *samples) {
	size_t class = loc.class;
	glp_printf("%i ", samples->label[class]);

	double *sample = samples->samples[class][loc.index];
	size_t dimension = samples->dimension;
	for (size_t j = 0; j < dimension; j++) {
		glp_printf("%g ", sample[j]);
	}
	glp_printf("\n");

}

void print_samples(samples_t *samples) {
	size_t class_cnt = samples->class_cnt;
	size_t *counts = samples->count;
	for (size_t class = 0; class < class_cnt; class ++) {
		size_t count = counts[class];
		for (size_t i = 0; i < count; i++) {
			sample_locator_t loc = { class, i };
			print_sample(loc, samples);
		}
	}
}


double distance(
        sample_locator_t *loc, 
        samples_t *samples, 
        double *hyperplane, 
		double precision) {
	size_t class = loc->class;
	double *sample = samples->samples[class][loc->index];
    size_t dimension = samples->dimension;
	double product = - hyperplane[dimension] 
                     - samples->label[class] * precision;
	for (size_t i = 0; i < dimension; i++) {
		product += sample[i] * hyperplane[i];
	}
    return product;
}

double sample_violation(
        sample_locator_t *loc, 
        samples_t *samples, 
        double *hyperplane, 
		double precision) {
    double dist = distance(loc, samples, hyperplane, precision);
    if (loc->class > 0) {
        dist = -dist;
    }
    return dist;
}


int side(
		sample_locator_t *loc, 
		samples_t *samples, 
		double *hyperplane, 
		double precision) {
    double dist = distance(loc, samples, hyperplane, precision);
    return 0. == dist ? loc->class : dist > 0.;
}

int side_cnt(
		int class, 
		samples_t *samples, 
		double *hyperplane,
		double precision) {
    sample_locator_t loc;
    loc.class = (int) class;
    int positive_cnt = 0;
    size_t count = samples->count[class];
    for (size_t i = 0; i < count; i++) {
        loc.index = i;
        positive_cnt += side(&loc, samples, hyperplane, precision);
    }
    return positive_cnt;
}
