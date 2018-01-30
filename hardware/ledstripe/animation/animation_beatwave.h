#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationBeatWave : public LedStripeAnimation
{
public:
    AnimationBeatWave(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationBeatWave();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
    void beatwave();

    // Palette definitions
    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    TBlendType currentBlending;
};

} /* namespace fastled */
