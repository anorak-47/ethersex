#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationInoiseFire : public LedStripeAnimation
{
public:
    static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    virtual ~AnimationInoiseFire();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
    AnimationInoiseFire(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    CRGBPalette16 currentPalette;

    uint32_t xscale = 20;                                          // How far apart they are
    uint32_t yscale = 3;                                           // How fast they move
    uint8_t index = 0;
};

} /* namespace fastled */
