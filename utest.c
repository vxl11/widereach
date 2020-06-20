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
	for (size_t k = 0; k < 2; k++) {
		for (size_t i = 0; i < samples->count[k]; i++) {
			for (size_t j = 0; j < 2; j++) {
				printf("%g\n", samples->samples[k][i][j]);
				CU_ASSERT(samples->samples[k][i][j] >= 0.);
				CU_ASSERT(samples->samples[k][i][j] <= 1.);
			}
		}
	}
	delete_samples(samples);
}

int main() {
	if (CU_initialize_registry() != CUE_SUCCESS) {
		return EXIT_FAILURE;
	}

	// Samples
	CU_pSuite samples = CU_add_suite("samples", init_samples, NULL);
	CU_add_test(samples, "generic samples", test_samples);

	// Run tests
	CU_basic_run_tests();

	CU_cleanup_registry();
	return EXIT_SUCCESS;
}
