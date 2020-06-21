#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "widereach.h"
#include "helper.h"

int init_samples() {
	srand48(20200620);
	return 0;
}

void test_samples() {
	samples_t *samples = random_samples(5, 3, 2);
	CU_ASSERT(is_binary(samples));
	CU_ASSERT_EQUAL(samples->dimension, 2);
	CU_ASSERT_EQUAL(samples_total(samples), 5);
	CU_ASSERT_EQUAL(positives(samples), 3);
	for (size_t k = 0; k < 2; k++) {
		for (size_t i = 0; i < samples->count[k]; i++) {
			for (size_t j = 0; j < 2; j++) {
				CU_ASSERT(samples->samples[k][i][j] >= 0.);
				CU_ASSERT(samples->samples[k][i][j] <= 1.);
			}
		}
	}
	free(delete_samples(samples));
}


void test_sparse_vector() {
	double ary[] = { 3., 2., 1. };
	sparse_vector_t *v = to_sparse(3, ary, 2);
	for (int i = 1; i <= 3; i++) {
          CU_ASSERT_EQUAL(v->ind[i], i);
	  CU_ASSERT_DOUBLE_EQUAL(v->val[i], (double) (4 - i), 1e-12);
	}

	multiply(v, -1.);
	for (int i = 1; i <= 3; i++) {
          CU_ASSERT_EQUAL(v->ind[i], i);
	  CU_ASSERT_DOUBLE_EQUAL(v->val[i], (double) -(4 - i), 1e-12);
	}

	CU_ASSERT_EQUAL(append(v, 6, 6.), 1);
        CU_ASSERT_EQUAL(v->ind[4], 6);
	CU_ASSERT_DOUBLE_EQUAL(v->val[4], 6., 1e-12);

	free(delete_sparse_vector(v));

	v = sparse_vector_blank(3);
        CU_ASSERT_EQUAL(v->len, 0);
        CU_ASSERT_EQUAL(v->extra, 3);
	free(delete_sparse_vector(v));
}

void test_env() {
	env_t env;
	env.samples = random_samples(5, 3, 2);
	delete_env(&env);
}


void test_indexing() {
	samples_t *samples = random_samples(5, 3, 2);
	CU_ASSERT_EQUAL(idx(0, 1, 1, samples), 5);
	CU_ASSERT_EQUAL(idx(1, 1, 1, samples), 2);
	CU_ASSERT_EQUAL(idx(0, 0, 1, samples), 8);
	CU_ASSERT_EQUAL(idx(1, 0, 1, samples), 5);
	CU_ASSERT_EQUAL(violation_idx(0, samples), 9);
	CU_ASSERT_EQUAL(violation_idx(1, samples), 6);
	CU_ASSERT_EQUAL(idx_extreme(0, 1, 0, samples), 4);
	CU_ASSERT_EQUAL(idx_extreme(0, 1, 1, samples), 6);
	CU_ASSERT_EQUAL(idx_extreme(0, 0, 0, samples), 7);
	CU_ASSERT_EQUAL(idx_extreme(0, 0, 1, samples), 8);
	CU_ASSERT_EQUAL(idx_extreme(1, 1, 0, samples), 1);
	CU_ASSERT_EQUAL(idx_extreme(1, 1, 1, samples), 3);
	CU_ASSERT_EQUAL(idx_extreme(1, 0, 0, samples), 4);
	CU_ASSERT_EQUAL(idx_extreme(1, 0, 1, samples), 5);
}

int main() {
	if (CU_initialize_registry() != CUE_SUCCESS) {
		return EXIT_FAILURE;
	}

	// Samples
	CU_pSuite samples = CU_add_suite("samples", init_samples, NULL);
	CU_add_test(samples, "generic samples", test_samples);

	// Sparse vectors
	CU_pSuite sparse = CU_add_suite("sparse vector", NULL, NULL);
	CU_add_test(sparse, "generic sparse vector", test_sparse_vector);
	
	// Environment
	CU_pSuite env = CU_add_suite("environment", init_samples, NULL);
	CU_add_test(env, "environment", test_env);

	// Indexing
	CU_pSuite indexing = CU_add_suite("indexing", init_samples, NULL);
	CU_add_test(indexing, "indexing", test_indexing);

	// Run tests
	CU_basic_run_tests();

	CU_cleanup_registry();
	return EXIT_SUCCESS;
}
