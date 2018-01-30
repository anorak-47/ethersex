#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationBasicTests : public LedStripeAnimation
{
public:
    AnimationBasicTests(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationBasicTests();

    virtual bool loop() override;
    virtual void setOption(uint8_t option) override;

protected:

    void color_test();
    void blink();
    void mover();

    uint8_t _option = 0;
	uint8_t _startIndex = 0;
};

} /* namespace fastled */
