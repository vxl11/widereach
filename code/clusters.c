#include "widereach.h"
#include "helper.h"

/* ---------------------- Sample clusters --------------------------- */

clusters_info_t *delete_clusters_info(clusters_info_t *info) {
  free(info->count);
  free(info->shift);
  free(info->side);
  return info;
}
