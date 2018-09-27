#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationTwinkles : public LedStripeAnimation
{
public:
    virtual ~AnimationTwinkles();

    virtual void setOption(uint8_t option) override;

protected:
    AnimationTwinkles(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    uint8_t _option = 0;
};

#define TWINKLE_DEF(name, loop_function)                                                                                                             \
    class AnimationTwinkles##name : public AnimationTwinkles                                                                                         \
    {                                                                                                                                                \
    public:                                                                                                                                          \
        static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation *animation_info);                                                \
        virtual ~AnimationTwinkles##name();                                                                                                          \
        virtual bool loop() override;                                                                                                                \
                                                                                                                                                     \
    protected:                                                                                                                                       \
        AnimationTwinkles##name(CRGB *leds, uint16_t led_count, animation *animation_info);                                                          \
    };

TWINKLE_DEF(IncandescentTwinkles, incandescentTwinkles)
TWINKLE_DEF(SnowTwinkles, snowTwinkles)
TWINKLE_DEF(RainbowTwinkles, rainbowTwinkles)
TWINKLE_DEF(CloudTwinkles, cloudTwinkles)

TWINKLE_DEF(RetroC9Twinkles, retroC9Twinkles)
TWINKLE_DEF(RedWhiteTwinkles, redWhiteTwinkles)
TWINKLE_DEF(BlueWhiteTwinkles, blueWhiteTwinkles)
TWINKLE_DEF(RedGreenWhiteTwinkles, redGreenWhiteTwinkles)
TWINKLE_DEF(FairyLightTwinkles, fairyLightTwinkles)
TWINKLE_DEF(Snow2Twinkles, snow2Twinkles)
TWINKLE_DEF(HollyTwinkles, hollyTwinkles)
TWINKLE_DEF(IceTwinkles, iceTwinkles)
TWINKLE_DEF(PartyTwinkles, partyTwinkles)
TWINKLE_DEF(ForestTwinkles, forestTwinkles)
TWINKLE_DEF(LavaTwinkles, lavaTwinkles)
TWINKLE_DEF(FireTwinkles, fireTwinkles)
TWINKLE_DEF(Cloud2Twinkles, cloud2Twinkles)
TWINKLE_DEF(OceanTwinkles, oceanTwinkles)

} /* namespace fastled */
