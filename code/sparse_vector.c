#include "widereach.h"
#include "helper.h"

sparse_vector_t *sparse_vector_blank(size_t extra) {
	sparse_vector_t *v = CALLOC(1, sparse_vector_t);
	v->len = 0;
	v->extra = extra;
	v->ind = CALLOC(extra + 1, int);
	v->val = CALLOC(extra + 1, double);
	return v;
}


sparse_vector_t *delete_sparse_vector(sparse_vector_t *v) {
	v->len = v->extra = 0;
	free(v->ind);
	free(v->val);
	return v;
}

sparse_vector_t *to_sparse(size_t nmemb, double *ary, size_t extra) {
	sparse_vector_t *v = CALLOC(1, sparse_vector_t);
	size_t len = nmemb + extra + 1;
	v->ind = CALLOC(len, int);
	v->val = CALLOC(len, double);
	v->extra = extra;

	v->len = (int) nmemb;
	for (size_t i = 1; i <= nmemb; i++) {
		v->ind[i] = (int) i;
		v->val[i] = ary[i-1];
	}
	return v;
}

int append(sparse_vector_t *v, int ind, double x) {
	if (!v->extra) {
		return -1;
	}
	int idx = v->len + 1;
	v->ind[idx] = ind;
	v->val[idx] = x;
	v->len = idx;
	return (--v->extra);
}


sparse_vector_t *multiply(sparse_vector_t *v, double x) {
	int len = v->len;
	for (int i = 1; i <= len; i++) {
		v->val[i] *= x;
	}
	return v;
}


sparse_vector_t *filter(
        sparse_vector_t *v, 
        int (*predicate)(sparse_vector_t *, int, void*),
        void *info) {
    int len = v->len;
    sparse_vector_t *u = sparse_vector_blank(len + v->extra);
    for (int i = 1; i <= len; i++) {
        if (predicate(v, i, info)) {
            append(u, v->ind[i], v->val[i]);
        }
    }
    return u;
}    
