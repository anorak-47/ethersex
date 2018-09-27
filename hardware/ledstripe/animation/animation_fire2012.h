#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationFire2012 : public LedStripeAnimation
{
public:
    static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    virtual ~AnimationFire2012();

    virtual bool loop() override;
    virtual void setOption(uint8_t option) override;

protected:
    AnimationFire2012(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    void Fire2012WithPalette();

    // Array of temperature readings at each simulation cell
    uint8_t *heat;

    bool gReverseDirection = false;
    CRGBPalette16 gPal;
};

} /* namespace fastled */
