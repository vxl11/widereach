#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "widereach.h"
#include "helper.h"

void test_samples() {
	samples_t *samples = random_samples(5, 3, 2);
	CU_ASSERT(is_binary(samples));
}

int main() {
	if (CU_initialize_registry() != CUE_SUCCESS) {
		return EXIT_FAILURE;
	}

	// Samples
	CU_pSuite samples = CU_add_suite("samples", NULL, NULL);
	CU_add_test(samples, "generic samples", test_samples);

	// Run tests
	CU_basic_run_tests();

	CU_cleanup_registry();
	return EXIT_SUCCESS;
}
