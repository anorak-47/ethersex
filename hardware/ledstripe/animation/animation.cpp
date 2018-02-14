#include "Arduino.h"
#include "animation.h"
#include "animation_config.h"
#include "animation_setup.h"
#include "animation_names.h"
#include "led_stripe_animation.h"
#include "sensor.h"

#include "../ledstripe_debug.h"

#if FASTLED_SUPPORTED

//#define ANIMATION_LIMIT_FPS_BY_TIMER1
#define ANIMATION_SHOW_FPS

#ifdef ANIMATION_LIMIT_FPS_BY_TIMER1
#include "timer.h"
#endif

using namespace fastled;

extern "C" {

#define EEPROM_CHK_MAGIC (0xA5)     // magic number to 'assure' empty/garbage eeprom sections can be
                                    // detected
uint8_t EEMEM chk_animation_eeprom; // assure this variable follows the eeprom
                                    // section which must be checksum-checked!

led_stripe_animation_status led_stripe_status[MAX_LED_STRIPES];
led_stripe_animation_status EEMEM led_stripe_status_eeprom[MAX_LED_STRIPES];

led_stripe_animation led_stripe[MAX_LED_STRIPES];

uint8_t EEMEM global_brightness_eeprom = FASTLED_DEFAULT_BRIGHTNESS;

static uint8_t active_animation_count = 0;
static uint16_t fld_fps = 0;
static uint32_t now = 0;
static bool animated = false;

void animation_reset_sensor_defaults(void)
{
    // sesor index defaults for stripe 0
    led_stripe_status[0].animations[led_stripe_status[0].sensor_animation].sensor_index[SENSOR_IDX_CURRENT] = 0;
    led_stripe_status[0].animations[led_stripe_status[0].sensor_animation].sensor_index[SENSOR_IDX_REF] = 1;

    // sesor index defaults for stripe 1
    led_stripe_status[1].animations[led_stripe_status[1].sensor_animation].sensor_index[SENSOR_IDX_CURRENT] = 2;
    led_stripe_status[1].animations[led_stripe_status[1].sensor_animation].sensor_index[SENSOR_IDX_REF] = 1;
}

void animation_reset(void)
{
    active_animation_count = 0;

    animation_set_global_brightness(FASTLED_DEFAULT_BRIGHTNESS);

    for (uint8_t stripe = 0; stripe < MAX_LED_STRIPES; stripe++)
    {
        memset(&led_stripe[stripe], 0, sizeof(led_stripe_animation));
        led_stripe[stripe].delay_msecs = FPS_TO_DELAY(10);

        memset(&led_stripe_status[stripe], 0, sizeof(led_stripe_animation_status));

        led_stripe_status[stripe].autoplay = false;
        led_stripe_status[stripe].is_autoswitch_sensor_animation = true;
        led_stripe_status[stripe].current_animation = animation_names::AnimationLittle;
        led_stripe_status[stripe].sensor_animation = animation_names::AnimationSensorVisualisation;

        // led_stripe_status[stripe].sensor_index[SENSOR_IDX_CURRENT] = 0;
        // led_stripe_status[stripe].sensor_index[SENSOR_IDX_REF] = 1;

        for (uint8_t a = 0; a < ANIMATION_COUNT; a++)
        {
            led_stripe_status[stripe].animations[a].fps = 25;
            led_stripe_status[stripe].animations[a].sensor_index[SENSOR_IDX_CURRENT] = 0;
            led_stripe_status[stripe].animations[a].sensor_index[SENSOR_IDX_REF] = 1;
            led_stripe_status[stripe].animations[a].hsv[0] = CHSV(0, 255, 255);
            led_stripe_status[stripe].animations[a].hsv[1] = CHSV(127, 255, 255);
        }

        animation_set_for_stripe(stripe, led_stripe_status[stripe].current_animation);

        if (led_stripe_status[stripe].autoplay)
        {
            animation_start(stripe);
        }
    }

    animation_reset_sensor_defaults();
}

void animation_load_stripe(uint8_t stripe)
{
    eeprom_read_block((void *)&(led_stripe_status[stripe]), &(led_stripe_status_eeprom[stripe]), sizeof(led_stripe_status[stripe]));
}

void animation_load(bool clear)
{
    animation_reset();

    if (clear)
        return;

    uint8_t magic = eeprom_read_byte(&chk_animation_eeprom);

    if (magic == EEPROM_CHK_MAGIC)
    {
        animation_set_global_brightness(eeprom_read_byte(&global_brightness_eeprom));

        for (uint8_t stripe = 0; stripe < MAX_LED_STRIPES; stripe++)
        {
            animation_load_stripe(stripe);
            animation_set_for_stripe(stripe, led_stripe_status[stripe].current_animation);
        }

        for (uint8_t stripe = 0; stripe < MAX_LED_STRIPES; stripe++)
        {
            if (led_stripe_status[stripe].autoplay)
            {
                animation_start(stripe);
            }
        }
    }
}

void animation_save_stripe(uint8_t stripe)
{
    eeprom_update_block((void *)&(led_stripe_status[stripe]), &(led_stripe_status_eeprom[stripe]), sizeof(led_stripe_status[stripe]));
}

void animation_save(void)
{
    for (uint8_t i = 0; i < MAX_LED_STRIPES; i++)
        animation_save_stripe(i);
    eeprom_update_byte(&global_brightness_eeprom, animation_get_global_brightness());
    eeprom_update_byte(&chk_animation_eeprom, EEPROM_CHK_MAGIC);
}

void animation_init(void)
{
    // This hopefully calls the Arduino init() from Arduino.h
    init();

    animation_load(false);
    animation_led_stripe_setup();
}

void animation_start(uint8_t stripe)
{
    LV_("astart %u r:%u", stripe, led_stripe[stripe].is_running);

    if (!led_stripe[stripe].is_running && led_stripe[stripe].animation)
    {
        active_animation_count++;
        led_stripe[stripe].is_running = true;
        led_stripe[stripe].loop_timer = millis();
        led_stripe[stripe].delay_msecs =
            FPS_TO_DELAY(led_stripe_status[stripe].animations[static_cast<uint8_t>(led_stripe_status[stripe].current_animation)].fps);
        led_stripe[stripe].animation->initialize();
        led_stripe[stripe].animation->setOption(
            led_stripe_status[stripe].animations[static_cast<uint8_t>(led_stripe_status[stripe].current_animation)].option);

        LV_("astart cnt %u", active_animation_count);
    }
}

void animation_stop(uint8_t stripe)
{
    // LV_("astop %u r:%u", stripe, led_stripe[stripe].is_running);

    if (led_stripe[stripe].is_running && led_stripe[stripe].animation)
    {
        led_stripe[stripe].is_running = false;
        led_stripe[stripe].animation->deinitialize();
        active_animation_count--;

        if (active_animation_count == 0)
        {
            animation_prepare_led_stripes_before_show();
            FastLED.show();
        }

        LV_("astop cnt %u", active_animation_count);
    }
}

void animation_set_running(uint8_t stripe, bool run)
{
    // LV_("set_running %u %u", stripe, run);

    if (run)
        animation_start(stripe);
    else
        animation_stop(stripe);
}

void animation_set_current_fps(uint8_t stripe, uint8_t fps)
{
    animation_set_fps(stripe, led_stripe_status[stripe].current_animation, fps);
}

animation_names animation_get_current_animation(uint8_t stripe)
{
    return led_stripe_status[stripe].current_animation;
}

void animation_set_current_animation(uint8_t stripe, animation_names animation)
{
    led_stripe_status[stripe].current_animation = animation;
}

animation_names animation_get_sensor_animation(uint8_t stripe)
{
    return led_stripe_status[stripe].sensor_animation;
}

void animation_set_sensor_animation(uint8_t stripe, animation_names animation)
{
    led_stripe_status[stripe].sensor_animation = animation;
}

animation_names animation_get_active_animation(uint8_t stripe)
{
    return led_stripe[stripe].active_animation;
}

void animation_set_active_animation(uint8_t stripe, animation_names animation)
{
    bool running = led_stripe[stripe].is_running;
    animation_stop(stripe);
    animation_set_for_stripe(stripe, animation);
    if (running)
        animation_start(stripe);
}

bool animation_get_running(uint8_t stripe)
{
    return led_stripe[stripe].is_running;
}

uint8_t animation_get_current_fps(uint8_t stripe)
{
    return DELAY_TO_FPS(led_stripe[stripe].delay_msecs);
}

void animation_set_fps(uint8_t stripe, animation_names animation, uint8_t fps)
{
    led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].fps = fps;

    if (led_stripe_status[stripe].current_animation == animation)
    {
        led_stripe[stripe].delay_msecs = FPS_TO_DELAY(fps);
    }

    // LV_("afps %u %u %u", stripe, fps, led_stripe[stripe].delay_msecs);
}

uint8_t animation_get_fps(uint8_t stripe, animation_names animation)
{
    return led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].fps;
}

bool animation_get_autoplay(uint8_t stripe)
{
    return led_stripe_status[stripe].autoplay;
}

void animation_set_autoplay(uint8_t stripe, bool autoplay)
{
    led_stripe_status[stripe].autoplay = autoplay;
}

void animation_set_color(uint8_t stripe, animation_names animation, uint8_t cnr, uint8_t *hsv)
{
    if (cnr > 1)
        return;
    // LV_("sc %u %u %u", stripe, animation, cnr);
    // LV_("sc %u %u %u", hsv[0], hsv[1], hsv[2]);
    led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr] = CHSV(hsv[0], hsv[1], hsv[2]);
    // LV_("sc %u %u %u",
    // led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr].hue,
    //    led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr].saturation,
    //    led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr].value);
    if (led_stripe[stripe].animation)
        led_stripe[stripe].animation->update();
}

void animation_get_color(uint8_t stripe, animation_names animation, uint8_t cnr, uint8_t *hsv)
{
    if (cnr > 1)
        return;
    // LV_("gc %u %u %u", stripe, animation, cnr);
    // LV_("gc %u %u %u",
    // led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr].hue,
    //    led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr].saturation,
    //    led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr].value);
    hsv[0] = led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr].hue;
    hsv[1] = led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr].saturation;
    hsv[2] = led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].hsv[cnr].value;
    // LV_("gc %u %u %u", hsv[0], hsv[1], hsv[2]);
}

void animation_set_current_color(uint8_t stripe, uint8_t cnr, uint8_t hsv[3])
{
    animation_set_color(stripe, led_stripe_status[stripe].current_animation, cnr, hsv);
}

void animation_get_current_color(uint8_t stripe, uint8_t cnr, uint8_t hsv[3])
{
    animation_get_color(stripe, led_stripe_status[stripe].current_animation, cnr, hsv);
}

#if 1
void animation_set_sensor_index(uint8_t stripe, animation_names animation, uint8_t sensor, uint8_t sensor_index)
{
    if (sensor_index >= MAX_SENSORS || sensor > 1)
        return;
    led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].sensor_index[sensor] = sensor_index;
    if (led_stripe[stripe].animation)
        led_stripe[stripe].animation->update();
}

uint8_t animation_get_sensor_index(uint8_t stripe, animation_names animation, uint8_t sensor)
{
    if (sensor > 1)
        return 0;
    return led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].sensor_index[sensor];
}

void animation_set_current_sensor_index(uint8_t stripe, uint8_t sensor, uint8_t sensor_index)
{
    animation_set_sensor_index(stripe, led_stripe_status[stripe].current_animation, sensor, sensor_index);
}

uint8_t animation_get_current_sensor_index(uint8_t stripe, uint8_t sensor)
{
    return animation_get_sensor_index(stripe, led_stripe_status[stripe].current_animation, sensor);
}

#else

void animation_set_current_sensor_index(uint8_t stripe, uint8_t sensor, uint8_t sensor_index)
{
    led_stripe_status[stripe].sensor_index[sensor] = sensor_index;
    if (led_stripe[stripe].animation)
        led_stripe[stripe].animation->update();
}

uint8_t animation_get_current_sensor_index(uint8_t stripe, uint8_t sensor)
{
    return led_stripe_status[stripe].sensor_index[sensor];
}
#endif

void animation_set_current_option(uint8_t stripe, uint8_t option)
{
    animation_set_option(stripe, led_stripe_status[stripe].current_animation, option);
}

uint8_t animation_get_current_option(uint8_t stripe)
{
    return animation_get_option(stripe, led_stripe_status[stripe].current_animation);
}

void animation_set_option(uint8_t stripe, animation_names animation, uint8_t option)
{
    led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].option = option;

    if (led_stripe_status[stripe].current_animation == animation && led_stripe[stripe].animation)
    {
        led_stripe[stripe].animation->setOption(option);
    }

    // LV_("aso %u %u %u", stripe, static_cast<uint8_t>(animation), option);
}

uint8_t animation_get_option(uint8_t stripe, animation_names animation)
{
    return led_stripe_status[stripe].animations[static_cast<uint8_t>(animation)].option;
}

void animation_set_global_brightness(uint8_t brightness)
{
    FastLED.setBrightness(brightness);
}

uint8_t animation_get_global_brightness()
{
    return FastLED.getBrightness();
}

uint16_t get_current_fld_fps()
{
    return fld_fps;
}

void animation_set_autoswitch_sensor_animation(uint8_t stripe, bool on)
{
    led_stripe_status[stripe].is_autoswitch_sensor_animation = on;
}

bool animation_get_autoswitch_sensor_animation(uint8_t stripe)
{
    return led_stripe_status[stripe].is_autoswitch_sensor_animation;
}

bool is_sensor_animation(uint8_t stripe)
{
    animation_names sensor_animation = animation_get_sensor_animation(stripe);

    //LV_("[%u] idx %u %u %u", stripe, sensor_animation, animation_get_sensor_index(stripe, sensor_animation, SENSOR_IDX_CURRENT),
    //    animation_get_sensor_index(stripe, sensor_animation, SENSOR_IDX_REF));

    int8_t sns_current_value = sensors_get_value(animation_get_sensor_index(stripe, sensor_animation, SENSOR_IDX_CURRENT));
    int8_t sns_ref_value = sensors_get_value(animation_get_sensor_index(stripe, sensor_animation, SENSOR_IDX_REF));

    if (sns_current_value < TREF)
        sns_current_value = TREF;

    int8_t delta = sns_current_value - sns_ref_value;

    //LV_("[%u] sns %i %i d:%i", stripe, sns_current_value, sns_ref_value, delta);

    return (delta > TDELTA);
}

void switch_to_sensor_animation(uint8_t stripe)
{
    if (!animation_get_autoswitch_sensor_animation(stripe))
        return;

    animation_names requested_animation;

    if (is_sensor_animation(stripe))
    {
        //LV_("[%u] to sensor ani", stripe);
        requested_animation = animation_get_sensor_animation(stripe);
    }
    else
    {
        //LV_("[%u] to current ani", stripe);
        requested_animation = animation_get_current_animation(stripe);
    }

    if (requested_animation != animation_get_active_animation(stripe))
    {
        LV_("[%u] run animation %u", stripe, requested_animation);

        animation_stop(stripe);
        animation_set_for_stripe(stripe, requested_animation);

        if (requested_animation == animation_get_sensor_animation(stripe))
        {
            led_stripe[stripe].is_current_animation_running_before_switching = led_stripe[stripe].is_running;
            animation_start(stripe);
        }
        else
        {
            LV_("[%u] restart current ani", stripe);
            if (led_stripe[stripe].is_current_animation_running_before_switching)
                animation_start(stripe);
        }
    }
}

#define TIMER_DIFF(a, b, max) ((a) >= (b) ? (a) - (b) : (max) - (b) + (a))
#define TIMER_DIFF_8(a, b) TIMER_DIFF(a, b, UINT8_MAX)
#define TIMER_DIFF_16(a, b) TIMER_DIFF(a, b, UINT16_MAX)
#define TIMER_DIFF_32(a, b) TIMER_DIFF(a, b, UINT32_MAX)
#define TIMER_DIFF_RAW(a, b) TIMER_DIFF_8(a, b)

inline uint16_t timer_elapsed(uint16_t last)
{
    return TIMER_DIFF_16((millis() & 0xFFFF), last);
}

inline uint32_t timer_elapsed32(uint32_t last)
{
    return TIMER_DIFF_32(millis(), last);
}

void animation_loop()
{
    EVERY_N_SECONDS(10)
    {
        sensors_update();

        for (uint8_t stripe = 0; stripe < MAX_LED_STRIPES; stripe++)
        {
            switch_to_sensor_animation(stripe);
        }

        fld_fps = FastLED.getFPS() / 2;
        // fld_fps = FastLED.getFPS();

#if DEBUG_OUTPUT_SUPPORTED && defined(ANIMATION_SHOW_FPS)
        LV_("fps: %u", fld_fps);
#endif
    }

    if (active_animation_count == 0)
        return;

    now = millis();

    animated = true;

    for (uint8_t stripe = 0; stripe < MAX_LED_STRIPES; stripe++)
    {
        if (led_stripe[stripe].is_running && led_stripe[stripe].animation)
        {
            if (timer_elapsed(led_stripe[stripe].loop_timer) >= led_stripe[stripe].delay_msecs)
            {
                led_stripe[stripe].loop_timer = now;
                animated = led_stripe[stripe].animation->loop() && animated;
            }
        }
    }

    // 1000/9 > 100fps.
    // 1000/19 > 50fps.
    // FPS are limited by lenght of the stripes because of slow protocol
    // Dithering works only if current FPS is greater than 100
    // Dithering with a frame rate below 100 results in flickering LEDs...
    EVERY_N_MILLISECONDS(9)
    {
        if (animated)
        {
            animation_prepare_led_stripes_before_show();
            // call this more often than 100 times a second to enable the dithering
            // this calls FastLED.countFPS(), too!
            FastLED.show();
            // calling this a second time here to fake 100fps, so we get dithering down to 50fsp
            FastLED.countFPS();
        }
    }
}
}
#endif
