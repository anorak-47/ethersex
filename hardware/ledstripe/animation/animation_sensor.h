#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationSensor : public LedStripeAnimation
{
public:
    AnimationSensor(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationSensor();

    virtual void initialize() override;
    virtual bool loop() override;
    virtual void setOption(uint8_t option) override;
    virtual void update() override;

protected:
    void addGlitter(fract8 chanceOfGlitter);
    void addGlitterAt(uint16_t led, fract8 chanceOfGlitter);

	uint8_t _startIndex = 0;
	uint8_t _option = 0;
	uint8_t _delta = 0;
	bool _changed = false;
	CHSVPalette16 _palette;
	CHSVPalette16 _movingPalette;
    uint16_t _startpos = 0;
    uint16_t _endpos = 0;

};

} /* namespace fastled */
