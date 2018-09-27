#pragma once

#include "animation_config.h"
#include <inttypes.h>
#include <stdbool.h>

#if FASTLED_SUPPORTED

#ifdef __cplusplus
extern "C"
{
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
