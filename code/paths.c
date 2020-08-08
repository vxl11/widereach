#include "widereach.h"

int are_consistent(int *index, double *a, double *b) {
    int loc;    
    for (int j = 0; index[j] != 0; j++) {
        loc = index[j];
        if (a[loc] != b[loc]) {
            return 0;
        }
    }
    return 1;
}

int is_path_consistent(sparse_vector_t *path, double *a) {
    for (int j = 1; j <= path->len; j++) {
        /*
        glp_printf("opt(%i)=%g vs %g\n",
                   path->ind[j], a[path->ind[j]], path->val[j]);
                   */
        if (a[path->ind[j]] != path->val[j]) {
            return 0;
        }
    }
    return 1;
}

int consistency_count(sparse_vector_t *path, double *a) {
    int cnt = 0;
    for (int j = 1; j <= path->len; j++) {
        cnt += (a[path->ind[j]] == path->val[j]);
    }
    return cnt;
}
