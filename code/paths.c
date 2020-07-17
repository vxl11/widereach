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
        if (a[path->ind[j]] != path->val[j]) {
            return 0;
        }
    }
    return 1;
}
