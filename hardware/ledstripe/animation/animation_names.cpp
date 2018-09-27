#include "animation_names.h"
#include "animation_fire2012.h"
#include "animation_gradient.h"
#include "animation_little.h"
#include "animation_basic_tests.h"
#include "animation_rotating_palette.h"
#include "animation_inoise_fire.h"
#include "animation_inoise_mover.h"
#include "animation_inoise_pal.h"
#include "animation_dot_beat.h"
#include "animation_serendipitous.h"
#include "animation_beatwave.h"
#include "animation_mover.h"
#include "animation_sensor.h"
#include "animation_noise16.h"
#include "animation_rainbow_beat.h"
#include "animation_twinkles.h"
#include "../ledstripe_debug.h"

#if FASTLED_SUPPORTED

using namespace fastled;

struct animation_description_s
{
    const char *name;
    const char *description;
    const char *options;
    void (*factory_function)(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);
};

using animation_description_t = struct animation_description_s;

animation_description_t animation_description;

#define TWINKLE(factory_function, name, description) {name, , description, 0, factory_function},

const animation_description_t animation_description_list[ANIMATION_COUNT] PROGMEM = {
    {"basic", "Basic tests", "0:color, 1:blink, 2:mover", &AnimationBasicTests::create},

    //{"little", "Some little animations", "0:Rainbow, 1:Rainbow w/Glitter, 2:Confetti, 3:Sinelon, 4:BPM, 5:Juggle", &AnimationLittle::create},

    {"rainbow", "Rainbow", 0, &AnimationLittleRainbow::create},
    {"glitter", "Rainbow with Glitter", 0, &AnimationLittleRainbowWithGlitter::create},
    {"confetti", "Confetti", 0, &AnimationLittleConfetti::create},
    {"sinelon", "Sinelon", 0, &AnimationLittleSinelon::create},
    {"bpm", "Bpm", 0, &AnimationLittleBpm::create},
    {"juggle", "Juggle", 0, &AnimationLittleJuggle::create},

    {"gradient", "Gradient co0/co1", 0, &AnimationGradient::create},
    {"palette", "Rotating Palettes", "0:Rainbow, 1:Stripes, 2:LinearStripes, 3:Clouds, 4:Party, 5:PurpleGreen, 6:Random ",
     &AnimationRotatingPalette::create},

    {"fire", "Fire2012", "1:Heat, 2:red/yellow, 3:blue/aqua, 4:red/white", &AnimationFire2012::create},
    {"inoise", "Noise on Palettes", 0, &AnimationInoiseFire::create},
    {"imover", "Noisy pixel mover", 0, &AnimationInoiseMover::create},
    {"pal", "Palette blending", 0, &AnimationInoisePal::create},
    {"dotbeat", "Dot and beatsin8", 0, &AnimationDotBeat::create},
    {"serendipitous", "1D Serendipitous Circles", 0, &AnimationSerendipitous::create},
    {"mover", "Moving pixel", 0, &AnimationMover::create},
    {"beatwave", "Coloured wave values from several beatsin8 functions", 0, &AnimationBeatWave::create},
    {"noise16", "A 16 bit noise routine with palettes", 0, &AnimationNoise16::create},
    {"rainbow", "Moving rainbow (beatsin8)", 0, &AnimationRainbowBeat::create},

    // Twinkle
    {"rainbowTwinkles", "Rainbow Twinkles", 0, &AnimationTwinklesRainbowTwinkles::create},
    {"snowTwinkles", "Snow Twinkles", 0, &AnimationTwinklesSnowTwinkles::create},
    {"cloudTwinkles", "Cloud Twinkles", 0, &AnimationTwinklesCloudTwinkles::create},
    {"incandescentTwinkles", "Incandescent Twinkles", 0, &AnimationTwinklesIncandescentTwinkles::create},

    // TwinkleFOX patterns::create
    {"retroC9Twinkles", "Retro C9 Twinkles", 0, &AnimationTwinklesRetroC9Twinkles::create},
    {"redWhiteTwinkles", "Red & White Twinkles", 0, &AnimationTwinklesRedWhiteTwinkles::create},
    {"blueWhiteTwinkles", "Blue & White Twinkles", 0, &AnimationTwinklesBlueWhiteTwinkles::create},
    {"redGreenWhiteTwinkles", "Red, Green & White Twinkles", 0, &AnimationTwinklesRedGreenWhiteTwinkles::create},
    {"fairyLightTwinkles", "Fairy Light Twinkles", 0, &AnimationTwinklesFairyLightTwinkles::create},
    {"snow2Twinkles", "Snow 2 Twinkles", 0, &AnimationTwinklesSnow2Twinkles::create},
    {"hollyTwinkles", "Holly Twinkles", 0, &AnimationTwinklesHollyTwinkles::create},
    {"iceTwinkles", "Ice Twinkles", 0, &AnimationTwinklesIceTwinkles::create},
    {"partyTwinkles", "Party Twinkles", 0, &AnimationTwinklesPartyTwinkles::create},
    {"forestTwinkles", "Forest Twinkles", 0, &AnimationTwinklesForestTwinkles::create},
    {"lavaTwinkles", "Lava Twinkles", 0, &AnimationTwinklesLavaTwinkles::create},
    {"fireTwinkles", "Fire Twinkles", 0, &AnimationTwinklesFireTwinkles::create},
    {"cloud2Twinkles", "Cloud 2 Twinkles", 0, &AnimationTwinklesCloud2Twinkles::create},
    {"oceanTwinkles", "Ocean Twinkles", 0, &AnimationTwinklesOceanTwinkles::create},

    // The animated palette. Must be the last one.
    {"sensor", "Palette animated by sensors", "0:c1/c0, 1:c0/c1, 2:?, 3:?, 4:HeatMap, 5:red/yellow/white, 6:blue/red, 7:sherbert",
     &AnimationSensor::create},
};

const char *get_animation_name(uint8_t index)
{
    if (index >= ANIMATION_COUNT)
        return 0;

    memcpy_P(&animation_description, &animation_description_list[index], sizeof(animation_description_t));
    return animation_description.name;
}

const char *get_animation_description(uint8_t index)
{
    if (index >= ANIMATION_COUNT)
        return 0;

    memcpy_P(&animation_description, &animation_description_list[index], sizeof(animation_description_t));
    return animation_description.description;
}

bool animation_has_options_description(uint8_t index)
{
    if (index >= ANIMATION_COUNT)
        return 0;

    memcpy_P(&animation_description, &animation_description_list[index], sizeof(animation_description_t));
    return animation_description.options != 0;
}

const char *get_animation_options_description(uint8_t index)
{
    if (index >= ANIMATION_COUNT)
        return 0;

    memcpy_P(&animation_description, &animation_description_list[index], sizeof(animation_description_t));
    return animation_description.options;
}

void animation_set_for_stripe_by_index(uint8_t stripe, uint8_t index)
{
    if (stripe >= MAX_LED_STRIPES)
        return;

    if (index >= ANIMATION_COUNT)
        return;

    memcpy_P(&animation_description, &animation_description_list[index], sizeof(animation_description_t));

    delete led_stripe[stripe].animation;

    led_stripe[stripe].active_animation = index;
    led_stripe[stripe].delay_msecs = FPS_TO_DELAY(led_stripe_status[stripe].animations[index].fps);

    led_stripe[stripe].animation =
        animation_description.factory_function(led_stripe[stripe].leds, led_stripe[stripe].led_count, &led_stripe_status[stripe].animations[index]);
}

#endif
