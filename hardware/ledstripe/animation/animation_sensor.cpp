
#include "animation_sensor.h"
#include "sensor.h"

#include "../ledstripe_debug.h"

#define PMAX (256 - (256 / 16))
//#define PMAX 255
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
    delta = 0;
    old_delta = 0;
    _startIndex = 0;

    setOption(0);
    _movingPalette = CRGBPalette16(CRGB::DarkSlateBlue, CRGB::DarkSlateBlue);
}

DEFINE_GRADIENT_PALETTE(heatmap_gp){0,   0,   0,   0,    // black
                                    128, 255, 0,   0,    // red
                                    224, 255, 255, 0,    // bright yellow
                                    255, 255, 255, 255}; // full white

DEFINE_GRADIENT_PALETTE(sherbert_gp){0,   0x64, 0xf3, 0x8c,  // 0
                                     255, 0xf7, 0x9d, 0x00}; // 255

DEFINE_GRADIENT_PALETTE(summer_gp){0,   0x22, 0xc1, 0xc3,  // 0
                                   255, 0xfd, 0xbb, 0x2d}; // 255

DEFINE_GRADIENT_PALETTE(sunset_gp){0,   0x0b, 0x48, 0x6b,  // 0
                                   255, 0xf5, 0x62, 0x17}; // 255

DEFINE_GRADIENT_PALETTE(bradyfun_gp){0,   0x00, 0xc3, 0xff,  // 0
                                     255, 0xff, 0xff, 0x1c}; // 255

DEFINE_GRADIENT_PALETTE(dusk_gp){0,   0x19, 0x54, 0x7b,  // 0
                                 255, 0xff, 0xd8, 0x9b}; // 255

DEFINE_GRADIENT_PALETTE(pinky_gp){0,   64,  224, 208, // 0
                                  127, 255, 0,   128, // 127
                                  230, 255, 140, 0,   // 230
                                  255, 255, 140, 0};  // 255

/*
 * https://uigradients.com
 * http://angrytools.com/gradient/
 * https://www.cssmatic.com/gradient-generator
 *
background: linear-gradient(to right, #FF0080, #FF8C00, #40E0D0);

dusk
background: linear-gradient(to right, #19547b, #ffd89b);

sherbert
background: linear-gradient(to right, #64f38c, #f79d00);
*/

void AnimationSensor::setOption(uint8_t option)
{
    _option = option;

    LV_("sns opt %u", _option);

    switch (option & 0x3F)
    {
    case 0:
        _palette = bradyfun_gp;
        break;
    case 1:
        _palette = sunset_gp;
        break;
    case 2:
        _palette = summer_gp;
        break;
    case 3:
        _palette = sherbert_gp;
        break;
    case 4:
        _palette = dusk_gp;
        break;
    case 5:
        _palette = pinky_gp;
        break;
    case 6:
        _palette = CHSVPalette16(_animation_info->hsv[1], _animation_info->hsv[0]);
        break;
    case 7:
        _palette = CHSVPalette16(_animation_info->hsv[0], _animation_info->hsv[1]);
        break;
    case 8:
        _palette = heatmap_gp;
        break;
    default:
        LV_("opt oor %u", option);
        break;
    }

    old_delta = 0;
}

void AnimationSensor::updateSensors()
{
    sns_current_value = sensors_get_value8(_animation_info->sensor_index[SENSOR_IDX_CURRENT]);
    sns_ref_value = sensors_get_value8(_animation_info->sensor_index[SENSOR_IDX_REF]);

    if (sns_current_value < sns_ref_value)
        sns_current_value = sns_ref_value;

    old_delta = delta;
    delta = sns_current_value - sns_ref_value;
}

bool AnimationSensor::sensorsChanged()
{
    return (old_delta != delta);
}

bool AnimationSensor::animateByOptionValue()
{
    return (_option & 0x80) || (_option & 0x40);
}

void AnimationSensor::updatePalette()
{
    uint16_t startpos = delta * TSTEPS;
    uint16_t endpos = (delta + 1) * TSTEPS;

    if (endpos > PMAX)
    {
        endpos = PMAX;
        startpos = endpos - TSTEPS;
    }

    LV_("snsa: d:%u %u-%u", delta, startpos, endpos);

    _targetPalette = CRGBPalette16(ColorFromPalette(_palette, startpos), ColorFromPalette(_palette, endpos));
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
    // EVERY_N_MILLISECONDS(20)
    {
        nblendPaletteTowardPalette(_movingPalette, _targetPalette, 10);
    }

    if (_option & 0x40)
    {
        fill_palette(_leds, _led_count, _startIndex, 1, _movingPalette, 255, LINEARBLEND);
        _startIndex++;
    }
    else
    {
        fill_palette(_leds, _led_count, 0, 2, _movingPalette, 255, LINEARBLEND);
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
        updatePalette();

    // if (sensorsChanged() || animateByOptionValue())
    {
        animate();
        return true;
    }

    return false;
}

} /* namespace fastled */
