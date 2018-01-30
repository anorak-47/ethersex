#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationLittle : public LedStripeAnimation
{
public:
    AnimationLittle(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationLittle();

    virtual bool loop() override;
    virtual void setOption(uint8_t option) override;

protected:

    void rainbow();
    void rainbowWithGlitter();
    void addGlitter( fract8 chanceOfGlitter);
    void confetti();
    void sinelon();
    void bpm();
    void juggle();

    uint8_t _option = 0;
	uint8_t _startIndex = 0;
	uint8_t gHue = 0; // rotating "base color" used by many of the patterns
};

} /* namespace fastled */
