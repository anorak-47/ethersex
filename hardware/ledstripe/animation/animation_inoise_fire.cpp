
#include "Arduino.h"
#include "animation_inoise_fire.h"

/* inoise8_fire
 *
 * By: Andrew Tuline
 *
 * Date: January, 2017
 *
 * This super short sketch (just two lines to the algorithm) displays fire thanks to FastLED's Perlin Noise function and Palettes.
 *
 * It needs some tweaking in order to work across a wide range of NUM_LED values, but looks pretty good at 60.
 *
 */

namespace fastled
{

AnimationInoiseFire::AnimationInoiseFire(CRGB *leds, uint16_t led_count, animation *animation_info)
    : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationInoiseFire::~AnimationInoiseFire()
{
}

void AnimationInoiseFire::initialize()
{
    currentPalette = CRGBPalette16(CRGB::Black, CRGB::Black, CRGB::Black, CHSV(0, 255, 4), CHSV(0, 255, 8), CRGB::Red, CRGB::Red, CRGB::Red,
                                   CRGB::DarkOrange, CRGB::Orange, CRGB::Orange, CRGB::Orange, CRGB::Yellow, CRGB::Yellow, CRGB::Gray, CRGB::Gray);
}

bool AnimationInoiseFire::loop()
{
    // I am the god of hell fire and I bring you . . .

    for (int i = 0; i < _led_count; i++)
    {
        index = inoise8(i * xscale, millis() * yscale * _led_count / 255); // X location is constant, but we move along the Y at the rate of millis()
        _leds[i] = ColorFromPalette(currentPalette, min(i * (index) >> 6, 255), i * 255 / _led_count,
                                   LINEARBLEND); // With that value, look up the 8 bit colour palette value and assign it to the current LED.
    }

    return true;
}

} /* namespace fastled */
