#include <dcore/common.h>
#include <tests/test.h>
#include <string.h>
#include <stdlib.h>

static DCtTest *tests;
static size_t testCount;

void dctRegisterTest_(DCtTest test) {
	if(tests == NULL)
		tests = malloc(sizeof(DCtTest) * (testCount = 1));
	else
		tests = realloc(tests, ++testCount * sizeof(DCtTest));
	memcpy(&tests[testCount - 1], &test, sizeof(DCtTest));
}

size_t dctGetTests(DCtTest **tests_) {
	if(tests) *tests_ = tests;
	return testCount;
}
