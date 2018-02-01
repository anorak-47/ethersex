
#include "animation_sensor.h"
#include "sensor.h"

#include "../ledstripe_debug.h"

//#define PMAX (255 - (256 / 16))
#define PMAX 255
#define TSTEPS ((PMAX + 1) / (TMAX - TREF))

namespace fastled
{

AnimationSensor::AnimationSensor(CRGB *leds, uint16_t led_count, animation *animation_info) : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationSensor::~AnimationSensor()
{
}

void AnimationSensor::updateSensors()
{
    sns_current_value = sensors_get_value(_animation_info->sensor_index[SENSOR_IDX_CURRENT]);
    sns_ref_value = sensors_get_value(_animation_info->sensor_index[SENSOR_IDX_REF]);

    if (sns_current_value < TREF)
        sns_current_value = TREF;

    delta = sns_current_value - sns_ref_value;
}

bool AnimationSensor::sensorsChanged()
{
    int8_t delta = sns_current_value - sns_ref_value;
    bool changed = (_old_delta != delta);
    _old_delta = delta;

    if (changed)
    	updatePalette();

    return changed || animateByOptionValue();
}

bool AnimationSensor::animateByOptionValue()
{
    return (_option & 0x80) || (_option & 0x40);
}

void AnimationSensor::updatePalette()
{
    uint16_t startpos = (sns_current_value - TREF) * TSTEPS;
    uint16_t endpos = (sns_current_value - TREF + 1) * TSTEPS;

    if (endpos > PMAX)
    {
        endpos = PMAX;
        startpos = endpos - TSTEPS;
    }

    LV_("snsa: c:%i r:%i %u-%u", sns_current_value, sns_ref_value, startpos, endpos);

    _movingPalette = CRGBPalette16(ColorFromPalette(_palette, startpos), ColorFromPalette(_palette, endpos));
}

void AnimationSensor::initialize()
{
	delta = 0;
    _old_delta = 0;
    _startIndex = 0;
}

DEFINE_GRADIENT_PALETTE(heatmap_gp){0,   0,   0,   0,    // black
                                    128, 255, 0,   0,    // red
                                    224, 255, 255, 0,    // bright yellow
                                    255, 255, 255, 255}; // full white

void AnimationSensor::setOption(uint8_t option)
{
    _option = option;

    switch (option & 0x3F)
    {
    case 0:
        _palette = CHSVPalette16(_animation_info->hsv[1], _animation_info->hsv[0]);
        break;
    case 1:
        _palette = CHSVPalette16(_animation_info->hsv[0], _animation_info->hsv[1]);
        break;
    case 2:
        _palette = CHSVPalette16(CHSV(160, 192, 64), CHSV(64, 255, 255));
        break;
    case 3:
        _palette = CHSVPalette16(CHSV(64, 255, 255), CHSV(160, 192, 64));
        break;
    case 4:
        _palette = HeatColors_p;
        break;
    case 5:
        _palette = heatmap_gp;
        break;
    default:
        LV_("opt oor %u", option);
        break;
    }

    _old_delta = 0;
}

void AnimationSensor::update()
{
}

void AnimationSensor::addGlitter(fract8 chanceOfGlitter)
{
    if (random8() < chanceOfGlitter)
    {
        _leds[random16(_led_count)] += CRGB::Lavender;
    }
}

void AnimationSensor::addGlitterAt(uint16_t led, fract8 chanceOfGlitter)
{
    if (random8() < chanceOfGlitter)
    {
        _leds[led] += CRGB::Lavender;
    }
}

void AnimationSensor::animate()
{
    if (_option & 0x40)
    {
    	/*
        for (uint16_t i = 0; i < _led_count; i++)
        {
            _leds[i] = ColorFromPalette(_movingPalette, i + _startIndex);
        }
        */

        fill_palette(_leds, _led_count, 0, _startIndex, _movingPalette, 255, LINEARBLEND);
        _startIndex++;
    }
    else
    {
        fill_palette(_leds, _led_count, 0, 0, _movingPalette, 255, LINEARBLEND);
    }

    if (_option & 0x80)
    {
        for (uint16_t i = 0; i < _led_count; i++)
        {
            addGlitterAt(i, 8);
        }
    }
}

bool AnimationSensor::loop()
{
    updateSensors();

    if (sensorsChanged())
    {
        animate();
        return true;
    }

    return false;
}

} /* namespace fastled */
