#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationMover : public LedStripeAnimation
{
public:
    AnimationMover(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationMover();

    virtual void initialize() override;
    virtual bool loop() override;

protected:
    void mover();
    void ChangeMe();

    uint8_t thisdelay = 20;
    uint8_t thisfade = 192; // How quickly does it fade? Lower = slower fade rate.
};

} /* namespace fastled */
