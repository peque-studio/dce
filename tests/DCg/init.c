#include <dcore/common.h>
#include <dcore/debug.h>
#include <dcore/graphics.h>
#include <stdio.h>

void fatalHandler() {}

int main(int argc, char *argv[]) {
    dcdInit();
    dcdSetFatalHandler(&fatalHandler);
    DCD_MSGF(INFO, "Graphics Init Test");
    {
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
    }
    if(dcdGetMsgStats()->error > 0 || dcdGetMsgStats()->fatal > 0) {
        DCD_MSGF(FATAL, "Graphics Init Test: Failed");
    } else {
        DCD_MSGF(SUCCESS, "Graphics Init Test: Success");
    }
    dcdDeInit();
}
