
#include "animation_gradient.h"

namespace fastled
{

AnimationGradient::AnimationGradient(CRGB *leds, uint16_t led_count, animation *animation_info)
    : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationGradient::~AnimationGradient()
{
}

void AnimationGradient::setOption(uint8_t option)
{
	_option = option;

    if (option == 0)
    {
    	fill_gradient(_leds, 0, _animation_info->hsv[0], _led_count-1, _animation_info->hsv[1], SHORTEST_HUES);
    }

    _changed = true;
}

void AnimationGradient::update()
{
	setOption(_option);
}

bool AnimationGradient::loop()
{
    //_startIndex = _startIndex + 1; /* motion speed */
    //fill_gradient(_leds, 0, _animation_info->hsv[0], _led_count-1, _animation_info->hsv[1], SHORTEST_HUES);

	if (_changed)
	{
		_changed = false;
		return true;
	}

	return false;
}

} /* namespace fastled */
