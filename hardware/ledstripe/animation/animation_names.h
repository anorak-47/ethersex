#pragma once

#include <inttypes.h>
#include "animation_config.h"
#include "animation_type.h"

#if FASTLED_SUPPORTED

#define ANIMATION_COUNT 38

#ifdef __cplusplus
extern "C" {
#endif

const char *get_animation_name(uint8_t index);
const char *get_animation_description(uint8_t index);
const char *get_animation_options_description(uint8_t index);
bool animation_has_options_description(uint8_t index);
void animation_set_for_stripe_by_index(uint8_t stripe, uint8_t index);

#ifdef __cplusplus
}
#endif

#endif
