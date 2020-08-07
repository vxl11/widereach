#include "widereach.h"

int positive_direction(double x) {
    return x > 0.;
}

int negative_direction(double x) {
    return x < 0.;
}

int vector_count(sparse_vector_t *v, int (*direction)(double)) {
    int len = v->len;
    double *val = v->val;
    int cnt = 0;
    for (int i = 1; i <= len; i++) {
        cnt += direction(val[i]);
    }
    return cnt;
}

double identity(double x, double bnd) {
    return x;
}

double discriminant(double x, double bnd) {
    return x < 0. ? -bnd : bnd;
}

void append_to_rhs(
        sparse_vector_t *rhs, 
        sparse_vector_t *v, 
        double (*map)(double, double), 
        double bnd) {
    int len = v->len;
    int *ind = v->ind;
    double *val = v->val;
    for (int i = 1; i <= len; i++) {
        append(rhs, ind[i], map(val[i], bnd));
    }
}

void interdiction_cut(
        sparse_vector_t *pth, 
        sparse_vector_t *interdicted,
        sparse_vector_t **rhs,
        double *lhs) {
    int bnd = interdicted->len;
    sparse_vector_t *rhs_local = sparse_vector_blank(pth->len + bnd);
    append_to_rhs(rhs_local, interdicted, identity, 0.);
    append_to_rhs(rhs_local, pth, discriminant, bnd);
    *rhs = rhs_local;
    *lhs = 
        bnd * vector_count(pth, positive_direction) - 
        vector_count(interdicted, negative_direction);
}
