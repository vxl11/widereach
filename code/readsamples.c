#include <stdio.h>

#include "widereach.h"
#include "helper.h"

samples_t *read_binary_samples(FILE *infile) {
    samples_t *samples = CALLOC(1, samples_t);
    samples->class_cnt = 2;
    int *label = samples->label = CALLOC(2, int);
    label[0] = -1;
    label[1] = 1;
    
    /*
    samples->count = CALLOC(2, size_t);
    fscanf(infile, "%u %u %u", 
           &(samples->dimension), &(samples->count[1]), &(samples->count)); 
           */
    
}
