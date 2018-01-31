#pragma once

#include <stdio.h>
#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "core/debug.h"
#include "animation/animation_config.h"

#if DEBUG_OUTPUT_SUPPORTED
#define LV_(s, args...) debug_printf(s "\n", args)
#define LS_(s) debug_printf(s "\n")
#define L__(s) debug_printf(#s "\n")
#else
#define LV_(s, args...) do {} while(0) xxx
#define LS_(s) do {} while(0) xxx
#define L__(s) do {} while(0)
#endif

#ifdef __cplusplus
}
#endif
