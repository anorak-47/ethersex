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

struct strand_configuration_s
{
    uint8_t id;
    bool autoplay;
    uint32_t autoplay_delay_msecs;
    bool autostart_on_sensor;
    int16_t autostart_sensor_delta;

    animation_configuration_t animations[ANIMATION_COUNT];
};

typedef struct strand_configuration_s strand_configuration_t;

struct animation_state_s
{
    bool is_running;
    bool was_autostarted;

    uint8_t id;
    uint16_t delay_msecs;
    uint16_t loop_timer;
    uint32_t autoplay_timer;

    bool is_autostart_constraint_matching;
    uint16_t sensor_delta;

    fastled::LedStripeAnimation *animation;

    CRGB *leds;
    uint16_t led_count;
};

typedef struct animation_state_s animation_state_t;

extern strand_configuration_t strand_config[MAX_LED_STRIPES];
extern animation_state_t animation_state[MAX_LED_STRIPES];

#ifdef __cplusplus
}
#endif
