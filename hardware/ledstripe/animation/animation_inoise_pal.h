#pragma once

#include "led_stripe_animation.h"
#include "colorpalettes.h"

namespace fastled
{

class AnimationInoisePal : public LedStripeAnimation
{
public:
    static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    virtual ~AnimationInoisePal();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
    AnimationInoisePal(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    void fillnoise8();

    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    //TBlendType currentBlending; // NOBLEND or LINEARBLEND

    int16_t dist = 0; // A random number for our noise generator.
    uint16_t xscale = 30;    // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
    uint16_t yscale = 30;    // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
    uint8_t maxChanges = 24; // Value for blending between palettes.
};

} /* namespace fastled */
