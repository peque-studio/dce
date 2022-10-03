#ifndef DCORE_DEBUG_H
#define DCORE_DEBUG_H
#include <stddef.h> // size_t, can't include dcore/common.h since it includes this header.

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
    size_t total;
    unsigned int debug;
    unsigned int info;
    unsigned int warning;
    unsigned int error;
    unsigned int fatal;
    unsigned int success;
} DCdMsgStats;

typedef struct DCdContext {
    const char *name;
    DCdMsgStats stats;
} DCdContext;

typedef void(*DCdFatalHandler)();

void dcdSetFatalHandler(DCdFatalHandler handler);
DCdFatalHandler dcdGetFatalHandler();

/** returns context from the stack top. @note there always is a context. */
DCdContext *dcdGetCurrentContext();

/** creates new context on top of the stack. @see dcdPushContextQuiet */
DCdContext *dcdPushContext(const char *name, const char *file, const char *func, int line);

#define DCD_PUSH_CONTEXT(NAME) dcdPushContext((NAME), __FILE__, __func__, __LINE__)

/** creates new context on top of the stack without a message. @see dcdPushContext */
DCdContext *dcdPushContextQuiet(const char *name);

/** removes context from top of the stack. @returns the new stack size. @warning doesn't pop last context. */
size_t dcdPopContext(const char *file, const char *func, int line);

#define DCD_POP_CONTEXT() dcdPopContext(__FILE__, __func__, __LINE__)

/** removes context from top of the stack without a message. @returns the new stack size @warning doesn't pop last context. */
size_t dcdPopContextQuiet();

void dcdMsgF(DCdMsgType type, const char *file, const char *func, int line, const char *fmt, ...);

void dcdInit(const char *name);
void dcdDeInit();

#define DCD_MSGF(T, FMT, ...) DCD_EMSGF(DCD_MSG_TYPE_##T, FMT, __VA_ARGS__)
#define DCD_EMSGF(T, FMT, ...) dcdMsgF(T, __FILE__, __func__, __LINE__, FMT __VA_OPT__(,) __VA_ARGS__)
#define DEBUGIF(COND) if(COND)

#define DCD_DEBUG(FMT, ...) DCD_MSGF(DEBUG, FMT, __VA_ARGS__);
#define DCD_INFO(FMT, ...) DCD_MSGF(INFO, FMT, __VA_ARGS__);
#define DCD_SUCCESS(FMT, ...) DCD_MSGF(SUCCESS, FMT, __VA_ARGS__);
#define DCD_WARNING(FMT, ...) DCD_MSGF(WARNING, FMT, __VA_ARGS__);
#define DCD_ERROR(FMT, ...) DCD_MSGF(ERROR, FMT, __VA_ARGS__);
#define DCD_FATAL(FMT, ...) DCD_MSGF(FATAL, FMT, __VA_ARGS__);

#endif
