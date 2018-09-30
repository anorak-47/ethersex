#include "Arduino.h"
#include "animation.h"
#include "animation_config.h"
#include "animation_setup.h"
#include "animation_names.h"
#include "led_stripe_animation.h"
#include "sensor.h"

#include "../ledstripe_debug.h"

#include <avr/eeprom.h>

#if FASTLED_SUPPORTED

//#define ANIMATION_LIMIT_FPS_BY_TIMER1
//#define ANIMATION_SHOW_FPS

#ifdef ANIMATION_LIMIT_FPS_BY_TIMER1
#include "timer.h"
#endif

using namespace fastled;

// magic number to 'assure' empty/garbage eeprom sections can be detected
#define EEPROM_CHK_MAGIC_0 (0xA5)
#define EEPROM_CHK_MAGIC_1 (0x5A)

#define EEPROM_POS_CHK_0 (uint8_t *)(100)
#define EEPROM_POS_CHK_1 (uint8_t *)(101)
#define EEPROM_POS_STATUS (uint8_t *)(102)

uint8_t global_brightness_eeprom EEMEM = FASTLED_DEFAULT_BRIGHTNESS;
// uint8_t chk_animation_eeprom EEMEM; // assure this variable follows the eeprom section which must be checksum-checked!
// led_stripe_animation_status_t led_stripe_status_eeprom[MAX_LED_STRIPES] EEMEM;

led_stripe_animation_status_t led_stripe_status[MAX_LED_STRIPES];
led_stripe_animation_t led_stripe[MAX_LED_STRIPES];

static uint8_t active_animation_count = 0;
static uint16_t fld_fps = 0;
static uint32_t now = 0;
static bool animated = false;
static int8_t tmax = 0;
static bool emergency_shutdown = false;
static bool is_constraint_matched = false;

#ifdef __cplusplus
extern "C"
{
#endif

    void animation_reset_sensor_defaults(void)
    {
        // Sensor 0: temperature bathtub
        // Sensor 1: temperature at floor under bathtub (reference)
        // Sensor 2: temperature at shower drain
        // Sensor 3: temperature of power supply

        // sensor index defaults for stripe 0
        led_stripe_status[0].animations[led_stripe_status[0].sensor_animation].sensor_index[SENSOR_IDX_CURRENT] = SENSOR_BATHTUB;
        led_stripe_status[0].animations[led_stripe_status[0].sensor_animation].sensor_index[SENSOR_IDX_REF] = SENSOR_REFERENCE;

        // sensor index defaults for stripe 1
        led_stripe_status[1].animations[led_stripe_status[1].sensor_animation].sensor_index[SENSOR_IDX_CURRENT] = SENSOR_SHOWER;
        led_stripe_status[1].animations[led_stripe_status[1].sensor_animation].sensor_index[SENSOR_IDX_REF] = SENSOR_REFERENCE;
    }

    void animation_reset(void)
    {
        active_animation_count = 0;

        animation_set_global_brightness(FASTLED_DEFAULT_BRIGHTNESS);

        for (uint8_t stripe = 0; stripe < MAX_LED_STRIPES; stripe++)
        {
            memset(&led_stripe[stripe], 0, sizeof(led_stripe_animation_t));
            led_stripe[stripe].delay_msecs = FPS_TO_DELAY(10);
            led_stripe[stripe].animation = 0;

            memset(&led_stripe_status[stripe], 0, sizeof(led_stripe_animation_status_t));

            led_stripe_status[stripe].autoplay = false;
            led_stripe_status[stripe].autoplay_delay_msecs = 120000;
            led_stripe_status[stripe].is_autoswitch_sensor_animation = true;
            led_stripe_status[stripe].current_animation = 9;
            led_stripe_status[stripe].sensor_animation = ANIMATION_COUNT - 1;

            for (uint8_t a = 0; a < ANIMATION_COUNT; a++)
            {
                led_stripe_status[stripe].animations[a].fps = 25;
                led_stripe_status[stripe].animations[a].sensor_index[SENSOR_IDX_CURRENT] = SENSOR_BATHTUB;
                led_stripe_status[stripe].animations[a].sensor_index[SENSOR_IDX_REF] = SENSOR_REFERENCE;
                led_stripe_status[stripe].animations[a].hsv[0] = CHSV(0, 255, 255);
                led_stripe_status[stripe].animations[a].hsv[1] = CHSV(127, 255, 255);
            }

            animation_set_for_stripe_by_index(stripe, led_stripe_status[stripe].current_animation);
        }

        animation_reset_sensor_defaults();
    }

    void animation_load_stripe(uint8_t stripe)
    {
        eeprom_read_block((void *)&led_stripe_status[stripe], EEPROM_POS_STATUS + (sizeof(led_stripe_animation_status_t) * stripe),
                          sizeof(led_stripe_animation_status_t));
    }

    void animation_load(bool clear)
    {
        animation_reset();

        animation_set_global_brightness(eeprom_read_byte(&global_brightness_eeprom));

        if (clear)
            return;

        uint8_t magic_0 = eeprom_read_byte(EEPROM_POS_CHK_0);
        uint8_t magic_1 = eeprom_read_byte(EEPROM_POS_CHK_1);

        if (magic_0 == EEPROM_CHK_MAGIC_0 && magic_1 == EEPROM_CHK_MAGIC_1)
        {
            for (uint8_t stripe = 0; stripe < MAX_LED_STRIPES; stripe++)
            {
                animation_load_stripe(stripe);
                animation_set_for_stripe_by_index(stripe, led_stripe_status[stripe].current_animation);
            }
        }
    }

    void animation_save_stripe(uint8_t stripe)
    {
        eeprom_update_block((void *)&led_stripe_status[stripe], EEPROM_POS_STATUS + (sizeof(led_stripe_animation_status_t) * stripe),
                            sizeof(led_stripe_animation_status_t));
    }

    void animation_save(void)
    {
        eeprom_update_byte(&global_brightness_eeprom, animation_get_global_brightness());

        for (uint8_t i = 0; i < MAX_LED_STRIPES; i++)
            animation_save_stripe(i);

        eeprom_update_byte(EEPROM_POS_CHK_0, EEPROM_CHK_MAGIC_0);
        eeprom_update_byte(EEPROM_POS_CHK_1, EEPROM_CHK_MAGIC_1);
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

        if (!led_stripe[stripe].is_running && led_stripe[stripe].animation == 0)
            animation_set_for_stripe_by_index(stripe, animation_get_current_animation(stripe));

        if (!led_stripe[stripe].is_running && led_stripe[stripe].animation)
        {
            active_animation_count++;
            led_stripe[stripe].is_running = true;
            led_stripe[stripe].loop_timer = millis();
            led_stripe[stripe].autoplay_timer = millis();
            led_stripe[stripe].delay_msecs = FPS_TO_DELAY(led_stripe_status[stripe].animations[(led_stripe_status[stripe].current_animation)].fps);
            led_stripe[stripe].animation->initialize();
            led_stripe[stripe].animation->setOption(led_stripe_status[stripe].animations[(led_stripe_status[stripe].current_animation)].option[0]);
            led_stripe[stripe].animation->setOption2(led_stripe_status[stripe].animations[(led_stripe_status[stripe].current_animation)].option[1]);

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

    uint8_t animation_get_current_animation(uint8_t stripe)
    {
        return led_stripe_status[stripe].current_animation;
    }

    void animation_set_current_animation(uint8_t stripe, uint8_t animation)
    {
        led_stripe_status[stripe].current_animation = animation;
    }

    uint8_t animation_get_sensor_animation(uint8_t stripe)
    {
        return led_stripe_status[stripe].sensor_animation;
    }

    void animation_set_sensor_animation(uint8_t stripe, uint8_t animation)
    {
        led_stripe_status[stripe].sensor_animation = animation;
    }

    uint8_t animation_get_active_animation(uint8_t stripe)
    {
        return led_stripe[stripe].active_animation;
    }

    void animation_set_active_animation(uint8_t stripe, uint8_t animation)
    {
        bool running = led_stripe[stripe].is_running;
        animation_stop(stripe);
        animation_set_for_stripe_by_index(stripe, animation);
        if (running)
            animation_start(stripe);
    }

    uint8_t animation_set_next_animation_active(uint8_t stripe)
    {
        uint8_t id = animation_get_active_animation(stripe);
        if (id < ANIMATION_COUNT - 1)
        {
            id++;
        }
        else
        {
            id = 0;
        }

        animation_set_current_animation(stripe, id);
        animation_set_active_animation(stripe, id);

        if (!animation_get_running(stripe))
            animation_start(stripe);

        return id;
    }

    uint8_t animation_set_previous_animation_active(uint8_t stripe)
    {
        uint8_t id = animation_get_active_animation(stripe);
        if (id > 0)
        {
            id--;
        }
        else
        {
            id = ANIMATION_COUNT - 2;
        }

        animation_set_current_animation(stripe, id);
        animation_set_active_animation(stripe, id);

        if (!animation_get_running(stripe))
            animation_start(stripe);

        return id;
    }

    bool animation_get_running(uint8_t stripe)
    {
        return led_stripe[stripe].is_running;
    }

    uint8_t animation_get_current_fps(uint8_t stripe)
    {
        return DELAY_TO_FPS(led_stripe[stripe].delay_msecs);
    }

    void animation_set_fps(uint8_t stripe, uint8_t animation, uint8_t fps)
    {
        led_stripe_status[stripe].animations[animation].fps = fps;

        if (led_stripe_status[stripe].current_animation == animation)
        {
            led_stripe[stripe].delay_msecs = FPS_TO_DELAY(fps);
        }

        // LV_("afps %u %u %u", stripe, fps, led_stripe[stripe].delay_msecs);
    }

    uint8_t animation_get_fps(uint8_t stripe, uint8_t animation)
    {
        return led_stripe_status[stripe].animations[animation].fps;
    }

    bool animation_get_autoplay(uint8_t stripe)
    {
        return led_stripe_status[stripe].autoplay;
    }

    void animation_set_autoplay(uint8_t stripe, bool autoplay)
    {
        led_stripe_status[stripe].autoplay = autoplay;
    }

    uint16_t animation_get_autoplay_delay_time(uint8_t stripe)
    {
        return led_stripe_status[stripe].autoplay_delay_msecs / 1000UL;
    }

    void animation_set_autoplay_delay_time(uint8_t stripe, uint16_t autoplay_delay_secs)
    {
        led_stripe_status[stripe].autoplay_delay_msecs = 1000UL * autoplay_delay_secs;
    }

    void animation_set_color(uint8_t stripe, uint8_t animation, uint8_t cnr, uint8_t *hsv)
    {
        if (cnr > 1)
            return;
        // LV_("sc %u %u %u", stripe, animation, cnr);
        // LV_("sc %u %u %u", hsv[0], hsv[1], hsv[2]);
        led_stripe_status[stripe].animations[animation].hsv[cnr] = CHSV(hsv[0], hsv[1], hsv[2]);
        // LV_("sc %u %u %u",
        // led_stripe_status[stripe].animations[animation].hsv[cnr].hue,
        //    led_stripe_status[stripe].animations[animation].hsv[cnr].saturation,
        //    led_stripe_status[stripe].animations[animation].hsv[cnr].value);
        if (led_stripe[stripe].animation)
            led_stripe[stripe].animation->update();
    }

    void animation_get_color(uint8_t stripe, uint8_t animation, uint8_t cnr, uint8_t *hsv)
    {
        if (cnr > 1)
            return;
        // LV_("gc %u %u %u", stripe, animation, cnr);
        // LV_("gc %u %u %u",
        // led_stripe_status[stripe].animations[animation].hsv[cnr].hue,
        //    led_stripe_status[stripe].animations[animation].hsv[cnr].saturation,
        //    led_stripe_status[stripe].animations[animation].hsv[cnr].value);
        hsv[0] = led_stripe_status[stripe].animations[animation].hsv[cnr].hue;
        hsv[1] = led_stripe_status[stripe].animations[animation].hsv[cnr].saturation;
        hsv[2] = led_stripe_status[stripe].animations[animation].hsv[cnr].value;
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

    void animation_set_sensor_index(uint8_t stripe, uint8_t animation, uint8_t sensor, uint8_t sensor_index)
    {
        if (sensor_index >= MAX_SENSORS || sensor > 1)
            return;
        led_stripe_status[stripe].animations[animation].sensor_index[sensor] = sensor_index;
        if (led_stripe[stripe].animation)
            led_stripe[stripe].animation->update();
    }

    uint8_t animation_get_sensor_index(uint8_t stripe, uint8_t animation, uint8_t sensor)
    {
        if (sensor > 1)
            return 0;
        return led_stripe_status[stripe].animations[animation].sensor_index[sensor];
    }

    void animation_set_current_sensor_index(uint8_t stripe, uint8_t sensor, uint8_t sensor_index)
    {
        animation_set_sensor_index(stripe, led_stripe_status[stripe].current_animation, sensor, sensor_index);
    }

    uint8_t animation_get_current_sensor_index(uint8_t stripe, uint8_t sensor)
    {
        return animation_get_sensor_index(stripe, led_stripe_status[stripe].current_animation, sensor);
    }

    void animation_set_current_option_0(uint8_t stripe, uint8_t option_value)
    {
        animation_set_option(stripe, led_stripe_status[stripe].current_animation, 0, option_value);
    }

    void animation_set_current_option_1(uint8_t stripe, uint8_t option_value)
    {
        animation_set_option(stripe, led_stripe_status[stripe].current_animation, 1, option_value);
    }

    uint8_t animation_get_current_option_0(uint8_t stripe)
    {
        return animation_get_option(stripe, led_stripe_status[stripe].current_animation, 0);
    }

    uint8_t animation_get_current_option_1(uint8_t stripe)
    {
        return animation_get_option(stripe, led_stripe_status[stripe].current_animation, 1);
    }

    void animation_set_option(uint8_t stripe, uint8_t animation, uint8_t option_id, uint8_t option_value)
    {
        led_stripe_status[stripe].animations[animation].option[option_id] = option_value;

        if (led_stripe_status[stripe].current_animation == animation && led_stripe[stripe].animation)
        {
            if (option_id == 0)
                led_stripe[stripe].animation->setOption(option_value);
            else if (option_id == 1)
                led_stripe[stripe].animation->setOption2(option_value);
        }

        // LV_("aso %u %u %u", stripe, (animation), option_value);
    }

    uint8_t animation_get_option(uint8_t stripe, uint8_t animation, uint8_t option_id)
    {
        return led_stripe_status[stripe].animations[animation].option[option_id];
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

    bool animation_is_autoswitch_to_sensor_animation(uint8_t stripe)
    {
        return led_stripe_status[stripe].is_autoswitch_sensor_animation;
    }

    void emergency_shutdown_power_on_supply_overheat()
    {
        if (!emergency_shutdown)
        {
            emergency_shutdown = true;
            animation_all_back();
        }
    }

    bool is_emergency_shutdown_power_on_supply_overheating()
    {
        int8_t temp = sensors_get_value(SENSOR_SUPPLY);
        return (temp > 45);
    }

    bool contraint_for_sensor_animation_matches(uint8_t stripe)
    {
        uint8_t sensor_animation = animation_get_sensor_animation(stripe);

        int16_t sns_current_value = sensors_get_value16(animation_get_sensor_index(stripe, sensor_animation, SENSOR_IDX_CURRENT));
        int16_t sns_ref_value = sensors_get_value16(animation_get_sensor_index(stripe, sensor_animation, SENSOR_IDX_REF));

        // LV_("[%u] idx %u %u %u", stripe, sensor_animation, animation_get_sensor_index(stripe, sensor_animation, SENSOR_IDX_CURRENT),
        //    animation_get_sensor_index(stripe, sensor_animation, SENSOR_IDX_REF));

        if (sns_current_value < sns_ref_value)
            return false;

        int16_t delta = sns_current_value - sns_ref_value;

        LV_("[%u] sns %i %i d:%i", stripe, sns_current_value, sns_ref_value, delta);

        bool matches = (delta > TDELTA);

        if (matches && !is_constraint_matched)
        {
            is_constraint_matched = true;
            tmax = sns_current_value;
        }

        if (is_constraint_matched && (tmax < sns_current_value))
        {
            tmax = sns_current_value;
        }

        if (is_constraint_matched && (delta < (tmax - sns_ref_value) / 2))
        {
            return false;
        }

        if (!matches && is_constraint_matched)
        {
            is_constraint_matched = false;
            tmax = 0;
        }

        return matches;
    }

    void switch_to_sensor_animation_if_constraint_matches(uint8_t stripe)
    {
        if (!animation_is_autoswitch_to_sensor_animation(stripe) || animation_get_autoplay(stripe))
            return;

        uint8_t requested_animation;

        if (contraint_for_sensor_animation_matches(stripe))
        {
            // LV_("[%u] to sensor ani", stripe);
            requested_animation = animation_get_sensor_animation(stripe);
        }
        else
        {
            // LV_("[%u] to current ani", stripe);
            requested_animation = animation_get_current_animation(stripe);
        }

        if (requested_animation != animation_get_active_animation(stripe))
        {
            // LV_("[%u] run animation %u", stripe, requested_animation);

            bool animation_was_running = animation_get_running(stripe);

            animation_stop(stripe);
            animation_set_for_stripe_by_index(stripe, requested_animation);

            if (requested_animation == animation_get_sensor_animation(stripe))
            {
                led_stripe[stripe].is_current_animation_running_before_switching = animation_was_running;
                // LV_("[%u] cur run %u", stripe, led_stripe[stripe].is_current_animation_running_before_switching);
                animation_start(stripe);
            }
            else
            {
                if (led_stripe[stripe].is_current_animation_running_before_switching)
                {
                    // LV_("[%u] restart cur ani", stripe);
                    animation_start(stripe);
                }
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
                switch_to_sensor_animation_if_constraint_matches(stripe);
            }

            fld_fps = FastLED.getFPS() / 2;
            // fld_fps = FastLED.getFPS();

#if DEBUG_OUTPUT_SUPPORTED && defined(ANIMATION_SHOW_FPS)
            LV_("fps: %u", fld_fps);
#endif
        }

        if (active_animation_count == 0)
            return;

        if (is_emergency_shutdown_power_on_supply_overheating())
        {
            emergency_shutdown_power_on_supply_overheat();
            return;
        }
        else
        {
            if (emergency_shutdown)
                emergency_shutdown = false;
        }

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

                if (led_stripe_status[stripe].autoplay &&
                    timer_elapsed32(led_stripe[stripe].autoplay_timer) >= led_stripe_status[stripe].autoplay_delay_msecs)
                {
                    animation_set_next_animation_active(stripe);
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

#ifdef __cplusplus
}
#endif
#endif
