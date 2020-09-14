#include <stdio.h>

#include "widereach.h"
#include "helper.h"

samples_t *read_binary_samples(FILE *infile) {
    samples_t *samples = CALLOC(1, samples_t);
    
    // Descriptor of binary samples
    samples->class_cnt = 2;
    int *label = samples->label = CALLOC(2, int);
    label[0] = -1;
    label[1] = 1;
    samples->samples = CALLOC(2, double **);
    
    samples->count = CALLOC(2, size_t);
    fscanf(infile, "%lu %lu %lu", 
           &(samples->dimension), &(samples->count[1]), &(samples->count[0])); 
    samples->samples[0] = CALLOC(samples->count[0], double *);
    samples->samples[1] = CALLOC(samples->count[1], double *);
    
    return samples;
}
