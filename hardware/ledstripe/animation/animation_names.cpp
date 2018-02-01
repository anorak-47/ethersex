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

#include "../ledstripe_debug.h"

#if FASTLED_SUPPORTED

using namespace fastled;

void animation_set_for_stripe(uint8_t stripe, animation_names animation)
{
    if (stripe >= MAX_LED_STRIPES)
        return;

    uint8_t animation_index = static_cast<uint8_t>(animation);
    LV_("set ani %u %u", stripe, animation_index);

    delete led_stripe[stripe].animation;

    led_stripe[stripe].active_animation = animation;
    led_stripe[stripe].delay_msecs = FPS_TO_DELAY(led_stripe_status[stripe].animations[animation_index].fps);

    switch (animation)
    {
    case animation_names::AnimationTests:
        led_stripe[stripe].animation = new fastled::AnimationBasicTests(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                        &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationLittle:
        led_stripe[stripe].animation = new fastled::AnimationLittle(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                    &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationGradient:
        led_stripe[stripe].animation = new fastled::AnimationGradient(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                      &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationPalette:
        led_stripe[stripe].animation = new fastled::AnimationRotatingPalette(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                             &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationFire2012:
        led_stripe[stripe].animation = new fastled::AnimationFire2012(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                      &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationInoiseFire:
        led_stripe[stripe].animation = new fastled::AnimationInoiseFire(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                        &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationInoiseMover:
        led_stripe[stripe].animation = new fastled::AnimationInoiseMover(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                         &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationInoisePal:
        led_stripe[stripe].animation = new fastled::AnimationInoisePal(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                       &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationDotBeat:
        led_stripe[stripe].animation = new fastled::AnimationDotBeat(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                     &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationSerendipitous:
        led_stripe[stripe].animation = new fastled::AnimationSerendipitous(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                           &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationMover:
        led_stripe[stripe].animation = new fastled::AnimationMover(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                   &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationBeatWave:
        led_stripe[stripe].animation = new fastled::AnimationBeatWave(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                      &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationNoise16:
        led_stripe[stripe].animation = new fastled::AnimationNoise16(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                     &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationRainbowBeat:
        led_stripe[stripe].animation = new fastled::AnimationRainbowBeat(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                         &led_stripe_status[stripe].animations[animation_index]);
        break;
    case animation_names::AnimationSensorVisualisation:
        led_stripe[stripe].animation = new fastled::AnimationSensor(led_stripe[stripe].leds, led_stripe[stripe].led_count,
                                                                    &led_stripe_status[stripe].animations[animation_index]);
        break;
    }
}

#endif
