
#include "Arduino.h"
#include "animation_dot_beat.h"
#include "colorutils.h"

/* dots
 *
 * By: John Burroughs
 *
 * Modified by: Andrew Tuline
 *
 * Date: July, 2015
 *
 * Similar to dots by John Burroughs, but uses the FastLED beatsin8() function instead.
 *
 */

namespace fastled
{

AnimationDotBeat::AnimationDotBeat(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info) : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationDotBeat::~AnimationDotBeat()
{
}

LedStripeAnimation *AnimationDotBeat::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
{
    return new AnimationDotBeat(leds, led_count, animation_info);
}

void AnimationDotBeat::initialize()
{
}

bool AnimationDotBeat::loop()
{
#if 0
    uint8_t inner = beatsin8(bpm, _led_count / 4, _led_count / 4 * 3);  // Move 1/4 to 3/4
    uint8_t outer = beatsin8(bpm, 0, _led_count - 1);                   // Move entire length
    uint8_t middle = beatsin8(bpm, _led_count / 3, _led_count / 3 * 2); // Move 1/3 to 2/3
    // TODO: use precalculates values here!
#else
    uint8_t inner = beatsin16(bpm, _led_count / 4, _led_count / 4 * 3);  // Move 1/4 to 3/4
    uint8_t outer = beatsin16(bpm, 0, _led_count - 1);                   // Move entire length
    uint8_t middle = beatsin16(bpm, _led_count / 3, _led_count / 3 * 2); // Move 1/3 to 2/3
    // TODO: use precalculates values here!
#endif

    _leds[middle] = CRGB::Purple;
    _leds[inner] = CRGB::Blue;
    _leds[outer] = CRGB::Aqua;

    nscale8(_leds, _led_count, fadeval); // Fade the entire array. Or for just a few LED's, use  nscale8(&leds[2], 5, fadeval);

    return true;
}

} /* namespace fastled */
