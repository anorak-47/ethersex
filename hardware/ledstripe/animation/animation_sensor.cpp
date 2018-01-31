
#include "animation_sensor.h"
#include "sensor.h"

#include "../ledstripe_debug.h"

#define TMAX 40
#define TREF 20
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

void AnimationSensor::initialize()
{
    setOption(5);

    LV_("sn0 %u, sn1 %u", sensors_get_value(_animation_info->sensor_index[0]), sensors_get_value(_animation_info->sensor_index[1]));

    uint8_t sns_current_value = sensors_get_value(_animation_info->sensor_index[1]);
    int8_t sd = sns_current_value - sensors_get_value(_animation_info->sensor_index[0]);

    if (sns_current_value < TREF)
        sns_current_value = TREF;

    _startpos = (sns_current_value - TREF) * TSTEPS;
    _endpos = (sns_current_value - TREF + 1) * TSTEPS;

    if (_endpos > PMAX)
    {
        _endpos = PMAX;
        _startpos = _endpos - TSTEPS;
    }

    _movingPalette = CRGBPalette16(ColorFromPalette(_palette, _startpos), ColorFromPalette(_palette, _endpos));

    LV_("v %i %u - %u", sns_current_value, _startpos, _endpos);

    _delta = sd;
    _changed = true;
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

    _movingPalette = CRGBPalette16(ColorFromPalette(_palette, _startpos), ColorFromPalette(_palette, _endpos));

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

        if (_endpos > PMAX)
        {
            _endpos = PMAX;
            _startpos = _endpos - TSTEPS;
        }

        _movingPalette = CRGBPalette16(ColorFromPalette(_palette, _startpos), ColorFromPalette(_palette, _endpos));

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
        else
        {
            fill_gradient_RGB(_leds, 0, ColorFromPalette(_palette, _startpos), _led_count - 1, ColorFromPalette(_palette, _endpos));
        }

        if (_option & 0x80)
        {
            for (uint16_t i = 0; i < _led_count; i++)
            {
                addGlitterAt(i, 8);
            }
        }

        _changed = false;
        return true;
    }

    return false;
}

} /* namespace fastled */
