#include <dcore/common.h>
#include <dcore/debug.h>
#include <dcore/graphics.h>
#include <tests/test.h>
#include <stdio.h>

DCT_TEST(graphicsInit, "graphics initialization test") {
    DCD_MSGF(DEBUG, "Creating State");
    DCgState *state = dcgNewState();
    DCD_MSGF(DEBUG, "Initializing State");
    dcgInit(state, 1, "DCE Tests");

    dcgClose(state);
    while(!dcgShouldClose(state)) {
        dcgUpdate(state);
    }

    DCD_MSGF(DEBUG, "DeInitializing State");
    dcgDeinit(state);
    DCD_MSGF(DEBUG, "Freeing State");
    dcgFreeState(state);
    
    return 0;
}

DCT_TEST(assertionTest, "test that won't fail!") {
    DCT_ASSERT(0 == 0, "check for sanity");
    DCT_ASSERT(1 == 1, "check for consistency");
    DCT_ASSERT(1 != 0, "check for insanity but inverted");
    return 0;
}

DCT_TEST(failTest, "test that will fail :(") {
    DCT_ASSERT(0 == 0, "check for sanity");
    DCT_ASSERT(1 == 0, "check for inconsistency");
    DCT_ASSERT(1 == 0, "check for insanity but not inverted");
    return 1;
}
