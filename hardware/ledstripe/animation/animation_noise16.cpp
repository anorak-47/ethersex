
#include "Arduino.h"
#include "animation_noise16.h"

/* noise16_1 noise16_2 noise16_3
 *
 * By: Andrew Tuline
 *
 * Date: January, 2017
 *
 * A 16 bit noise routine with palettes. This was adapted by a routine originally by Stefan Petrick.
 *
 */

namespace fastled
{

AnimationNoise16::AnimationNoise16(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info) : LedStripeAnimation(leds, led_count, animation_info)
{
    currentBlending = LINEARBLEND;
}

AnimationNoise16::~AnimationNoise16()
{
}

static LedStripeAnimation *AnimationNoise16::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
{
    return new AnimationNoise16(leds, led_count, animation_info);
}

void AnimationNoise16::initialize()
{
    currentPalette = PartyColors_p;
}

void AnimationNoise16::setOption(uint8_t option)
{
    _option = option;

    if (option > 2)
        _option = 0;
}

void AnimationNoise16::noise16_1()
{ // moves a noise up and down while slowly shifting to the side

    uint16_t scale = 1000; // the "zoom factor" for the noise

    for (uint16_t i = 0; i < _led_count; i++)
    {

        uint16_t shift_x = beatsin8(5);    // the x position of the noise field swings @ 17 bpm
        uint16_t shift_y = millis() / 100; // the y position becomes slowly incremented

        uint16_t real_x = (i + shift_x) * scale; // the x position of the noise field swings @ 17 bpm
        uint16_t real_y = (i + shift_y) * scale; // the y position becomes slowly incremented
        uint32_t real_z = millis() * 20;         // the z position becomes quickly incremented

        uint8_t noise = inoise16(real_x, real_y, real_z) >> 8; // get the noise data and scale it down

        uint8_t index = sin8(noise * 3); // map LED color based on noise data
        uint8_t bri = noise;

        _leds[i] = ColorFromPalette(currentPalette, index, bri,
                                    LINEARBLEND); // With that value, look up the 8 bit colour palette value and assign it to the current LED.
    }

} // noise16_1()

void AnimationNoise16::noise16_2()
{
    // just moving along one axis = "lavalamp effect"

    uint16_t scale = 1000; // the "zoom factor" for the noise

    for (uint16_t i = 0; i < _led_count; i++)
    {

        uint16_t shift_x = millis() / 10; // x as a function of time
        uint16_t shift_y = 0;

        uint32_t real_x = (i + shift_x) * scale; // calculate the coordinates within the noise field
        uint32_t real_y = (i + shift_y) * scale; // based on the precalculated positions
        uint32_t real_z = 4223;

        uint8_t noise = inoise16(real_x, real_y, real_z) >> 8; // get the noise data and scale it down

        uint8_t index = sin8(noise * 3); // map led color based on noise data
        uint8_t bri = noise;

        _leds[i] = ColorFromPalette(currentPalette, index, bri,
                                    LINEARBLEND); // With that value, look up the 8 bit colour palette value and assign it to the current LED.
    }

} // noise16_2()

void AnimationNoise16::noise16_3()
{ // no x/y shifting but scrolling along

    uint16_t scale = 1000; // the "zoom factor" for the noise

    for (uint16_t i = 0; i < _led_count; i++)
    {

        uint16_t shift_x = 4223; // no movement along x and y
        uint16_t shift_y = 1234;

        uint32_t real_x = (i + shift_x) * scale; // calculate the coordinates within the noise field
        uint32_t real_y = (i + shift_y) * scale; // based on the precalculated positions
        uint32_t real_z = millis() * 2;          // increment z linear

        uint8_t noise = inoise16(real_x, real_y, real_z) >> 7; // get the noise data and scale it down

        uint8_t index = sin8(noise * 3); // map led color based on noise data
        uint8_t bri = noise;

        _leds[i] = ColorFromPalette(currentPalette, index, bri,
                                    LINEARBLEND); // With that value, look up the 8 bit colour palette value and assign it to the current LED.
    }

} // noise16_3()

bool AnimationNoise16::loop()
{
    EVERY_N_MILLISECONDS(50)
    {
        nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges); // Blend towards the target palette
    }

    EVERY_N_SECONDS(5)
    {
        // Change the target palette to a random one every 5 seconds.
        targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)),
                                      CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
    }

    switch (_option)
    {
    case 0:
        noise16_1();
        break;
    case 1:
        noise16_2();
        break;
    case 2:
        noise16_3();
        break;
    }

    return true;
}

} /* namespace fastled */
