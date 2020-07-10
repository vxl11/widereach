#include "widereach.h"

int has_add_error(child_data_t *child_data, int node) {
    int child_cnt = child_data->child_cnt;
    return (child_cnt >= 2 || 
        (child_cnt == 1 && child_data->child[0] == node));
}

void sort_children(child_data_t *child_data) {
    int *child = child_data->child;
    if (2 == child_data->child_cnt && child[0] > child[1]) {
        int tmp = child[0];
        child[0] = child[1];
        child[1] = tmp;
    }
}
    

int add_child(child_data_t *child_data, int node) {
    if (has_add_error(child_data, node)) {
        return -1;
    }
    child_data->child[(child_data->child_cnt)++] = node;
    sort_children(child_data);
    return 1;
}
