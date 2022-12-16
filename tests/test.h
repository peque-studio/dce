#ifndef DCORE_TESTS_H
#define DCORE_TESTS_H
#include <dcore/common.h>

// TODO: cross-platform (VC)

typedef struct DCtTest {
	const char *description;
	struct DebugInfo {
		const char *file, *func;
		int line;
	} debugInfo;
	int (*function)();
} DCtTest;

size_t dctGetTests(DCtTest **tests);

#define DCT_ASSERT(EXPR, MSG) \
	if(!(EXPR)) { \
		DC_ASSERT_FALSE(EXPR, MSG); \
		return 1; \
	} else { \
		DCD_MSGF(SUCCESS, "Assertion passed: %s", #EXPR); \
	}

// Registers a test
// clang-format off
#define DCT_TEST(NAME, DESC) \
	void NAME##__DCtCtor() __attribute__((constructor)); \
	int NAME(); \
	void NAME##__DCtCtor() { \
		extern void dctRegisterTest_(DCtTest test); \
		dctRegisterTest_((DCtTest){ \
		  (DESC), {__FILE__, #NAME, __LINE__}, &(NAME) }); } \
int NAME()
// clang-format on

#endif
