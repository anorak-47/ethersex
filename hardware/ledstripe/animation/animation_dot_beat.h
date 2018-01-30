#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationDotBeat : public LedStripeAnimation
{
public:
    AnimationDotBeat(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationDotBeat();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
    // Define variables used by the sequences.
    int thisdelay = 10;    // A delay value for the sequence(s)
    uint8_t count = 0;     // Count up to 255 and then reverts to 0
    uint8_t fadeval = 224; // Trail behind the LED's. Lower => faster fade.

    uint8_t bpm = 30;
};

} /* namespace fastled */
