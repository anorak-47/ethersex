
#include "animation_sensor.h"
#include "sensor.h"

#include "../ledstripe_debug.h"

//#define PMAX (255 - (256 / 16))
#define PMAX 255
#define TSTEPS ((PMAX + 1) / (TMAX - TREF))

namespace fastled
{

AnimationSensor::AnimationSensor(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
    : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationSensor::~AnimationSensor()
{
}

static LedStripeAnimation *AnimationSensor::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
{
    return new AnimationSensor(leds, led_count, animation_info);
}

void AnimationSensor::updateSimulation()
{
    if (simulate)
    {
        EVERY_N_SECONDS((TMAX_SIMULATE - TREF)*5)
        {
            if (simulated_sensor_dir_up)
            {
                simulated_sensor++;
                if (simulated_sensor > TMAX_SIMULATE)
                {
                    simulated_sensor_dir_up = false;
                }
            }
            else
            {
                simulated_sensor--;
                if (simulated_sensor < TREF)
                {
                    simulated_sensor_dir_up = true;
                }
            }
        }
    }
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
    // int8_t current_delta = sns_current_value - sns_ref_value;

    if (simulate)
    {
        delta = simulated_sensor - TREF;
    }

    bool changed = (_old_delta != delta);
    _old_delta = delta;

    if (changed)
        updatePalette();

    return changed || animateByOptionValue();
}

bool AnimationSensor::animateByOptionValue()
{
    return (_option2);
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
    simulated_sensor = TREF;
    simulate = false;

    setOption(5);
}

DEFINE_GRADIENT_PALETTE(heatmap_gp){0,   0,   0,   0,    // black
                                    128, 255, 0,   0,    // red
                                    224, 255, 255, 0,    // bright yellow
                                    255, 255, 255, 255}; // full white

DEFINE_GRADIENT_PALETTE(blue2red_gp){0,   0,   0,   128,  // blue
                                     128, 255, 255, 0,    // bright yellow
                                     192, 255, 0,   0,    // red
                                     255, 255, 128, 128}; // full white

// Palettes from http://colorpalettes.net

// Color Palette #3642
// bright blue, bright burgundy, bright cyan, bright yellow, burgundy and saffron yellow, color of egg yolk,
// contrast combination of colors for winter, contrasting winter color combination, gentle blue, shades of blue, yellow and blue.

DEFINE_GRADIENT_PALETTE(colorpalette3642_gp){0,   0x06, 0x39, 0x7d, //
                                             64,  0x82, 0xad, 0xd6, //
                                             128, 0xe4, 0xe9, 0xf4, //
                                             192, 0xfa, 0xcb, 0x66, //
                                             220, 0x91, 0x1f, 0x1e};

// Color Palette #3795

DEFINE_GRADIENT_PALETTE(colorpalette3795_gp){0,   0xa1, 0x05, 0x32, //
                                             64,  0xff, 0x6f, 0xa9, //
                                             128, 0xff, 0xd0, 0xcc, //
                                             192, 0x00, 0xb7, 0xcc, //
                                             224, 0x00, 0x5f, 0x81};

// Color Palette #1524
// Pastel turquoise basic background elevates smoky whiteness which does not prevail.

DEFINE_GRADIENT_PALETTE(colorpalette1524_gp){0,   0x01, 0x46, 0x56, //
                                             64,  0x00, 0xa2, 0xad, //
                                             128, 0xf9, 0xf7, 0xf6, //
                                             192, 0xfd, 0xd6, 0x5d, //
                                             224, 0xf6, 0xa8, 0x01};

// Color Palette #1946
// Green grass and blue sky, bright sun and summer mood – this color palette includes blue, azure, light green,
// mustard yellow, yellow. Softer shades of green and blue look advantageous in combination with more saturated colors.

DEFINE_GRADIENT_PALETTE(colorpalette1946_gp){0,   0xaa, 0x9c, 0x31, //
                                             64,  0xe8, 0xd5, 0x3a, //
                                             128, 0xdc, 0xec, 0x94, //
                                             192, 0xb9, 0xe6, 0xf7, //
                                             224, 0x85, 0xc3, 0xfe};

// https://coolors.co/247ba0-70c1b3-b2dbbf-f3ffbd-ff1654

DEFINE_GRADIENT_PALETTE(coolors_0_gp){0,   36,  123, 160, //
                                      64,  112, 193, 179, //
                                      128, 178, 219, 191, //
                                      192, 243, 255, 189, //
                                      224, 255, 22,  84};

// https : // coolors.co/264653-2a9d8f-e9c46a-f4a261-e76f51

DEFINE_GRADIENT_PALETTE(coolors_1_gp){0,   38,  70,  83,  //
                                      64,  42,  157, 143, //
                                      128, 233, 196, 106, //
                                      192, 244, 162, 97,  //
                                      224, 231, 111, 81};

// sherbert
// background: linear-gradient(to right, #64f38c, #f79d00);

DEFINE_GRADIENT_PALETTE(sherbert_linear_gp){0,   0x64, 0xf3, 0x8c, //
                                            255, 0xf7, 0x9d, 0x00};

void AnimationSensor::setOption(uint8_t option)
{
    _option = option;

    switch (option)
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
    case 6:
        _palette = blue2red_gp;
        break;
    case 7:
        _palette = sherbert_linear_gp;
        break;
    case 8:
        _palette = colorpalette3642_gp;
        break;
    case 9:
        _palette = colorpalette3795_gp;
        break;
    case 10:
        _palette = colorpalette1524_gp;
        break;
    case 11:
        _palette = colorpalette1946_gp;
        break;
    case 12:
        _palette = coolors_0_gp;
        break;
    case 13:
        _palette = coolors_1_gp;
        break;
    default:
        LV_("opt oor %u", option);
        break;
    }

    _old_delta = 0;
}

void AnimationSensor::setOption2(uint8_t option)
{
    _option2 = option;
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
    if (_option2 & 0x1)
    {
        // rotate palette
        fill_palette(_leds, _led_count, _startIndex, 1, _movingPalette, 255, LINEARBLEND);
        _startIndex++;
    }
    else
    {
        fill_palette(_leds, _led_count, 0, 1, _movingPalette, 255, LINEARBLEND);
    }

    if (_option2 & 0x2)
    {
        // randomly add some glitter
        for (uint16_t i = 0; i < _led_count; i++)
        {
            addGlitterAt(i, 8);
        }
    }

    simulate = _option2 & 0x4;
}

bool AnimationSensor::loop()
{
    updateSimulation();
    updateSensors();

    if (sensorsChanged())
    {
        animate();
        return true;
    }

    return false;
}

} /* namespace fastled */
