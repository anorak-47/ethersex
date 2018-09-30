#pragma once

#include "led_stripe_animation.h"

namespace fastled
{

class AnimationSensor : public LedStripeAnimation
{
public:
    static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
    virtual ~AnimationSensor();

    virtual void initialize() override;
    virtual bool loop() override;
    virtual void setOption(uint8_t option) override;
    virtual void setOption2(uint8_t option) override;
    virtual void update() override;

protected:
    AnimationSensor(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);

    void animate();
    void updateSensors();
    void updateSimulation();
    bool sensorsChanged();
    void updatePalette();
    bool animateByOptionValue();

    void addGlitter(fract8 chanceOfGlitter);
    void addGlitterAt(uint16_t led, fract8 chanceOfGlitter);

    uint8_t _startIndex = 0;

    uint8_t _option = 3;
    uint8_t _option2 = 0;

    CRGBPalette16 _palette;
    CRGBPalette16 _movingPalette;
    CRGBPalette16 _targetPalette;

    int8_t sns_current_value = 0;
    int8_t sns_ref_value = 0;

    int8_t delta = 0;
    int8_t old_delta = 0;

    bool simulate = false;
    uint8_t simulated_sensor;
    bool simulated_sensor_dir_up = true;
};

} /* namespace fastled */
