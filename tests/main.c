#include <dcore/common.h>
#include <dcore/debug.h>
#include <tests/test.h>
#include <stdlib.h>

void fatalHandler() {}

int main() {
    dcdInit("Testing...");
    dcdSetFatalHandler(&fatalHandler);

    DCtTest *tests = NULL;
    size_t testCount = dctGetTests(&tests);

    struct Test {
        DCtTest *test;
        bool failed;
    } *testResults = malloc(sizeof(struct Test) * testCount);

    bool stopAtFirstFail = false;
    size_t failedTests = 0;
    for(size_t i = 0; i < testCount; ++i) {
        testResults[i].test = &tests[i];
        DCD_PUSH_CONTEXT(tests[i].description);
        DCD_MSGF(INFO, "%s:%d @%s", tests[i].debugInfo.file, tests[i].debugInfo.line, tests[i].debugInfo.func);
        int returnCode = tests[i].function();
        if(dcdGetCurrentContext()->stats.error != 0
        || dcdGetCurrentContext()->stats.fatal != 0
        || returnCode != 0) {
            DCD_MSGF(FATAL, "\033[m\033[41mTEST FAILED");
            failedTests += 1;
            if(stopAtFirstFail) {
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

    DCD_PUSH_CONTEXT("Summary");

    int exitCode;
    if(stopAtFirstFail) {
        DCD_MSGF(ERROR, "Stopped after first failed test.");
        exitCode = 1;
    } else {
        for(size_t i = 0; i < testCount; ++i) {
            if(testResults[i].failed) {
                DCD_MSGF(ERROR, "\033[m\033[41mTEST FAILED\033[m %s %s:%d @%s",
                    tests[i].description,
                    tests[i].debugInfo.file, tests[i].debugInfo.line, tests[i].debugInfo.func
                );
            } else {
                DCD_MSGF(SUCCESS, "\033[m\033[42mTEST SUCCEEDED\033[m %s", tests[i].description);
            }
        }

        DCD_MSGF(INFO, "Total: %d/%d tests failed! (%d%% success rate)",
            failedTests, testCount,
            100 - (failedTests * 100) / testCount);
        exitCode = failedTests > 0;
    }

    DCD_POP_CONTEXT();

    dcdDeInit();
    return exitCode;
}
