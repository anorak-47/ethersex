#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationBeatWave : public LedStripeAnimation
{
public:
    static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    virtual ~AnimationBeatWave();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
    AnimationBeatWave(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    void beatwave();

    // Palette definitions
    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    TBlendType currentBlending;
};

} /* namespace fastled */
