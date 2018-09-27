
#include "Arduino.h"
#include "animation_mover.h"

/* mover
By: Andrew Tuline
Date: February 2015
This is a simple pixel moving routine for those of you that just like to count pixels on a strand.
I prefer to use sine waves as they provide a LOT of flexibility with less code for moving pixels around.
You should be able to add more variables, such as hues, hue rotation, direction and so forth.
*/

namespace fastled
{

AnimationMover::AnimationMover(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info) : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationMover::~AnimationMover()
{
}

LedStripeAnimation *AnimationMover::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
{
    return new AnimationMover(leds, led_count, animation_info);
}

void AnimationMover::initialize()
{
}

void AnimationMover::mover()
{
    static uint8_t hue = 0;
    for (int i = 0; i < _led_count; i++)
    {
        fadeToBlackBy(_leds, _led_count, thisfade); // Low values = slower fade.
        _leds[i] += CHSV(hue, 255, 255);
        _leds[(i + 5) % _led_count] += CHSV(hue + 85, 255, 255);   // We use modulus so that the location is between 0 and _led_count
        _leds[(i + 10) % _led_count] += CHSV(hue + 170, 255, 255); // Same here.
    }
}

void AnimationMover::ChangeMe()
{
    // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.
    uint8_t secondHand = (millis() / 1000) % 15; // IMPORTANT!!! Change '15' to a different value to change duration of the loop.
    static uint8_t lastSecond = 99;              // Static variable, means it's only defined once. This is our 'debounce' variable.
    if (lastSecond != secondHand)
    { // Debounce to make sure we're not repeating an assignment.
        lastSecond = secondHand;
        switch (secondHand)
        {
        case 0:
            thisdelay = 20;
            thisfade = 240;
            break; // You can change values here, one at a time , or altogether.
        case 5:
            thisdelay = 50;
            thisfade = 128;
            break;
        case 10:
            thisdelay = 100;
            thisfade = 64;
            break; // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
        case 15:
            break;
        }
    }
}

bool AnimationMover::loop()
{
    ChangeMe();
    mover();
    return true;
}

} /* namespace fastled */
