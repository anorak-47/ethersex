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
// led_strand_animation_status_t led_strand_status_eeprom[MAX_LED_STRIPES] EEMEM;

strand_configuration_t strand_config[MAX_LED_STRIPES];
animation_state_t animation_state[MAX_LED_STRIPES];

static int8_t active_animation_count = 0;
static uint16_t fld_fps = 0;
static uint32_t now = 0;
static bool animated = false;

static int8_t tmax = 0;
static bool emergency_shutdown = false;

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

    for (uint8_t a = 0; a < ANIMATION_COUNT; a++)
    {
        // sensor index defaults for strand 0
        strand_config[0].animations[a].sensor_index[SENSOR_IDX_CURRENT] = SENSOR_BATHTUB;
        strand_config[0].animations[a].sensor_index[SENSOR_IDX_REF] = SENSOR_REFERENCE;

        // sensor index defaults for strand 1
        strand_config[1].animations[a].sensor_index[SENSOR_IDX_CURRENT] = SENSOR_SHOWER;
        strand_config[1].animations[a].sensor_index[SENSOR_IDX_REF] = SENSOR_REFERENCE;
    }
}

void animation_reset(void)
{
    active_animation_count = 0;

    set_strands_global_brightness(FASTLED_DEFAULT_BRIGHTNESS);

    for (uint8_t strand = 0; strand < MAX_LED_STRIPES; strand++)
    {
        animation_stop(strand);

        memset(&animation_state[strand], 0, sizeof(animation_state_t));
        animation_state[strand].delay_msecs = FPS_TO_DELAY(10);
        animation_state[strand].animation = 0;

        memset(&strand_config[strand], 0, sizeof(strand_configuration_t));

        strand_config[strand].id = 9;
        strand_config[strand].autoplay = false;
        strand_config[strand].autostart_on_sensor = true;
        strand_config[strand].autostart_sensor_delta = TDELTA;
        strand_config[strand].autoplay_delay_msecs = 120000;

        for (uint8_t a = 0; a < ANIMATION_COUNT; a++)
        {
            strand_config[strand].animations[a].fps = 25;
            strand_config[strand].animations[a].sensor_index[SENSOR_IDX_CURRENT] = SENSOR_BATHTUB;
            strand_config[strand].animations[a].sensor_index[SENSOR_IDX_REF] = SENSOR_REFERENCE;
            strand_config[strand].animations[a].hsv[0] = CHSV(0, 255, 255);
            strand_config[strand].animations[a].hsv[1] = CHSV(127, 255, 255);
        }

        create_animation_for_strand(strand, strand_config[strand].id);
    }

    animation_reset_sensor_defaults();
}

void load_strand_config(uint8_t strand)
{
    eeprom_read_block((void *)&strand_config[strand], EEPROM_POS_STATUS + (sizeof(strand_configuration_t) * strand),
                      sizeof(strand_configuration_t));
}

void animation_load(bool clear)
{
    animation_reset();

    set_strands_global_brightness(eeprom_read_byte(&global_brightness_eeprom));

    if (clear)
        return;

    uint8_t magic_0 = eeprom_read_byte(EEPROM_POS_CHK_0);
    uint8_t magic_1 = eeprom_read_byte(EEPROM_POS_CHK_1);

    if (magic_0 == EEPROM_CHK_MAGIC_0 && magic_1 == EEPROM_CHK_MAGIC_1)
    {
        for (uint8_t strand = 0; strand < MAX_LED_STRIPES; strand++)
        {
            load_strand_config(strand);
            create_animation_for_strand(strand, strand_config[strand].id);
        }
    }
}

void save_strand_config(uint8_t strand)
{
    eeprom_update_block((void *)&strand_config[strand], EEPROM_POS_STATUS + (sizeof(strand_configuration_t) * strand),
                        sizeof(strand_configuration_t));
}

void animation_save(void)
{
    eeprom_update_byte(&global_brightness_eeprom, animation_get_global_brightness());

    for (uint8_t i = 0; i < MAX_LED_STRIPES; i++)
        save_strand_config(i);

    eeprom_update_byte(EEPROM_POS_CHK_0, EEPROM_CHK_MAGIC_0);
    eeprom_update_byte(EEPROM_POS_CHK_1, EEPROM_CHK_MAGIC_1);
}

void animation_init(void)
{
    // This hopefully calls the Arduino init() from Arduino.h
    init();

    animation_load(false);
    led_strands_setup();
}

void animation_start(uint8_t strand)
{
    LV_("animation_start s:%u r:%u", strand, animation_state[strand].is_running);

    if (!animation_state[strand].is_running && animation_state[strand].animation == 0)
        create_animation_for_strand(strand, strand_config[strand].id);

    if (!animation_state[strand].is_running && animation_state[strand].animation)
    {
        active_animation_count++;
        uint8_t id = strand_config[strand].id;

        animation_state[strand].is_running = true;
        animation_state[strand].loop_timer = millis();
        animation_state[strand].autoplay_timer = millis();
        animation_state[strand].delay_msecs = FPS_TO_DELAY(strand_config[strand].animations[id].fps);

        animation_state[strand].animation->initialize();
        animation_state[strand].animation->setOption(strand_config[strand].animations[id].option[0]);
        animation_state[strand].animation->setOption2(strand_config[strand].animations[id].option[1]);

        LV_("animation_start cnt %u", active_animation_count);
    }
}

void animation_stop(uint8_t strand)
{
    LV_("animation_stop s:%u r:%u", strand, led_strand[strand].is_running);

    if (animation_state[strand].is_running)
    {
        animation_state[strand].is_running = false;
        animation_state[strand].was_autostarted = false;

        if (animation_state[strand].animation)
            animation_state[strand].animation->deinitialize();

        active_animation_count--;

        if (active_animation_count <= 0)
        {
            if (active_animation_count < 0)
            {
                LV_("ERROR: active_animation_count < 0");
                active_animation_count = 0;
            }

            led_strands_prepare_before_show();
            FastLED.show();
        }

        LV_("animation_stop cnt %u", active_animation_count);
    }
}

void set_strand_animation_running(uint8_t strand, bool run)
{
    LV_("set_running s:%u r:%u", strand, run);

    if (run)
        animation_start(strand);
    else
        animation_stop(strand);
}

uint8_t get_animation_index(uint8_t strand)
{
    return strand_config[strand].id;
}

void set_animation_by_index(uint8_t strand, uint8_t animation)
{
    strand_config[strand].id = animation;
    animation_state[strand].was_autostarted = false;
}

void run_animation_by_index(uint8_t strand, uint8_t animation)
{
    set_animation_by_index(strand, animation);

    if (is_strand_animation_running(strand))
    {
        animation_state[strand].was_autostarted = false;

        animation_stop(strand);
        animation_start(strand);
    }
}

uint8_t run_next_animation(uint8_t strand)
{
    uint8_t id = strand_config[strand].id;
    if (id < ANIMATION_COUNT - 1)
    {
        id++;
    }
    else
    {
        id = 0;
    }

    run_animation_by_index(strand, id);

    return id;
}

uint8_t animation_set_previous_animation_active(uint8_t strand)
{
    uint8_t id = strand_config[strand].id;
    if (id > 0)
    {
        id--;
    }
    else
    {
        id = ANIMATION_COUNT - 2;
    }

    run_animation_by_index(strand, id);

    return id;
}

bool is_strand_animation_running(uint8_t strand)
{
    return animation_state[strand].is_running;
}

void set_strand_fps(uint8_t strand, uint8_t fps)
{
    set_animation_fps(strand, strand_config[strand].id, fps);
}

uint8_t get_strand_fps(uint8_t strand)
{
    return DELAY_TO_FPS(animation_state[strand].delay_msecs);
}

void set_animation_fps(uint8_t strand, uint8_t animation, uint8_t fps)
{
    strand_config[strand].animations[animation].fps = fps;

    if (strand_config[strand].id == animation)
    {
        animation_state[strand].delay_msecs = FPS_TO_DELAY(fps);
    }

    // LV_("afps %u %u %u", strand, fps, led_strand[strand].delay_msecs);
}

uint8_t get_animation_fps(uint8_t strand, uint8_t animation)
{
    return strand_config[strand].animations[animation].fps;
}

bool get_strand_autoplay_delay_time(uint8_t strand)
{
    return strand_config[strand].autoplay;
}

void set_strand_autoplay(uint8_t strand, bool autoplay)
{
    strand_config[strand].autoplay = autoplay;
}

uint16_t get_strand_autoplay_delay_time(uint8_t strand)
{
    return strand_config[strand].autoplay_delay_msecs / 1000UL;
}

void set_strand_autoplay_delay_time(uint8_t strand, uint16_t autoplay_delay_secs)
{
    strand_config[strand].autoplay_delay_msecs = 1000UL * autoplay_delay_secs;
}

void set_animation_color(uint8_t strand, uint8_t animation, uint8_t cnr, uint8_t *hsv)
{
    if (cnr > 1)
        return;
    // LV_("sc %u %u %u", strand, animation, cnr);
    // LV_("sc %u %u %u", hsv[0], hsv[1], hsv[2]);
    strand_config[strand].animations[animation].hsv[cnr] = CHSV(hsv[0], hsv[1], hsv[2]);
    // LV_("sc %u %u %u",
    // led_strand_status[strand].animations[animation].hsv[cnr].hue,
    //    led_strand_status[strand].animations[animation].hsv[cnr].saturation,
    //    led_strand_status[strand].animations[animation].hsv[cnr].value);
    if (animation_state[strand].animation)
        animation_state[strand].animation->update();
}

void get_animation_color(uint8_t strand, uint8_t animation, uint8_t cnr, uint8_t *hsv)
{
    if (cnr > 1)
        return;
    // LV_("gc %u %u %u", strand, animation, cnr);
    // LV_("gc %u %u %u",
    // led_strand_status[strand].animations[animation].hsv[cnr].hue,
    //    led_strand_status[strand].animations[animation].hsv[cnr].saturation,
    //    led_strand_status[strand].animations[animation].hsv[cnr].value);
    hsv[0] = strand_config[strand].animations[animation].hsv[cnr].hue;
    hsv[1] = strand_config[strand].animations[animation].hsv[cnr].saturation;
    hsv[2] = strand_config[strand].animations[animation].hsv[cnr].value;
    // LV_("gc %u %u %u", hsv[0], hsv[1], hsv[2]);
}

void set_strand_color(uint8_t strand, uint8_t cnr, uint8_t hsv[3])
{
    set_animation_color(strand, strand_config[strand].id, cnr, hsv);
}

void get_strand_color(uint8_t strand, uint8_t cnr, uint8_t hsv[3])
{
    get_animation_color(strand, strand_config[strand].id, cnr, hsv);
}

void animation_set_sensor_index(uint8_t strand, uint8_t animation, uint8_t sensor, uint8_t sensor_index)
{
    if (sensor_index >= MAX_SENSORS || sensor > 1)
        return;

    strand_config[strand].animations[animation].sensor_index[sensor] = sensor_index;

    if (animation_state[strand].animation)
        animation_state[strand].animation->update();
}

uint8_t animation_get_sensor_index(uint8_t strand, uint8_t animation, uint8_t sensor)
{
    if (sensor > 1)
        return 0;
    return strand_config[strand].animations[animation].sensor_index[sensor];
}

void animation_set_current_sensor_index(uint8_t strand, uint8_t sensor, uint8_t sensor_index)
{
    animation_set_sensor_index(strand, strand_config[strand].id, sensor, sensor_index);
}

uint8_t animation_get_current_sensor_index(uint8_t strand, uint8_t sensor)
{
    return animation_get_sensor_index(strand, strand_config[strand].id, sensor);
}

void set_strand_option_0(uint8_t strand, uint8_t option_value)
{
    set_animation_option(strand, strand_config[strand].id, 0, option_value);
}

void set_strand_option_1(uint8_t strand, uint8_t option_value)
{
    set_animation_option(strand, strand_config[strand].id, 1, option_value);
}

uint8_t get_strand_option_0(uint8_t strand)
{
    return get_animation_option(strand, strand_config[strand].id, 0);
}

uint8_t get_strand_option_1(uint8_t strand)
{
    return get_animation_option(strand, strand_config[strand].id, 1);
}

void set_animation_option(uint8_t strand, uint8_t animation, uint8_t option_id, uint8_t option_value)
{
    strand_config[strand].animations[animation].option[option_id] = option_value;

    if (strand_config[strand].id == animation && animation_state[strand].animation)
    {
        if (option_id == 0)
            animation_state[strand].animation->setOption(option_value);
        else if (option_id == 1)
            animation_state[strand].animation->setOption2(option_value);

        animation_state[strand].animation->update();
    }

    // LV_("aso %u %u %u", strand, (animation), option_value);
}

uint8_t get_animation_option(uint8_t strand, uint8_t animation, uint8_t option_id)
{
    return strand_config[strand].animations[animation].option[option_id];
}

void set_strands_global_brightness(uint8_t brightness)
{
    FastLED.setBrightness(brightness);
}

uint8_t get_strands_global_brightness()
{
    return FastLED.getBrightness();
}

uint16_t get_real_fastled_fps()
{
    return fld_fps;
}

void set_strand_autorun(uint8_t strand, bool on)
{
    strand_config[strand].autostart_on_sensor = on;
}

bool get_strand_autorun(uint8_t strand)
{
    return strand_config[strand].autostart_on_sensor;
}

void set_strand_autorun_sensor_delta(uint8_t strand, int16_t delta)
{
    strand_config[strand].autostart_on_sensor = delta;
}

int16_t get_strand_autorun_sensor_delta(uint8_t strand)
{
    return strand_config[strand].autostart_on_sensor;
}

bool was_strand_autostarted(uint8_t strand)
{
    return animation_state[strand].was_autostarted;
}

uint16_t get_strand_sensor_delta(uint8_t strand)
{
    return animation_state[strand].sensor_delta;
}

void emergency_shutdown_power_on_supply_overheat()
{
    if (!emergency_shutdown)
    {
        emergency_shutdown = true;
        led_strands_all_back();
    }
}

bool is_emergency_shutdown_power_on_supply_overheating()
{
    int8_t temp = sensors_get_value(SENSOR_SUPPLY);
    return (temp > 45);
}

bool constraint_for_sensor_animation_matches(uint8_t strand)
{
    uint8_t animation = get_animation_index(strand);

    int16_t sns_current_value = sensors_get_value16(animation_get_sensor_index(strand, animation, SENSOR_IDX_CURRENT));
    int16_t sns_ref_value = sensors_get_value16(animation_get_sensor_index(strand, animation, SENSOR_IDX_REF));

    // LV_("[%u] idx %u %u %u", strand, sensor_animation, animation_get_sensor_index(strand, sensor_animation, SENSOR_IDX_CURRENT),
    //    animation_get_sensor_index(strand, sensor_animation, SENSOR_IDX_REF));

    if (sns_current_value < sns_ref_value)
        return false;

    int16_t delta = sns_current_value - sns_ref_value;
    animation_state[strand].sensor_delta = delta;

    LV_("[%u] sns %i %i d:%i", strand, sns_current_value, sns_ref_value, delta);

    bool matches = (delta > strand_config[strand].autostart_sensor_delta);

    if (matches && !animation_state[strand].is_autostart_constraint_matching)
    {
        animation_state[strand].is_autostart_constraint_matching = true;
        tmax = sns_current_value;
    }

    if (animation_state[strand].is_autostart_constraint_matching && (tmax < sns_current_value))
    {
        tmax = sns_current_value;
    }

    if (animation_state[strand].is_autostart_constraint_matching && (delta < (tmax - sns_ref_value) / 2))
    {
        return false;
    }

    if (!matches && animation_state[strand].is_autostart_constraint_matching)
    {
        animation_state[strand].is_autostart_constraint_matching = false;
        tmax = 0;
    }

    return matches;
}

void start_animation_if_constraint_matches(uint8_t strand)
{
    if (!get_strand_autorun(strand) || get_strand_autoplay(strand))
        return;

    uint8_t requested_animation;

    if (constraint_for_sensor_animation_matches(strand))
    {
        if (!animation_state[strand].is_running)
        {
            animation_state[strand].was_autostarted = !(animation_state[strand].is_running);
            animation_start(strand);
        }
    }
    else
    {
        if (animation_state[strand].is_running && animation_state[strand].was_autostarted)
        {
            animation_stop(strand);
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

        for (uint8_t strand = 0; strand < MAX_LED_STRIPES; strand++)
        {
            start_animation_if_constraint_matches(strand);
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

    animated = false;

    for (uint8_t strand = 0; strand < MAX_LED_STRIPES; strand++)
    {
        if (animation_state[strand].is_running && animation_state[strand].animation)
        {
            if (timer_elapsed(animation_state[strand].loop_timer) >= animation_state[strand].delay_msecs)
            {
                animation_state[strand].loop_timer = now;
                animated = animation_state[strand].animation->loop() || animated;
            }

            if (strand_config[strand].autoplay &&
                timer_elapsed32(animation_state[strand].autoplay_timer) >= strand_config[strand].autoplay_delay_msecs)
            {
                run_next_animation(strand);
            }
        }
    }

    // 1000/9 > 100fps.
    // 1000/19 > 50fps.
    // FPS are limited by lenght of the strands because of slow protocol
    // Dithering works only if current FPS is greater than 100
    // Dithering with a frame rate below 100 results in flickering LEDs...
    EVERY_N_MILLISECONDS(9)
    {
        if (animated)
        {
            led_strands_prepare_before_show();
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
