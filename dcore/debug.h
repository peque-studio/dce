#ifndef DCORE_DEBUG_H
#define DCORE_DEBUG_H

void dcdInit();
void dcdDeInit();

typedef enum DCdMsgType {
    DCD_MSG_TYPE_DEBUG,
    DCD_MSG_TYPE_INFO,
    DCD_MSG_TYPE_WARNING,
    DCD_MSG_TYPE_ERROR,
    DCD_MSG_TYPE_FATAL,
    DCD_MSG_TYPE_SUCCESS,
} DCdMsgType;

typedef struct DCdMsgStats {
    unsigned int debug;
    unsigned int info;
    unsigned int warning;
    unsigned int error;
    unsigned int fatal;
    unsigned int success;
} DCdMsgStats;

typedef void(*DCdFatalHandler)();

void dcdSetFatalHandler(DCdFatalHandler handler);
DCdFatalHandler dcdGetFatalHandler();
DCdMsgStats *dcdGetMsgStats();
void dcdMsgF(DCdMsgType type, const char *file, const char *func, int line, const char *fmt, ...);

void dcdInit();
void dcdDeInit();

#define DCD_MSGF(T, FMT, ...) dcdMsgF(DCD_MSG_TYPE_##T, __FILE__, __func__, __LINE__, FMT __VA_OPT__(,) __VA_ARGS__)
#define DEBUGIF(COND) if(COND)

#define DCD_FATAL(FMT, ...) { DCD_MSGF(FATAL, FMT, __VA_ARGS__); dcdGetFatalHandler()(); }

#endif
