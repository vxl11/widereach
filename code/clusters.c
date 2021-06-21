#include "widereach.h"
#include "helper.h"

/* ---------------------- Sample clusters --------------------------- */

clusters_info_t *delete_cluster_info(clusters_info_t *info) {
  for (size_t i = 0; i < info->cluster_cnt; i++) {
    free(info->count[i]);
    free(info->shift[i]);
    free(info->size[i]);
  }
  return info;
}
