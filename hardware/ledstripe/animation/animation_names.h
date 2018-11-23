#pragma once

#include "animation_config.h"
#include "externc.h"
#include <inttypes.h>
#include <stdbool.h>

#if FASTLED_SUPPORTED

EXTERN_C

const char *get_animation_name(uint8_t index);
const char *get_animation_description(uint8_t index);
const char *get_animation_options_description(uint8_t index);
bool animation_has_options_description(uint8_t index);

void create_animation_for_strand(uint8_t strand, uint8_t index);

EXTERN_C_END

#endif
