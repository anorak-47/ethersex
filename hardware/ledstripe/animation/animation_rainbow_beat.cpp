
#include "Arduino.h"
#include "animation_rainbow_beat.h"

/* rainbow_beat
 *
 * By: Andrew Tuline
 *
 * Date: July, 2015
 *
 * A rainbow moving back and forth using FastLED function calls with no delays or 'for' loops. Very simple, yet effective.
 *
 */

namespace fastled
{

AnimationRainbowBeat::AnimationRainbowBeat(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
    : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationRainbowBeat::~AnimationRainbowBeat()
{
}

static LedStripeAnimation *AnimationRainbowBeat::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
{
    return new AnimationRainbowBeat(leds, led_count, animation_info);
}

void AnimationRainbowBeat::initialize()
{
}

bool AnimationRainbowBeat::loop()
{
    uint8_t beatA = beatsin8(17, 0, 255); // Starting hue
    uint8_t beatB = beatsin8(13, 0, 255);
    fill_rainbow(_leds, _led_count, (beatA + beatB) / 2, 8); // Use FastLED's fill_rainbow routine.
    return true;
}

} /* namespace fastled */
