#include <dcore/debug.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void defaultFatalHandler() {
    exit(1);
}

static DCdFatalHandler fatalHandler;
static size_t sinkCount;
static FILE **sinks;
static DCdMsgStats stats;

void dcdSetFatalHandler(DCdFatalHandler handler) {
    fatalHandler = handler;
}

DCdFatalHandler dcdGetFatalHandler() {
    return fatalHandler;
}

DCdMsgStats *dcdGetMsgStats() { return &stats; }

static const char *messageTypePrefix[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
    "SUCCESS"
};

static const char *messageColor[] = {
    "\033[m",
    "\033[34m",
    "\033[33m",
    "\033[31m",
    "\033[1;31m",
    "\033[32m"
};

void dcdMsgF(DCdMsgType type, const char *file, const char *func, int line, const char *fmt, ...) {
    time_t timer;
    char timeString[26];
    struct tm* tmInfo;

    timer = time(NULL);
    tmInfo = localtime(&timer);

    strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", tmInfo);

    *(&stats.debug + (size_t)type) += 1;

    for(int i = 0; i < sinkCount; ++i) {
        fprintf(sinks[i], "%s%s | %s:%d (%s) | %s | ",
            messageColor[type], timeString,
            file, line, func,
            messageTypePrefix[type]
        );
        va_list va;
        va_start(va, fmt);
        vfprintf(sinks[i], fmt, va);
        va_end(va);
        fputs("\033[m\n", sinks[i]);
    }
}

void dcdInit() {
    fatalHandler = &defaultFatalHandler;
    sinkCount = 1;
    sinks = malloc(sinkCount * sizeof(FILE**));
    sinks[0] = stdout;
}

void dcdDeInit() {
    if(sinks != NULL) free(sinks);
}
