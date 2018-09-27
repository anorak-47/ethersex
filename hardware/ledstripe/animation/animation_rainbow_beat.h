#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationRainbowBeat : public LedStripeAnimation
{
public:
    static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    virtual ~AnimationRainbowBeat();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
    AnimationRainbowBeat(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
};

} /* namespace fastled */
