#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationRainbowBeat : public LedStripeAnimation
{
public:
    AnimationRainbowBeat(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationRainbowBeat();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
};

} /* namespace fastled */
