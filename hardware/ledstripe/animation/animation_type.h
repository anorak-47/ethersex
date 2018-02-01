#pragma once

#define FASTLED_INTERNAL
#include "animation_config.h"
#include "animation_names.h"
#include "FastLED.h"
#include <inttypes.h>
#include <avr/eeprom.h>

namespace fastled {
class LedStripeAnimation;
}

#ifdef __cplusplus
extern "C" {
#endif

#define FPS_TO_DELAY(fps) (fps ? (1000/fps) : 1)
#define DELAY_TO_FPS(delay) (delay ? (1000/delay) : 1)

#define SENSOR_IDX_CURRENT 0
#define SENSOR_IDX_REF 1

struct _s_animation_type
{
    uint8_t fps;
    uint8_t option;
    uint8_t sensor_index[2];
    CHSV hsv[2];
};

typedef struct _s_animation_type animation;


struct _s_led_stripe_animation_status
{
	animation_names current_animation;
	animation_names sensor_animation;
    bool autoplay;
    bool is_autoswitch_sensor_animation;
    animation animations[ANIMATION_COUNT];
};

typedef struct _s_led_stripe_animation_status led_stripe_animation_status;


struct _s_led_stripe_animation
{
    bool is_running;
    animation_names active_animation;
    uint16_t delay_msecs;
    uint16_t loop_timer;

    CRGB *leds;
    uint16_t led_count;
    fastled::LedStripeAnimation *animation;
};

typedef struct _s_led_stripe_animation led_stripe_animation;

extern animation animations[ANIMATION_COUNT];
extern animation EEMEM animations_eeprom[ANIMATION_COUNT];

extern led_stripe_animation_status led_stripe_status[MAX_LED_STRIPES];
extern led_stripe_animation_status EEMEM led_stripe_status_eeprom[MAX_LED_STRIPES];

extern led_stripe_animation led_stripe[MAX_LED_STRIPES];

#ifdef __cplusplus
}
#endif
