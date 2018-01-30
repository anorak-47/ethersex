#pragma once

#include "led_stripe_animation.h"
#include "colorpalettes.h"


namespace fastled
{

class AnimationInoiseMover : public LedStripeAnimation
{
public:
    AnimationInoiseMover(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationInoiseMover();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
    void inoise8_mover();

    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    //TBlendType currentBlending; // NOBLEND or LINEARBLEND

    int16_t dist = 0;  // A moving location for our noise generator.
    uint16_t xscale = 30; // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
    uint16_t yscale = 30; // Wouldn't recommend changing this on the fly, or the animation will be really blocky.

    uint8_t maxChanges = 24; // Value for blending between palettes.
};

} /* namespace fastled */
