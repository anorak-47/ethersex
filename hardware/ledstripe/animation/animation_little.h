#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationLittle : public LedStripeAnimation
{
public:
    //static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation *animation_info);
    virtual ~AnimationLittle();

    virtual bool loop() override;
    virtual void setOption(uint8_t option) override;

protected:
    AnimationLittle(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);

    void rainbow();
    void rainbowWithGlitter();
    void addGlitter(fract8 chanceOfGlitter);
    void confetti();
    void sinelon();
    void bpm();
    void juggle();

    uint8_t _option = 0;
    uint8_t _startIndex = 0;
    uint8_t gHue = 0; // rotating "base color" used by many of the patterns
};

#define LITTLE_DEF(name, loop_function)                                                                                                              \
    class AnimationLittle##name : public AnimationLittle                                                                                             \
    {                                                                                                                                                \
    public:                                                                                                                                          \
        static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation *animation_info);                                                \
        virtual ~AnimationLittle##name();                                                                                                            \
        virtual bool loop() override;                                                                                                                \
                                                                                                                                                     \
    protected:                                                                                                                                       \
        AnimationLittle##name(CRGB *leds, uint16_t led_count, animation *animation_info);                                                            \
    };

LITTLE_DEF(Rainbow, rainbow)
LITTLE_DEF(RainbowWithGlitter, rainbowWithGlitter)
LITTLE_DEF(Confetti, confetti)
LITTLE_DEF(Sinelon, sinelon)
LITTLE_DEF(Bpm, bpm)
LITTLE_DEF(Juggle, juggle)

} /* namespace fastled */
