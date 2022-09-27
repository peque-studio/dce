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
} DCdMsgType;

void dcdMsgF(DCdMsgType type, const char *file, const char *func, int line, const char *fmt, ...);

#define DCD_MSGF(T, FMT, ...) dcdMsgF(DCD_MSG_TYPE_##T, __FILE__, __func__, __LINE__, FMT __VA_OPT__(,) __VA_ARGS__)

#endif
