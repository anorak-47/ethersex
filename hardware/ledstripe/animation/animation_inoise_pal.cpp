
#include "Arduino.h"
#include "animation_inoise_pal.h"
#include "colorutils.h"

/* inoise8_pal_demo
 *
 * By: Andrew Tuline
 *
 * Date: August, 2016
 *
 * This short sketch demonstrates some of the functions of FastLED, including:
 *
 * Perlin noise
 * Palettes
 * Palette blending
 * Alternatives to blocking delays
 * Beats (and not the Dr. Dre kind, but rather the sinewave kind)
 *
 * Refer to the FastLED noise.h and lib8tion.h routines for more information on these functions.
 *
 */

namespace fastled
{

AnimationInoisePal::AnimationInoisePal(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info) : LedStripeAnimation(leds, led_count, animation_info)
{
    currentPalette = CRGBPalette16(CRGB::Black);
    targetPalette = OceanColors_p;
}

AnimationInoisePal::~AnimationInoisePal()
{
}

static LedStripeAnimation *AnimationInoisePal::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
{
    return new AnimationInoisePal(leds, led_count, animation_info);
}

void AnimationInoisePal::initialize()
{
    dist = random16(12345); // A semi-random number for our noise generator
}

void AnimationInoisePal::fillnoise8()
{
    for (int i = 0; i < _led_count; i++)
    {                                                                 // Just ONE loop to fill up the LED array as all of the pixels change.
        uint8_t index = inoise8(i * xscale, dist + i * yscale) % 255; // Get a value from the noise function. I'm using both x and y axis.
        _leds[i] = ColorFromPalette(currentPalette, index, 255,
                                    LINEARBLEND); // With that value, look up the 8 bit colour palette value and assign it to the current LED.
    }

    dist += beatsin8(10, 1, 4); // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
                                // In some sketches, I've used millis() instead of an incremented counter. Works a treat.
}

bool AnimationInoisePal::loop()
{
    uint8_t maxChanges = 24;
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges); // AWESOME palette blending capability.
    fillnoise8();                                                          // Update the LED array with noise at the new location

    EVERY_N_SECONDS(5)
    { // Change the target palette to a random one every 5 seconds.
        targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)),
                                      CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
    }

    return true;
}

} /* namespace fastled */
