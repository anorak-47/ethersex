#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationNoise16 : public LedStripeAnimation
{
public:
    AnimationNoise16(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationNoise16();

    virtual void setOption(uint8_t option) override;
    virtual void initialize() override;
    virtual bool loop() override;

protected:
    void noise16_1();
    void noise16_2();
    void noise16_3();

    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    TBlendType currentBlending; // NOBLEND or LINEARBLEND

    uint8_t maxChanges = 24; // Number of tweens between palettes.
    uint8_t _option = 0;
};

} /* namespace fastled */
