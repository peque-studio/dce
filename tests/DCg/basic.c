#include <dcore/common.h>
#include <dcore/debug.h>
#include <dcore/graphics.h>
#include <dcore/renderers/basic.h>
#include <tests/test.h>

DCT_TEST(basicRendererInit, "basic renderer init test") {
    DCD_MSGF(DEBUG, "Creating State");
    DCgState *state = dcgNewState();
    DCD_MSGF(DEBUG, "Initializing State");
    dcgInit(state, 1, "DCE Tests");

    DCD_DEBUG("dcgBasicRendererCreateInfo");
    dcgBasicRendererCreateInfo(state);

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
