#include "widereach.h"
#include "helper.h"

/* ---------------------- Sample clusters --------------------------- */

clusters_info_t *delete_clusters_info(clusters_info_t *info) {
  free(info->count);
  free(info->shift);
  free(info->side);
  return info;
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
