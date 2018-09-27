
#include "animation_twinkles.h"

namespace fastled
{

AnimationTwinkles::AnimationTwinkles(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info) : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationTwinkles::~AnimationTwinkles()
{
}

static LedStripeAnimation *AnimationTwinkles::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
{
    return new AnimationTwinkles(leds, led_count, animation_info);
}

void AnimationTwinkles::setOption(uint8_t option)
{
    _option = option;
}

#define TWINKLE_IMPL(name, loop_function)                                                                                                            \
    LedStripeAnimation *AnimationTwinkles##name::create(CRGB *leds, uint16_t led_count, animation *animation_info)                                   \
    {                                                                                                                                                \
        return new AnimationTwinkles##name(leds, led_count, animation_info);                                                                         \
    }                                                                                                                                                \
    AnimationTwinkles##name::~AnimationTwinkles##name()                                                                                              \
    {                                                                                                                                                \
    }                                                                                                                                                \
    bool AnimationTwinkles##name::loop()                                                                                                             \
    {                                                                                                                                                \
        loop_function();                                                                                                                             \
        return true;                                                                                                                                 \
    }

#include "twinkle.h"
#include "twinkleFOX.h"

TWINKLE_IMPL(IncandescentTwinkles, incandescentTwinkles)
TWINKLE_IMPL(SnowTwinkles, snowTwinkles)
TWINKLE_IMPL(RainbowTwinkles, rainbowTwinkles)
TWINKLE_IMPL(CloudTwinkles, cloudTwinkles)

TWINKLE_IMPL(RetroC9Twinkles, retroC9Twinkles)
TWINKLE_IMPL(RedWhiteTwinkles, redWhiteTwinkles)
TWINKLE_IMPL(BlueWhiteTwinkles, blueWhiteTwinkles)
TWINKLE_IMPL(RedGreenWhiteTwinkles, redGreenWhiteTwinkles)
TWINKLE_IMPL(FairyLightTwinkles, fairyLightTwinkles)
TWINKLE_IMPL(Snow2Twinkles, snow2Twinkles)
TWINKLE_IMPL(HollyTwinkles, hollyTwinkles)
TWINKLE_IMPL(IceTwinkles, iceTwinkles)
TWINKLE_IMPL(PartyTwinkles, partyTwinkles)
TWINKLE_IMPL(ForestTwinkles, forestTwinkles)
TWINKLE_IMPL(LavaTwinkles, lavaTwinkles)
TWINKLE_IMPL(FireTwinkles, fireTwinkles)
TWINKLE_IMPL(Cloud2Twinkles, cloud2Twinkles)
TWINKLE_IMPL(OceanTwinkles, oceanTwinkles)

} /* namespace fastled */
