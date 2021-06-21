#include "widereach.h"
#include "helper.h"

/* ---------------------- Sample clusters --------------------------- */

cluster_info_t *delete_cluster_info(cluster_info_t *info) {
  for (size_t i = 0; i < info->cluster_cnt; i++) {
    free(info->count[i]);
    free(info->shift[i]);
    free(info->size[i]);
  }
  return info;
}
