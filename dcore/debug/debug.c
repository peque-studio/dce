#include <dcore/debug.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#define ISATTY(FILE) (isatty(fileno((FILE))))
#else
#define ISATTY(FILE) 1
#endif

static void defaultFatalHandler() {
    exit(1);
}

static DCdFatalHandler fatalHandler;
static size_t sinkCount;
static FILE **sinks;
static size_t contextStackSize;
static DCdContext *contextStack;

void dcdSetFatalHandler(DCdFatalHandler handler) {
    fatalHandler = handler;
}

DCdFatalHandler dcdGetFatalHandler() {
    return fatalHandler;
}

DCdContext *dcdGetCurrentContext() { return &contextStack[contextStackSize - 1]; }
DCdContext *dcdPushContext(const char *name, const char *file, const char *func, int line) {
    dcdMsgF(10000, file, func, line, "[ %s ]", name);
    return dcdPushContextQuiet(name);
}

DCdContext *dcdPushContextQuiet(const char *name) {
    if(contextStack)
        contextStack = realloc(contextStack, sizeof(DCdContext) * ++contextStackSize);
    else
        contextStack = malloc(sizeof(DCdContext) * (contextStackSize = 1));
    dcdGetCurrentContext()->name = name;
    memset(&dcdGetCurrentContext()->stats, 0, sizeof(DCdMsgStats));
    return dcdGetCurrentContext();
}

size_t dcdPopContext(const char *file, const char *func, int line) {
    dcdMsgF(10001, file, func, line, "[ %s ]", dcdGetCurrentContext()->name);
    return dcdPopContextQuiet();
}

size_t dcdPopContextQuiet() {
    if(contextStack == NULL) return 0;
    if(contextStackSize == 1) return 1;
    contextStack = realloc(contextStack, sizeof(DCdContext) * --contextStackSize);
    return contextStackSize;
}

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

static void printSepratator(FILE *sink) {
    for(int j = 0; j < 20 + 54 + 10 + 1; ++j) {
        if(j == 20 || j == 20 + 54) fputc('|', sink);
        else if(j == 20 + 54 + 10) fputc('+', sink);
        else fputc('-', sink);
    }
    fputc('\n', sink);
}

void dcdMsgF(DCdMsgType type, const char *file, const char *func, int line, const char *fmt, ...) {
    // increment stats
    if((int)type < 10000) {
        dcdGetCurrentContext()->stats.total += 1;
        *(&dcdGetCurrentContext()->stats.debug + (size_t)type) += 1;
    }
    if(!sinks) { if(type == DCD_MSG_TYPE_FATAL) fatalHandler(); return; }

    time_t timer;
    char timeString[26];
    struct tm* tmInfo;
    timer = time(NULL);
    tmInfo = localtime(&timer);
    strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", tmInfo);

    for(int i = 0; i < sinkCount; ++i) {
        // TODO: Configure separator length (also whether it exists or not) and char
        if((int)type == 10000) printSepratator(sinks[i]);
        fprintf(sinks[i], "%s%s | %25s:%-3d @%-20s | %7s | ",
            (!ISATTY(sinks[i]) || (int)type >= 10000) ? "" : messageColor[type], timeString,
            file, line, func,
            (int)type >= 10000 ? "" : messageTypePrefix[type]
        );
        
        // indentation
        // TODO: different indent size
        for(int j = 0; j < contextStackSize - 1 - ((int)type == 10001 ? 1 : 0); ++j) {
            fputs("  ", sinks[i]);
        }

        va_list va;
        va_start(va, fmt);
        vfprintf(sinks[i], fmt, va);
        va_end(va);

        if(ISATTY(sinks[i])) fputs("\033[m\n", sinks[i]);
    }

    if(type == DCD_MSG_TYPE_FATAL) fatalHandler();
}

void dcdInit(const char *name) {
    fatalHandler = &defaultFatalHandler;
    sinkCount = 1;
    sinks = malloc(sinkCount * sizeof(FILE*));
    sinks[0] = stdout;
    contextStack = NULL;
    contextStackSize = 0;
    dcdPushContextQuiet(name);
}

void dcdAddSink(FILE *sink) {
    sinks = realloc(sinks, ++sinkCount * sizeof(FILE*));
    sinks[sinkCount - 1] = sink;
}

void dcdRemoveSink(FILE *sink) {
    size_t pos = 0;
    for(; pos <= sinkCount; ++pos)
        if(pos < sinkCount && sinks[pos] == sink) break;
    
    if(pos == sinkCount) {
        DCD_WARNING("Tried to remove non-existing sink!");
        return;
    }
    
    for(size_t i = pos + 1; i < sinkCount; ++i)
        sinks[i - 1] = sinks[i];
    
    sinks = realloc(sinks, --sinkCount * sizeof(FILE*));
}

void dcdDeInit() {
    for(int i = 0; i < sinkCount; ++i)
        printSepratator(sinks[i]);
    
    if(sinks != NULL) free(sinks);
    if(contextStack != NULL) free(contextStack);
}
