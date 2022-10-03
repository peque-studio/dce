#ifndef DCORE_COMMON_H
#define DCORE_COMMON_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <dcore/debug.h>

#define DCE_VERSION_MAJOR 0
#define DCE_VERSION_MINOR 0
#define DCE_VERSION_PATCH 1

#define ARRAYSIZE(A) (sizeof(A)/sizeof(0[A]))

#define DC_ASSERT_FALSE(EXPR, MSG) { DCD_MSGF(FATAL, "Assertion failed: %s", #EXPR); \
    DCD_MSGF(FATAL, "  %s", MSG); } // TODO: DCD_TABSTRING
#define DC_ASSERT(EXPR, MSG) { if(!(EXPR)) DC_ASSERT_FALSE(EXPR, MSG); }

#endif
