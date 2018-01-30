
#include "shell_animation.h"
#include "animation/animation.h"
#include "sensor.h"
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>

#include "ledstripe_debug.h"

#if FASTLED_SUPPORTED

#define DEBUG_FUNCTIONS_SUPPORTED
#define SPRINTF(fmt, args...) sprintf_P(es_output_buffer, PSTR(fmt), args)
#define CATSPRINTF(fmt, args...)                                                                                                                     \
    {                                                                                                                                                \
        int8_t cnt = sprintf_P(es_output_buffer, PSTR(fmt), args);                                                                                   \
        es_output_buffer += cnt;                                                                                                                     \
    }
#define CATOUT(fmt)                                                                                                                                  \
    {                                                                                                                                                \
        int8_t cnt = sprintf_P(es_output_buffer, PSTR(fmt));                                                                                         \
        es_output_buffer += cnt;                                                                                                                     \
    }

extern char *es_output_buffer;
extern uint16_t es_output_buffer_len;

bool animation_cmd_status(uint8_t argc, char **argv);
bool animation_cmd_run(uint8_t argc, char **argv);
bool animation_cmd_configuration(uint8_t argc, char **argv);
bool animation_cmd_global_configuration(uint8_t argc, char **argv);
bool animation_cmd_sensor(uint8_t argc, char **argv);
bool animation_cmd_settings(uint8_t argc, char **argv);

#ifdef DEBUG_FUNCTIONS_SUPPORTED
#endif

const struct _s_shell_cmd animation_shell_cmd[] PROGMEM = {
    SHELLCMD("sta", animation_cmd_status, "#", "animation status"),
    SHELLCMD("run", animation_cmd_run, "# [0/1]", "start/stop animation"),
    SHELLCMD("cfg", animation_cmd_configuration, "# ani|fps|apl|sns|opt #", "animation configuration"),
    SHELLCMD("glo", animation_cmd_global_configuration, "[bri #]", "global animation configuration"),
    SHELLCMD("set", animation_cmd_settings, "[#] load|save", "load/save animation settings"),
    SHELLCMD("sns", animation_cmd_sensor, "# [v]", "get/set sensor value"),
#ifdef DEBUG_FUNCTIONS_SUPPORTED
#endif
    SHELLCMD(0, 0, 0, 0)};

void animation_dump_status(uint8_t stripe)
{
    SPRINTF(".str %u\n", stripe);
    SPRINTF(".ani %u\n", animation_get_current(stripe));
    SPRINTF(".run %u\n", animation_get_running(stripe));
    SPRINTF(".apl %u\n", animation_get_autoplay(stripe));
    SPRINTF(".fps %u\n", animation_get_current_fps(stripe));
    SPRINTF(".opt %u\n", animation_get_current_option(stripe));
    SPRINTF(".sn0 %u\n", animation_get_current_sensor_index(stripe, 0));
    SPRINTF(".sn1 %u\n", animation_get_current_sensor_index(stripe, 1));

    uint8_t hsv[3];
    animation_get_current_color(stripe, 0, hsv);
    SPRINTF(".co0 %x %x %x\n", hsv[0], hsv[1], hsv[2]);
    animation_get_current_color(stripe, 1, hsv);
    SPRINTF(".co1 %x %x %x\n", hsv[0], hsv[1], hsv[2]);
}

#ifdef DEBUG_FUNCTIONS_SUPPORTED
#endif

bool animation_cmd_settings(uint8_t argc, char **argv)
{
    if (argc == 0)
        return false;

    if (argc == 1)
    {
        if (strcmp_P(argv[0], PSTR("load")) == 0)
        {
            animation_load(false);
        }
        else if (strcmp_P(argv[0], PSTR("clear")) == 0)
        {
            animation_load(true);
        }
        else
        {
            animation_save();
        }
    }
    else
    {
        uint8_t stripe = atoi(argv[0]);
        if (stripe >= MAX_LED_STRIPES)
            return false;

        if (strcmp_P(argv[1], PSTR("load")) == 0)
        {
            animation_load_stripe(stripe);
        }
        else
        {
            animation_save_stripe(stripe);
        }

        SPRINTF(".str %u\n", stripe);
    }

    return true;
}

bool animation_cmd_global_configuration(uint8_t argc, char **argv)
{
    CATOUT("glo ");

    if (argc == 2)
    {
        uint8_t value = atoi(argv[1]);

        if (strcmp_P(argv[0], PSTR("bri")) == 0)
        {
            animation_set_global_brightness(value);
        }
        else
        {
            return false;
        }
    }

    if (argc >= 1)
    {
        if (strcmp_P(argv[0], PSTR("bri")) == 0)
        {
            SPRINTF("bri %u", animation_get_global_brightness());
        }
        else if (strcmp_P(argv[0], PSTR("fps")) == 0)
        {
            SPRINTF("fps %u", get_current_fld_fps());
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool animation_cmd_sensor(uint8_t argc, char **argv)
{
    if (argc < 1)
        return false;

    uint8_t index = atoi(argv[0]);
    if (index >= MAX_SENSORS)
        return false;

    CATOUT("sns ");

    if (argc == 2)
    {
        int8_t value = atoi(argv[1]);
        sensors_set_value(index, value);
    }

    if (argc == 3)
    {
        int8_t value = atoi(argv[1]);
        uint8_t fraction = atoi(argv[2]);
        sensors_set_value(index, value);
        sensors_set_fraction(index, fraction);
    }

    SPRINTF("%u %u %u", index, sensors_get_value(index), sensors_get_fraction(index));

    return true;
}

bool animation_cmd_configuration(uint8_t argc, char **argv)
{
    // str cfg 0 ani [#]

    LV_("cfg argc %u", argc);

    if (argc < 1)
        return false;

    uint8_t stripe = atoi(argv[0]);
    if (stripe >= MAX_LED_STRIPES)
        return false;

    CATSPRINTF("cfg %u ", stripe);

    if (argc == 3)
    {
        uint8_t argv3 = atoi(argv[2]);

        if (strcmp_P(argv[1], PSTR("ani")) == 0)
        {
            animation_set_current(stripe, argv3);
        }

        else if (strcmp_P(argv[1], PSTR("fps")) == 0)
        {
            animation_set_current_fps(stripe, argv3);
        }

        else if (strcmp_P(argv[1], PSTR("apl")) == 0)
        {
            animation_set_autoplay(stripe, argv3);
        }

        else if (strcmp_P(argv[1], PSTR("opt")) == 0)
        {
            animation_set_current_option(stripe, argv3);
        }

        else if (strcmp_P(argv[1], PSTR("sn0")) == 0)
        {
            animation_set_current_sensor_index(stripe, 0, argv3);
        }

        else if (strcmp_P(argv[1], PSTR("sn1")) == 0)
        {
            animation_set_current_sensor_index(stripe, 1, argv3);
        }

        else
        {
            return false;
        }
    }
    else if (argc == 5)
    {
        uint8_t hsv[3];
        hsv[0] = atoi(argv[2]);
        hsv[1] = atoi(argv[3]);
        hsv[2] = atoi(argv[4]);

        if (strcmp_P(argv[1], PSTR("co0")) == 0)
        {
            animation_set_current_color(stripe, 0, hsv);
        }

        else if (strcmp_P(argv[1], PSTR("co1")) == 0)
        {
            animation_set_current_color(stripe, 1, hsv);
        }

        else
        {
            return false;
        }
    }

    if (argc >= 1)
    {
        // str cfg 0 ani [#]

        SPRINTF("cfg %u ", stripe);

        if (strcmp_P(argv[1], PSTR("ani")) == 0)
        {
            SPRINTF("ani %u", animation_get_current(stripe));
        }

        else if (strcmp_P(argv[1], PSTR("fps")) == 0)
        {
            SPRINTF("fps %u", animation_get_current_fps(stripe));
        }

        else if (strcmp_P(argv[1], PSTR("apl")) == 0)
        {
            SPRINTF("apl %u", animation_get_autoplay(stripe));
        }

        else if (strcmp_P(argv[1], PSTR("opt")) == 0)
        {
            SPRINTF("opt %u", animation_get_current_option(stripe));
        }

        else if (strcmp_P(argv[1], PSTR("sn0")) == 0)
        {
            SPRINTF("sn0 %u", animation_get_current_sensor_index(stripe, 0));
        }

        else if (strcmp_P(argv[1], PSTR("sn1")) == 0)
        {
            SPRINTF("sn1 %u", animation_get_current_sensor_index(stripe, 1));
        }

        else if (strcmp_P(argv[1], PSTR("co0")) == 0)
        {
            uint8_t hsv[3];
            animation_get_current_color(stripe, 0, hsv);
            SPRINTF("co0 %u %u %u", hsv[0], hsv[1], hsv[2]);
        }

        else if (strcmp_P(argv[1], PSTR("co1")) == 0)
        {
            uint8_t hsv[3];
            animation_get_current_color(stripe, 1, hsv);
            SPRINTF("co1 %u %u %u", hsv[0], hsv[1], hsv[2]);
        }

        else
        {
            return false;
        }
    }

    // animation_dump_status(stripe);

    return true;
}

bool animation_cmd_run(uint8_t argc, char **argv)
{
    if (argc < 1)
        return false;

    uint8_t stripe = atoi(argv[0]);
    if (stripe >= MAX_LED_STRIPES)
        return false;

    if (argc >= 2)
    {
        bool run = atoi(argv[1]);
        animation_set_running(stripe, run);
    }

    SPRINTF("run %u %u", stripe, animation_get_running(stripe));

    return true;
}

bool animation_cmd_status(uint8_t argc, char **argv)
{
    if (argc != 1)
        return false;

    uint8_t stripe = atoi(argv[0]);
    if (stripe >= MAX_LED_STRIPES)
        return false;

    animation_dump_status(stripe);

    return true;
}

#endif
