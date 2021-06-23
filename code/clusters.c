#include "widereach.h"
#include "helper.h"

/* ---------------------- Sample clusters --------------------------- */

clusters_info_t *delete_clusters_info(clusters_info_t *info) {
  free(info->count);
  free(info->shift);
  free(info->side);
  return info;
}

clusters_info_t *new_clusters_info_singleton(size_t count, size_t dimension) {
  clusters_info_t *info = CALLOC(1, clusters_info_t);
  info->cluster_cnt = 1;
  info->count = CALLOC(1, size_t);
  info->count[0] = 1;
  info->shift = CALLOC(1, double);
  info->shift[0] = 0.;
  info->side = CALLOC(1, double);
  info->side[0] = 1.;
  info->dimension = dimension;
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

size_t clusters_count(clusters_info_t *info) {
  size_t cluster_cnt = info->cluster_cnt;
  size_t *count = info->count;
  size_t count_total = 0;
  for (size_t h = 0; h < cluster_cnt; h++) {
    count_total += count[h];
  }
  return count_total;
}


double **random_point_clusters(clusters_info_t *info) {
  double **samples = CALLOC(clusters_count(info), double *);
  
  size_t cluster_cnt = info->cluster_cnt;
  size_t *count = info->count;
  size_t dimension = info->dimension;
  double *shift = info->shift;
  double *side = info->side;
  
  double **samples_pointer = samples;
  for (size_t h = 0; h < cluster_cnt; h++) {
    samples_pointer = 
      random_point_cluster(
        count[h], 
        dimension, 
        shift[h], 
        side[h], 
        samples_pointer);
  }
  
  return samples;
}

void set_sample_class_clusters(
		samples_t *samples, 
		size_t class, 
		int label, 
		clusters_info_t *info) {
	samples->label[class] = label;
	samples->count[class] = clusters_count(info);
	samples->samples[class] = random_point_clusters(info);
}
