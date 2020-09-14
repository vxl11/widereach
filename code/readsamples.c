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
    double ***s = samples->samples = CALLOC(2, double **);
    
    size_t *count = samples->count = CALLOC(2, size_t);
    fscanf(infile, "%lu %lu %lu", 
           &(samples->dimension), &(count[1]), &(count[0])); 
    for (int class = 0; class < 2; class++) {
        s[class] = CALLOC(count[class], double *);
    }
    
    return samples;
}
