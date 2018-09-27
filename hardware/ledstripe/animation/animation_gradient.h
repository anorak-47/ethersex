#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationGradient : public LedStripeAnimation
{
public:
    static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    virtual ~AnimationGradient();

    virtual bool loop() override;
    virtual void setOption(uint8_t option) override;
    virtual void update() override;

protected:
    AnimationGradient(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    uint8_t _option = 0;
    uint8_t _startIndex = 0;
    bool _changed = false;
    CRGBPalette16 _palette;
};

} /* namespace fastled */
