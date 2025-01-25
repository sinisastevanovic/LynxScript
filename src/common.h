#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NAN_BOXING

#ifdef DEBUG
#define DEBUG_PRINT_CODE
//#define DEBUG_TRACE_EXECUTION // TODO: Disable in prod
//#define DEBUG_STRESS_GC
//#define DEBUG_LOG_GC
#endif

#define UINT8_COUNT (UINT8_MAX + 1)