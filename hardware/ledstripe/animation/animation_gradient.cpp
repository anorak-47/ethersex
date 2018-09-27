
#include "animation_gradient.h"

namespace fastled
{

AnimationGradient::AnimationGradient(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
    : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationGradient::~AnimationGradient()
{
}

static LedStripeAnimation *AnimationGradient::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
{
    return new AnimationGradient(leds, led_count, animation_info);
}

void AnimationGradient::setOption(uint8_t option)
{
    _option = option;

    if (option == 0)
    {
        fill_gradient(_leds, 0, _animation_info->hsv[0], _led_count-1, _animation_info->hsv[1], SHORTEST_HUES);
    }
    else
    {
        _palette = CHSVPalette16(_animation_info->hsv[0], _animation_info->hsv[1]);
    }

    _changed = true;
}

void AnimationGradient::update()
{
    setOption(_option);
}

bool AnimationGradient::loop()
{
    if (_option >= 1)
    {
        fill_palette(_leds, _led_count, _startIndex, 1, _palette, 255, LINEARBLEND);
        _startIndex++; /* motion speed */
        _changed = true;
    }

    if (_changed)
    {
        _changed = false;
        return true;
    }

    return false;
}

} /* namespace fastled */
