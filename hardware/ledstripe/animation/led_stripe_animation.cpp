
#include "led_stripe_animation.h"

namespace fastled
{

LedStripeAnimation::LedStripeAnimation(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
    : _leds(leds), _led_count(led_count), _animation_info(animation_info)
{
}

LedStripeAnimation::~LedStripeAnimation()
{
}

void LedStripeAnimation::initialize()
{
    fill_solid(_leds, _led_count, CRGB::Black);
    setOption(0);
}

void LedStripeAnimation::deinitialize()
{
    fill_solid(_leds, _led_count, CRGB::Black);
}

void LedStripeAnimation::setOption(uint8_t)
{
    // do nothing
}

void LedStripeAnimation::setOption2(uint8_t)
{
    // do nothing
}

void LedStripeAnimation::update()
{
    // do nothing
}

} /* namespace fastled */
