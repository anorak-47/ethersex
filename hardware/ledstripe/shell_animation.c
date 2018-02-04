
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
bool animation_cmd_cfg_animation(uint8_t argc, char **argv);
bool animation_cmd_global_configuration(uint8_t argc, char **argv);
bool animation_cmd_sensor(uint8_t argc, char **argv);
bool animation_cmd_settings(uint8_t argc, char **argv);
bool animation_cmd_help(uint8_t argc, char **argv);

#ifdef DEBUG_FUNCTIONS_SUPPORTED
#endif

const struct _s_shell_cmd animation_shell_cmd[] PROGMEM = {
    SHELLCMD("help", animation_cmd_help, "", "this help"),
    SHELLCMD("sta", animation_cmd_status, "[#]", "status"),
    SHELLCMD("run", animation_cmd_run, "# [0/1]", "start/stop"),
    SHELLCMD("cfg", animation_cmd_configuration, "# ani|ans|apl|asw #", "stripe config"),
    SHELLCMD("ani", animation_cmd_cfg_animation, "# fps|opt|sn|co # [#]", "animation config"),
    SHELLCMD("glo", animation_cmd_global_configuration, "[bri #]", "global configuration"),
    SHELLCMD("set", animation_cmd_settings, "[#] load|clear|save", "load/clear/save settings"),
    SHELLCMD("sns", animation_cmd_sensor, "[#] [v] [f]", "get/set sensor value (fraction)"),
#ifdef DEBUG_FUNCTIONS_SUPPORTED
#endif
    SHELLCMD(0, 0, 0, 0)};

#ifdef DEBUG_FUNCTIONS_SUPPORTED
#endif

bool animation_cmd_help(uint8_t argc, char **argv)
{
    struct _s_shell_cmd shell_cmd;
    uint8_t cmdid = 0;

    memcpy_P(&shell_cmd, &animation_shell_cmd[cmdid++], sizeof(struct _s_shell_cmd));

    while (shell_cmd.name != 0)
    {
#if defined(SHELL_ARGS_HELP_SUPPORTED) || defined(SHELL_HELP_SUPPORTED)
        CATSPRINTF("%s: %s\n", shell_cmd.name, shell_cmd.cmd_help);
        if (shell_cmd.args_help[0] != '\0')
            CATSPRINTF(" %s\n", shell_cmd.args_help);
#else
#if defined(SHELL_ARGS_HELP_SUPPORTED)
        CATSPRINTF("%s:\n", shell_cmd.name);
        if (shell_cmd.args_help[0] != '\0')
            CATSPRINTF(" %s\n", shell_cmd.args_help);
#else
#if defined(SHELL_HELP_SUPPORTED)
        CATSPRINTF("%s: %s\n", shell_cmd.name, shell_cmd.cmd_help);
#else
        CATSPRINTF("%s\n", shell_cmd.name);
#endif
#endif
#endif

        memcpy_P(&shell_cmd, &animation_shell_cmd[cmdid++], sizeof(struct _s_shell_cmd));
    }

    return true;
}

void animation_dump_current_status(uint8_t stripe)
{
    CATSPRINTF("sta %u\n", stripe);
    CATSPRINTF(".run %u\n", animation_get_running(stripe));
    CATSPRINTF(".anr %u\n", animation_get_active_animation(stripe));
    CATSPRINTF(".ani %u\n", animation_get_current_animation(stripe));
    CATSPRINTF(".asn %u\n", animation_get_sensor_animation(stripe));
    CATSPRINTF(".asw %u\n", animation_get_autoswitch_sensor_animation(stripe));
    CATSPRINTF(".apl %u\n", animation_get_autoplay(stripe));
    CATSPRINTF(".fps %u\n", animation_get_current_fps(stripe));
    CATSPRINTF(".opt %u\n", animation_get_current_option(stripe));
    CATSPRINTF(".sn0 %u\n", animation_get_current_sensor_index(stripe, 0));
    CATSPRINTF(".sn1 %u\n", animation_get_current_sensor_index(stripe, 1));
    uint8_t hsv[3];
    animation_get_current_color(stripe, 0, hsv);
    CATSPRINTF(".co0 %x %x %x\n", hsv[0], hsv[1], hsv[2]);
    animation_get_current_color(stripe, 1, hsv);
    CATSPRINTF(".co1 %x %x %x\n", hsv[0], hsv[1], hsv[2]);
}

void animation_dump_status(uint8_t stripe, animation_names animation)
{
    CATSPRINTF("sta %u\n", stripe);
    CATSPRINTF(".run %u\n", animation_get_running(stripe));
    CATSPRINTF(".anr %u\n", animation_get_active_animation(stripe));
    CATSPRINTF(".ani %u\n", animation_get_current_animation(stripe));
    CATSPRINTF(".asn %u\n", animation_get_sensor_animation(stripe));
    CATSPRINTF(".asw %u\n", animation_get_autoswitch_sensor_animation(stripe));
    CATSPRINTF(".apl %u\n", animation_get_autoplay(stripe));
    CATSPRINTF(".fps %u\n", animation_get_fps(stripe, animation));
    CATSPRINTF(".opt %u\n", animation_get_option(stripe, animation));
    CATSPRINTF(".sn0 %u\n", animation_get_sensor_index(stripe, animation, 0));
    CATSPRINTF(".sn1 %u\n", animation_get_sensor_index(stripe, animation, 1));
    uint8_t hsv[3];
    animation_get_color(stripe, animation, 0, hsv);
    CATSPRINTF(".co0 %x %x %x\n", hsv[0], hsv[1], hsv[2]);
    animation_get_color(stripe, animation, 1, hsv);
    CATSPRINTF(".co1 %x %x %x\n", hsv[0], hsv[1], hsv[2]);
}

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
        else if (strcmp_P(argv[0], PSTR("save")) == 0)
        {
            animation_save();
        }
        else
        {
            return false;
        }

        SPRINTF("set %s", argv[0]);
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
        else if (strcmp_P(argv[1], PSTR("save")) == 0)
        {
            animation_save_stripe(stripe);
        }

        SPRINTF("set %u %s", stripe, argv[1]);
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
    {
        for (uint8_t index = 0; index < MAX_SENSORS; ++index)
            CATSPRINTF("sns %u %u %u\n", index, sensors_get_value(index), sensors_get_fraction(index));
        return true;
    }

    CATOUT("sns ");

    if (strcmp_P(argv[0], PSTR("upd")) == 0)
    {
        sensors_update();
        CATOUT("upd");
        return true;
    }

    uint8_t index = atoi(argv[0]);
    if (index >= MAX_SENSORS)
        return false;

    if (argc == 2)
    {
        int8_t value = atoi(argv[1]);
        sensors_set_value(index, value);
    }

    else if (argc == 3)
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
    // str cfg 0 <cmd> [#]
    // LV_("cfg argc %u", argc);

    if (argc < 1)
        return false;

    uint8_t stripe = atoi(argv[0]);
    if (stripe >= MAX_LED_STRIPES)
        return false;

    char *cmd = argv[1];

    CATSPRINTF("cfg %u %s ", stripe, cmd);

    if (argc == 2)
    {
        if (strcmp_P(cmd, PSTR("anc")) == 0)
        {
            animation_set_active_animation(stripe, animation_get_current_animation(stripe));
        }

        else if (strcmp_P(cmd, PSTR("asc")) == 0)
        {
            animation_set_active_animation(stripe, animation_get_sensor_animation(stripe));
        }
    }
    else if (argc == 3)
    {
        //        0  1  2
        // str cfg 0 fps 5

        uint8_t value = atoi(argv[2]);

        if (strcmp_P(cmd, PSTR("ani")) == 0)
        {
            animation_set_current_animation(stripe, value);
        }

        else if (strcmp_P(cmd, PSTR("anc")) == 0)
        {
            animation_set_current_animation(stripe, value);
            animation_set_active_animation(stripe, animation_get_current_animation(stripe));
        }

        else if (strcmp_P(cmd, PSTR("asn")) == 0)
        {
            animation_set_sensor_animation(stripe, value);
        }

        else if (strcmp_P(cmd, PSTR("asc")) == 0)
        {
            animation_set_sensor_animation(stripe, value);
            animation_set_active_animation(stripe, animation_get_sensor_animation(stripe));
        }

        else if (strcmp_P(cmd, PSTR("fps")) == 0)
        {
            animation_set_current_fps(stripe, value);
        }

        else if (strcmp_P(cmd, PSTR("apl")) == 0)
        {
            animation_set_autoplay(stripe, value);
        }

        else if (strcmp_P(cmd, PSTR("asw")) == 0)
        {
            animation_set_autoswitch_sensor_animation(stripe, value);
        }

        else if (strcmp_P(cmd, PSTR("opt")) == 0)
        {
            animation_set_current_option(stripe, value);
        }

        else if (strcmp_P(cmd, PSTR("sn0")) == 0)
        {
            animation_set_current_sensor_index(stripe, 0, value);
        }

        else if (strcmp_P(cmd, PSTR("sn1")) == 0)
        {
            animation_set_current_sensor_index(stripe, 1, value);
        }

        else
        {
            return false;
        }
    }
    else if (argc == 5)
    {
        cmd = argv[1];

        uint8_t hsv[3];
        hsv[0] = atoi(argv[2]);
        hsv[1] = atoi(argv[3]);
        hsv[2] = atoi(argv[4]);

        if (strcmp_P(cmd, PSTR("co0")) == 0)
        {
            animation_set_current_color(stripe, 0, hsv);
        }

        else if (strcmp_P(cmd, PSTR("co1")) == 0)
        {
            animation_set_current_color(stripe, 1, hsv);
        }

        else
        {
            return false;
        }
    }

    if (strcmp_P(cmd, PSTR("ani")) == 0)
    {
        SPRINTF("%u", animation_get_current_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("anc")) == 0)
    {
        SPRINTF("%u", animation_get_current_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("asn")) == 0)
    {
        SPRINTF("%u", animation_get_sensor_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("asc")) == 0)
    {
        SPRINTF("%u", animation_get_sensor_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("anr")) == 0)
    {
        SPRINTF("%u", animation_get_active_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("fps")) == 0)
    {
        SPRINTF("%u", animation_get_current_fps(stripe));
    }

    else if (strcmp_P(cmd, PSTR("apl")) == 0)
    {
        SPRINTF("%u", animation_get_autoplay(stripe));
    }

    else if (strcmp_P(cmd, PSTR("asw")) == 0)
    {
        SPRINTF("%u", animation_get_autoswitch_sensor_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("opt")) == 0)
    {
        SPRINTF("%u", animation_get_current_option(stripe));
    }

    else if (strcmp_P(cmd, PSTR("sn0")) == 0)
    {
        SPRINTF("%u", animation_get_current_sensor_index(stripe, 0));
    }

    else if (strcmp_P(cmd, PSTR("sn1")) == 0)
    {
        SPRINTF("%u", animation_get_current_sensor_index(stripe, 1));
    }

    else if (strcmp_P(cmd, PSTR("co0")) == 0)
    {
        uint8_t hsv[3];
        animation_get_current_color(stripe, 0, hsv);
        SPRINTF("%u %u %u", hsv[0], hsv[1], hsv[2]);
    }

    else if (strcmp_P(cmd, PSTR("co1")) == 0)
    {
        uint8_t hsv[3];
        animation_get_current_color(stripe, 1, hsv);
        SPRINTF("%u %u %u", hsv[0], hsv[1], hsv[2]);
    }

    else
    {
        return false;
    }

    return true;
}

bool animation_cmd_cfg_animation(uint8_t argc, char **argv)
{
    //             0      1     2      3
    // str ani <stripe> <cmd> <ani> [value]
    // str ani <stripe>  co0  <ani>  h s v

    if (argc < 3)
        return false;

    uint8_t stripe = atoi(argv[0]);
    if (stripe >= MAX_LED_STRIPES)
        return false;

    uint8_t animationid = atoi(argv[2]);
    if (animationid >= ANIMATION_COUNT)
        return false;

    char *cmd = argv[1];

    CATSPRINTF("ani %u %s %u", stripe, cmd, animationid);

    if (argc == 4)
    {
        uint8_t value = atoi(argv[3]);

        if (strcmp_P(cmd, PSTR("fps")) == 0)
        {
            animation_set_fps(stripe, animationid, value);
        }

        else if (strcmp_P(cmd, PSTR("opt")) == 0)
        {
            animation_set_option(stripe, animationid, value);
        }

        else if (strcmp_P(cmd, PSTR("sn0")) == 0)
        {
            animation_set_sensor_index(stripe, animationid, 0, value);
        }

        else if (strcmp_P(cmd, PSTR("sn1")) == 0)
        {
            animation_set_sensor_index(stripe, animationid, 1, value);
        }

        else
        {
            return false;
        }
    }
    else if (argc == 6)
    {
        uint8_t hsv[3];
        hsv[0] = atoi(argv[3]);
        hsv[1] = atoi(argv[4]);
        hsv[2] = atoi(argv[5]);

        if (strcmp_P(cmd, PSTR("co0")) == 0)
        {
            animation_set_color(stripe, animationid, 0, hsv);
        }

        else if (strcmp_P(cmd, PSTR("co1")) == 0)
        {
            animation_set_color(stripe, animationid, 1, hsv);
        }

        else
        {
            return false;
        }
    }

    if (strcmp_P(cmd, PSTR("fps")) == 0)
    {
        SPRINTF("%u", animation_get_fps(stripe, animationid));
    }

    else if (strcmp_P(cmd, PSTR("opt")) == 0)
    {
        SPRINTF("%u", animation_get_option(stripe, animationid));
    }

    else if (strcmp_P(cmd, PSTR("sn0")) == 0)
    {
        SPRINTF("%u", animation_get_sensor_index(stripe, animationid, 0));
    }

    else if (strcmp_P(cmd, PSTR("sn1")) == 0)
    {
        SPRINTF("%u", animation_get_sensor_index(stripe, animationid, 1));
    }

    else if (strcmp_P(cmd, PSTR("co0")) == 0)
    {
        uint8_t hsv[3];
        animation_get_color(stripe, animationid, 0, hsv);
        SPRINTF("%u %u %u", hsv[0], hsv[1], hsv[2]);
    }

    else if (strcmp_P(cmd, PSTR("co1")) == 0)
    {
        uint8_t hsv[3];
        animation_get_color(stripe, animationid, 1, hsv);
        SPRINTF("%u %u %u", hsv[0], hsv[1], hsv[2]);
    }

    else
    {
        return false;
    }

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
    if (argc < 1)
    {
        for (uint8_t stripe = 0; stripe < MAX_LED_STRIPES; ++stripe)
            animation_dump_status(stripe, animation_get_active_animation(stripe));
        return true;
    }

    uint8_t stripe = atoi(argv[0]);
    if (stripe >= MAX_LED_STRIPES)
        return false;

    if (argc < 2)
    {
        animation_dump_status(stripe, animation_get_active_animation(stripe));
    }
    else
    {
        uint8_t animationid = atoi(argv[1]);
        if (animationid >= ANIMATION_COUNT)
            return false;

        animation_dump_status(stripe, animationid);
    }

    return true;
}

#endif
