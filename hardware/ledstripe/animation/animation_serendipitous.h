#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationSerendipitous : public LedStripeAnimation
{
public:
    static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    virtual ~AnimationSerendipitous();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
    AnimationSerendipitous(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);

    void serendipitous();

    uint8_t maxChanges = 24; // Value for blending between palettes.

    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    TBlendType currentBlending; // NOBLEND or LINEARBLEND

    uint16_t Xorig = 0x012;
    uint16_t Yorig = 0x015;
    uint16_t X;
    uint16_t Y;
    uint16_t Xn = 0;
    uint16_t Yn = 0;
    uint8_t index = 0;
};

} /* namespace fastled */
