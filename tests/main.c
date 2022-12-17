#include <dcore/common.h>
#include <dcore/debug.h>
#include <string.h>
#include <tests/test.h>
#include <stdlib.h>

void fatalHandler() { }

static void showHelp(int argc, char **argv) {
	printf(
	  "usage: %s [options]\n"
	  "runs all tests bundled.\n"
	  "options:\n"
	  "  -h, --help show this help message\n"
	  "  -s         stop after first test fail\n"
	  "  -q         don't show any output from tests\n"
	  "  -v         show additional output (verbose)\n"
	  "  -n         don't show summary\n",
	  argv[0]
	);
}

int main(int argc, char **argv) {
	struct {
		bool stopAtFirstFail;
		bool quiet;
		bool verbose;
		bool noSummary;
	} options = { false, false, false, false };

	for(int i = 1; i < argc; ++i) {
		if(argv[i][0] == '-') {
			if(argv[i][1] == '-') {
				if(strcmp(argv[i] + 2, "help") == 0) {
					showHelp(argc, argv);
					return 0;
				}
			}

			for(int j = 1; argv[i][j]; ++j)
				switch(argv[i][j]) {
				case 's': options.stopAtFirstFail = true; break;
				case 'q': options.quiet = true; break;
				case 'v': options.verbose = true; break;
				case 'n': options.noSummary = true; break;
				case 'h': showHelp(argc, argv); return 0;
				}
		}
	}

	dcdInit("Testing...");
	dcdSetFatalHandler(&fatalHandler);

	DCtTest *tests = NULL;
	size_t testCount = dctGetTests(&tests);

	struct Test {
		DCtTest *test;
		bool failed;
	} *testResults = dcmemAllocate(sizeof(struct Test) * testCount);

	size_t failedTests = 0;

	if(options.quiet) dcdRemoveSink(stdout);
	for(size_t i = 0; i < testCount; ++i) {
		testResults[i].test = &tests[i];
		DCD_PUSH_CONTEXT(tests[i].description);
		DCD_MSGF(INFO, "%s:%d @%s", tests[i].debugInfo.file, tests[i].debugInfo.line, tests[i].debugInfo.func);
		int returnCode = tests[i].function();
		if(dcdGetCurrentContext()->stats.error != 0 || dcdGetCurrentContext()->stats.fatal != 0 || returnCode != 0) {
			DCD_MSGF(FATAL, "\033[m\033[41mTEST FAILED");
			failedTests += 1;
			if(options.stopAtFirstFail) {
				DCD_POP_CONTEXT();
				break;
			}
			testResults[i].failed = true;
		} else {
			DCD_MSGF(SUCCESS, "\033[m\033[42mTEST SUCCEEDED");
			testResults[i].failed = false;
		}
		DCD_POP_CONTEXT();
	}

	if(options.quiet) dcdAddSink(stdout);

	int exitCode = failedTests > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
	if(!options.noSummary) {
		DCD_PUSH_CONTEXT("Summary");

		if(options.stopAtFirstFail && failedTests != 0) {
			DCD_MSGF(ERROR, "Stopped after first failed test.");
		} else {
			for(size_t i = 0; i < testCount; ++i) {
				if(testResults[i].failed) {
					DCD_MSGF(
					  ERROR, "\033[m\033[41mTEST FAILED\033[m %s %s:%d @%s", tests[i].description, tests[i].debugInfo.file, tests[i].debugInfo.line,
					  tests[i].debugInfo.func
					);
				} else {
					DCD_MSGF(SUCCESS, "\033[m\033[42mTEST SUCCEEDED\033[m %s", tests[i].description);
				}
			}

			DCD_MSGF(INFO, "Total: %d/%d tests failed! (%d%% success rate)", failedTests, testCount, 100 - (failedTests * 100) / testCount);
		}

		DCD_POP_CONTEXT();
	}

	dcmemDeallocate(testResults);
	dcdDeInit();
	return exitCode;
}
