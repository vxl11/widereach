#include "widereach.h"

void delete_samples(size_t n, struct sample_t *samples) {
	for (size_t j = 0; j < n; j++) {
		free(sample[j].values);
	}
	free(samples);
}
