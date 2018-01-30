
#include "animation_sensor.h"
#include "sensor.h"

#include "../ledstripe_debug.h"

#define TMAX 40
#define TREF 20
#define TSTEPS (255 / (TMAX - TREF))

namespace fastled
{

AnimationSensor::AnimationSensor(CRGB *leds, uint16_t led_count, animation *animation_info) : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationSensor::~AnimationSensor()
{
}

void AnimationSensor::initialize()
{
    LedStripeAnimation::initialize();

    LV_("sn0 %u, sn1 %u", sensors_get_value(_animation_info->sensor_index[0]), sensors_get_value(_animation_info->sensor_index[1]));

    uint8_t sns_current_value = sensors_get_value(_animation_info->sensor_index[1]);
    int8_t sd = sns_current_value - sensors_get_value(_animation_info->sensor_index[0]);

    if (sns_current_value < TREF)
        sns_current_value = TREF;

    _startpos = (sns_current_value - TREF) * TSTEPS;
    _endpos = (sns_current_value - TREF + 1) * TSTEPS;

    if (_endpos > 255)
    {
        _endpos = 255;
        _startpos = _endpos - TSTEPS;
    }

    _movingPalette = CHSVPalette16(ColorFromPalette(_palette, _startpos), ColorFromPalette(_palette, _endpos));

    LV_("v %i %u - %u", sns_current_value, _startpos, _endpos);

    _delta = sd;
    _changed = true;
}

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
    default:
        LV_("opt oor %u", option);
        break;
    }

    _movingPalette = CHSVPalette16(ColorFromPalette(_palette, _startpos), ColorFromPalette(_palette, _endpos));

    _changed = true;
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

bool AnimationSensor::loop()
{
    uint8_t sns_current_value = sensors_get_value(_animation_info->sensor_index[1]);

    if (sns_current_value < TREF)
        sns_current_value = TREF;

    int8_t sd = sns_current_value - sensors_get_value(_animation_info->sensor_index[0]);

    if (sd > 1 && sd != _delta)
    {
        LV_("sd %i", sd);

        _startpos = (sns_current_value - TREF) * TSTEPS;
        _endpos = (sns_current_value - TREF + 1) * TSTEPS;

        if (_endpos > 255)
        {
            _endpos = 255;
            _startpos = _endpos - TSTEPS;
        }

        _movingPalette = CHSVPalette16(ColorFromPalette(_palette, _startpos), ColorFromPalette(_palette, _endpos));

        LV_("v %i %u - %u", sns_current_value, _startpos, _endpos);

        _delta = sd;
        _changed = true;
    }

    if ((_option & 0x80) || (_option & 0x40) || _changed)
    {
        if (_option & 0x40)
        {
            for (uint16_t i = 0; i < _led_count; i++)
            {
                _leds[i] = ColorFromPalette(_movingPalette, i + _startIndex);
            }

            _startIndex++;
        }
        else if (_option & 0x80)
        {
            for (uint16_t i = 0; i < _led_count; i++)
            {
                _leds[i] = ColorFromPalette(_movingPalette, i + _startIndex);
                addGlitterAt(i, 8);
            }

            _startIndex++;
        }
        else
        {
            fill_gradient(_leds, 0, ColorFromPalette(_palette, _startpos), _led_count - 1, ColorFromPalette(_palette, _endpos));
        }

        _changed = false;
        return true;
    }

    return false;
}

} /* namespace fastled */
