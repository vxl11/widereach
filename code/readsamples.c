#include "widereach.h"
#include "helper.h"

double *read_vector(FILE *infile, size_t dimension) {
    double *sample = CALLOC(dimension, double);
    for (size_t i = 0; i < dimension; i++) {
        if (fscanf(infile, "%lg", sample+i) != 1) {
            exit(EXIT_FAILURE);
        }
    }
    return sample;
}

void read_class(FILE *infile, samples_t *samples, int class) {
    size_t count = samples->count[class];
    double **s = samples->samples[class] = CALLOC(count, double *);
    for (size_t i = 0; i < count; i++) {
        s[i] = read_vector(infile, samples->dimension); 
    }
}

void read_classes(FILE *infile, samples_t *samples) {
    for (int class = 0; class < 2; class++) {
        read_class(infile, samples, class);
    }
}

samples_t *read_binary_samples(FILE *infile) {
    samples_t *samples = CALLOC(1, samples_t);
    
    // Descriptor of binary samples
    samples->class_cnt = 2;
    int *label = samples->label = CALLOC(2, int);
    label[0] = -1;
    label[1] = 1;
    samples->samples = CALLOC(2, double **);
    
    size_t *count = samples->count = CALLOC(2, size_t);
    if (fscanf(infile, "%lu %lu %lu", 
               &(samples->dimension), &(count[0]), &(count[1])) != 3) {
        exit(EXIT_FAILURE);
    }
    read_classes(infile, samples);
    
    return samples;
}
