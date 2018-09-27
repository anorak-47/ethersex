
#include "Arduino.h"
#include "animation_inoise_mover.h"

/* inoise8_mover
 *
 * By: Andrew Tuline
 *
 * Date: February, 2017
 *
 * We've used sine waves and counting to move pixels around a strand. In this case, I'm using Perlin Noise to move a pixel up and down the strand.
 *
 * The advantage here is that it provides random natural movement without requiring lots of fancy math by joe programmer.
 *
 */

namespace fastled
{

AnimationInoiseMover::AnimationInoiseMover(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
    : LedStripeAnimation(leds, led_count, animation_info)
{
    currentPalette = LavaColors_p;
    targetPalette = OceanColors_p;
}

AnimationInoiseMover::~AnimationInoiseMover()
{
}

LedStripeAnimation *AnimationInoiseMover::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
{
    return new AnimationInoiseMover(leds, led_count, animation_info);
}

void AnimationInoiseMover::initialize()
{
    dist = random16(12345); // A semi-random number for our noise generator
}

void AnimationInoiseMover::inoise8_mover()
{
    uint8_t locn = inoise8(xscale, dist + yscale) % 255;   // Get a new pixel location from moving noise.
    uint8_t pixlen = map(locn, 0, 255, 0, _led_count - 1); // Map that to the length of the strand.
    _leds[pixlen] = ColorFromPalette(currentPalette, pixlen, 255,
                                     LINEARBLEND); // Use that value for both the location as well as the palette index colour for the pixel.

    dist += beatsin8(10, 1, 4); // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.

} // inoise8_mover()

bool AnimationInoiseMover::loop()
{
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges); // AWESOME palette blending capability.

    inoise8_mover(); // Update the LED array with noise at the new location
    fadeToBlackBy(_leds, _led_count, 4);

    EVERY_N_SECONDS(5)
    {
        // Change the target palette to a random one every 5 seconds.
        targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)),
                                      CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
    }
    return true;
}

} /* namespace fastled */
