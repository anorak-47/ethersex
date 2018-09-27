#pragma once

#include "animation_config.h"
#include "animation_type.h"

namespace fastled
{

class LedStripeAnimation
{
public:
    LedStripeAnimation(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    virtual ~LedStripeAnimation();

    virtual void initialize();
    virtual bool loop() = 0;
    virtual void deinitialize();

    virtual void setOption(uint8_t option);
    virtual void setOption2(uint8_t option);

    virtual void update();

protected:
    CRGB *_leds = 0;
    uint16_t _led_count = 0;
    animation_configuration_t *_animation_info = 0;
};

} /* namespace fastled */
