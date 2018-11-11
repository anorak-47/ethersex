#pragma once

#define FASTLED_INTERNAL
#include "animation_config.h"
#include "animation_names.h"
#include "FastLED.h"
#include <inttypes.h>
#include <avr/eeprom.h>

namespace fastled
{
class LedStripeAnimation;
}

#ifdef __cplusplus
extern "C"
{
#endif

#define FPS_TO_DELAY(fps) (fps ? (1000 / fps) : 1)
#define DELAY_TO_FPS(delay) (delay ? (1000 / delay) : 1)

#define SENSOR_IDX_CURRENT 0
#define SENSOR_IDX_REF 1

    struct animation_configuration_s
    {
        uint8_t fps;
        uint8_t option[2];
        uint8_t sensor_index[2];
        CHSV hsv[2];
    };

    typedef struct animation_configuration_s animation_configuration_t;

    struct led_stripe_animation_status_s
    {
        uint8_t current_animation;
        uint8_t sensor_animation;
        bool is_current_animation_running_before_switching;
        uint8_t old_current_animation;
        bool autoplay;
        uint32_t autoplay_delay_msecs;
        bool is_autoswitch_sensor_animation;
        animation_configuration_t animations[ANIMATION_COUNT];
    };

    typedef struct led_stripe_animation_status_s led_stripe_animation_status_t;

    struct led_stripe_animation_s
    {
        bool is_running;
        uint8_t active_animation;
        uint16_t delay_msecs;
        uint16_t loop_timer;
        uint32_t autoplay_timer;

        CRGB *leds;
        uint16_t led_count;
        fastled::LedStripeAnimation *animation;
    };

    typedef struct led_stripe_animation_s led_stripe_animation_t;

    extern led_stripe_animation_status_t led_stripe_status[MAX_LED_STRIPES];
    extern led_stripe_animation_t led_stripe[MAX_LED_STRIPES];

#ifdef __cplusplus
}
#endif
