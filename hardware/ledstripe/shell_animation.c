
#include "shell_animation.h"
#include "animation/animation.h"
#include "animation/animation_names.h"
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
// extern uint16_t es_output_buffer_len;

bool cmd_status(uint8_t argc, char **argv);
bool cmd_run(uint8_t argc, char **argv);
bool cmd_animation(uint8_t argc, char **argv);
bool cmd_configure_animation(uint8_t argc, char **argv);
bool cmd_global_configuration(uint8_t argc, char **argv);
bool cmd_sensor(uint8_t argc, char **argv);
bool cmd_manage_settings(uint8_t argc, char **argv);
bool cmd_help(uint8_t argc, char **argv);
bool cmd_animation_desc(uint8_t argc, char **argv);

void option_help_animation();
void option_help_configure_animation();

#ifdef DEBUG_FUNCTIONS_SUPPORTED
#endif

const struct _s_shell_cmd animation_shell_cmd[] PROGMEM = {
    SHELLCMD("help", cmd_help, "", "this help", 0),
    SHELLCMD("sta", cmd_status, "s#", "get status for strand s#", 0),
    SHELLCMD("run", cmd_run, "s# 0|1|cyc v", "start/stop/cycle running animation", 0),
    SHELLCMD("set", cmd_animation, "s# anc|asc|ani|ans|fps|apl|asw|op0/1|sn0/1|co0/1 v", "get/set configuration for strand s#",
             option_help_animation),
    SHELLCMD("cnf", cmd_configure_animation, "s# a# [fps|op0/1|sn0/1|co0/1 [v]]", "get/set configuration for animation a#",
             option_help_configure_animation),
    SHELLCMD("glo", cmd_global_configuration, "fps|bri [v]", "get/set global configuration for all strands", 0),
    SHELLCMD("eem", cmd_manage_settings, "load|clear|save", "load/clear/save settings for all strand", 0),
    SHELLCMD("lst", cmd_animation_desc, "[#a]", "list all or show description for animation #a", 0),
    SHELLCMD("sns", cmd_sensor, "upd|s# [v] [f]", "get/set sensor value [fraction]; upd forces update", 0),
#ifdef DEBUG_FUNCTIONS_SUPPORTED
#endif
    SHELLCMD(0, 0, 0, 0, 0)};

#define CMD_CATOUT(fmt)                                                                                                                              \
    {                                                                                                                                                \
        int8_t cnt = sprintf_P(es_output_buffer, PSTR("\t" fmt));                                                                                    \
        es_output_buffer += cnt;                                                                                                                     \
    }

void option_help_animation()
{
    CATOUT("set <strand#> <cmd> [value]\n")
    CMD_CATOUT("cyc: cycle to next/previous animation\n");
    CMD_CATOUT("anc: active animation\n");
    CMD_CATOUT("asc: active sensor animation\n");
    CMD_CATOUT("ani: current animation\n");
    CMD_CATOUT("ans: current sensor animation\n");
    CMD_CATOUT("apl: autoplay\n");
    CMD_CATOUT("asw: autoswitch\n");
    CMD_CATOUT("op0/1: option 0/1\n");
    CMD_CATOUT("sn0/sn1: sensor index\n");
    CMD_CATOUT("co0/co1: current color 0/1 [RR GG BB]\n");
}

void option_help_configure_animation()
{
    CATOUT("cnf <strand#> <ani#> <cmd> [value]")
    CMD_CATOUT("fps: frames per second\n");
    CMD_CATOUT("op0/1: option 0/1\n");
    CMD_CATOUT("sn0/1: sensor 0/1\n");
    CMD_CATOUT("co0/1: color 0/1 [RR GG BB]\n");
}

bool cmd_help(uint8_t argc, char **argv)
{
    struct _s_shell_cmd shell_cmd;
    uint8_t cmdid = 0;

    memcpy_P(&shell_cmd, &animation_shell_cmd[cmdid++], sizeof(struct _s_shell_cmd));

    while (shell_cmd.cmd != 0)
    {
#if defined(SHELL_ARGS_HELP_SUPPORTED) || defined(SHELL_HELP_SUPPORTED)
        CATSPRINTF("%s %s - %s\n", shell_cmd.cmd, shell_cmd.args_help, shell_cmd.cmd_help);
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
#ifdef SHELL_ARGS_HELP_SUPPORTED
        if (shell_cmd.func_help != 0)
        {
            CATOUT("---\n");
            shell_cmd.func_help();
            CATOUT("---\n");
        }
#endif

        memcpy_P(&shell_cmd, &animation_shell_cmd[cmdid++], sizeof(struct _s_shell_cmd));
    }

    return true;
}

void dump_current_status(uint8_t stripe)
{
#if 0
    CATSPRINTF("sta %u\n", stripe);
    CATSPRINTF(".run %u\n", animation_get_running(stripe));
    CATSPRINTF(".anr %u\n", animation_get_active_animation(stripe));
    CATSPRINTF(".ani %u\n", animation_get_current_animation(stripe));
    CATSPRINTF(".asn %u\n", animation_get_sensor_animation(stripe));
    CATSPRINTF(".asw %u\n", animation_is_autoswitch_to_sensor_animation(stripe));
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
#else
    CATSPRINTF("strand: %u\n", stripe);
    CATSPRINTF("running: %u \n", animation_get_running(stripe));
    CATSPRINTF("active animation: %u \n", animation_get_active_animation(stripe));
    CATSPRINTF("current animation: %u\n", animation_get_current_animation(stripe));
    CATSPRINTF("sensor animation: %u\n", animation_get_sensor_animation(stripe));
    CATSPRINTF("autoswitch %u\n", animation_is_autoswitch_to_sensor_animation(stripe));
    CATSPRINTF("autoplay %u\n", animation_get_autoplay(stripe));
    CATSPRINTF("apl time %u\n", animation_get_autoplay_delay_time(stripe));
    CATSPRINTF("fps: %u\n", animation_get_current_fps(stripe));
    CATSPRINTF("option 0: %u\n", animation_get_current_option_0(stripe));
    CATSPRINTF("option 1: %u\n", animation_get_current_option_1(stripe));
    CATSPRINTF("sensor 0: %u\n", animation_get_current_sensor_index(stripe, 0));
    CATSPRINTF("sensor 1: %u\n", animation_get_current_sensor_index(stripe, 1));
    uint8_t hsv[3];
    animation_get_current_color(stripe, 0, hsv);
    CATSPRINTF("color 0: %x %x %x\n", hsv[0], hsv[1], hsv[2]);
    animation_get_current_color(stripe, 1, hsv);
    CATSPRINTF("color 1: %x %x %x\n", hsv[0], hsv[1], hsv[2]);
#endif
}

void dump_configuration_for_animation(uint8_t stripe, uint8_t animation)
{
#if 0
    CATSPRINTF("sta %u\n", stripe);
    CATSPRINTF(".run %u\n", animation_get_running(stripe));
    CATSPRINTF(".anr %u\n", animation_get_active_animation(stripe));
    CATSPRINTF(".ani %u\n", animation_get_current_animation(stripe));
    CATSPRINTF(".asn %u\n", animation_get_sensor_animation(stripe));
    CATSPRINTF(".asw %u\n", animation_is_autoswitch_to_sensor_animation(stripe));
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
#else
    CATSPRINTF("strand: %u\n", stripe);
    CATSPRINTF("animation: %u\n", animation);
    CATSPRINTF("fps: %u\n", animation_get_fps(stripe, animation));
    CATSPRINTF("option 0: %u\n", animation_get_option(stripe, animation, 0));
    CATSPRINTF("option 1: %u\n", animation_get_option(stripe, animation, 1));
    CATSPRINTF("sensor 0: %u\n", animation_get_sensor_index(stripe, animation, 0));
    CATSPRINTF("sensor 1: %u\n", animation_get_sensor_index(stripe, animation, 1));
    uint8_t hsv[3];
    animation_get_color(stripe, animation, 0, hsv);
    CATSPRINTF("color 0: %x %x %x\n", hsv[0], hsv[1], hsv[2]);
    animation_get_color(stripe, animation, 1, hsv);
    CATSPRINTF("color 1: %x %x %x\n", hsv[0], hsv[1], hsv[2]);
#endif
}

bool cmd_manage_settings(uint8_t argc, char **argv)
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

        SPRINTF("eem %s", argv[0]);
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

bool cmd_global_configuration(uint8_t argc, char **argv)
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

bool cmd_sensor(uint8_t argc, char **argv)
{
    if (argc < 1)
    {
        CATOUT("sns\n");
        for (uint8_t index = 0; index < MAX_SENSORS; ++index)
        {
            CATSPRINTF(".sns %u %d.%04u\n", index, sensors_get_value(index), (uint16_t)sensors_get_fraction(index) * 625);
        }
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

    SPRINTF("%u %d.%04u", index, sensors_get_value(index), (uint16_t)sensors_get_fraction(index) * 625);

    return true;
}

bool cmd_animation(uint8_t argc, char **argv)
{
    // args[]:     0      1      2
    // str set <stripe> <cmd> [value]

    if (argc < 1)
        return false;

    uint8_t stripe = atoi(argv[0]);
    if (stripe >= MAX_LED_STRIPES)
        return false;

    char *cmd = argv[1];

    CATSPRINTF("set %u %s ", stripe, cmd);

    if (argc == 2)
    {
        if (strcmp_P(cmd, PSTR("anc")) == 0)
        {
            // make the current animation the active animation
            animation_set_active_animation(stripe, animation_get_current_animation(stripe));
        }

        else if (strcmp_P(cmd, PSTR("asc")) == 0)
        {
            // make the sensor animation the active animation
            animation_set_active_animation(stripe, animation_get_sensor_animation(stripe));
        }
    }
    else if (argc == 3)
    {
        // str set <stripe> <cmd> [#]
        //         0  1  2
        // str set 0 fps 5
        // str set 1 cyc 1

        uint8_t value = atoi(argv[2]);

        if (strcmp_P(cmd, PSTR("cyc")) == 0)
        {
            if (value == 0)
                animation_set_previous_animation_active(stripe);
            else
                animation_set_next_animation_active(stripe);
        }

        else if (strcmp_P(cmd, PSTR("ani")) == 0)
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

        else if (strcmp_P(cmd, PSTR("apt")) == 0)
        {
            uint16_t secs = atoi(argv[2]);
            animation_set_autoplay_delay_time(stripe, secs);
        }

        else if (strcmp_P(cmd, PSTR("asw")) == 0)
        {
            animation_set_autoswitch_sensor_animation(stripe, value);
        }

        else if (strcmp_P(cmd, PSTR("op0")) == 0)
        {
            animation_set_current_option_0(stripe, value);
        }

        else if (strcmp_P(cmd, PSTR("op1")) == 0)
        {
            animation_set_current_option_1(stripe, value);
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

    if (strcmp_P(cmd, PSTR("cyc")) == 0)
    {
        SPRINTF("%u", animation_get_active_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("ani")) == 0)
    {
        SPRINTF("%u", animation_get_current_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("anc")) == 0)
    {
        SPRINTF("%u", animation_get_active_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("asn")) == 0)
    {
        SPRINTF("%u", animation_get_sensor_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("asc")) == 0)
    {
        SPRINTF("%u", animation_get_sensor_animation(stripe));
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
        SPRINTF("%u", animation_is_autoswitch_to_sensor_animation(stripe));
    }

    else if (strcmp_P(cmd, PSTR("op0")) == 0)
    {
        SPRINTF("%u", animation_get_current_option_0(stripe));
    }

    else if (strcmp_P(cmd, PSTR("op1")) == 0)
    {
        SPRINTF("%u", animation_get_current_option_1(stripe));
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

    else if (strcmp_P(cmd, PSTR("rst")) == 0)
    {
        CATOUT("ok");
    }

    else
    {
        return false;
    }

    return true;
}

bool cmd_configure_animation(uint8_t argc, char **argv)
{
    // argv[]:      0      1     2      3
    // str cnf <stripe#> <ani#> <cmd> [value]
    // str cnf <stripe#> <ani#> co0  h s v

    if (argc < 2)
        return false;

    uint8_t stripe = atoi(argv[0]);
    if (stripe >= MAX_LED_STRIPES)
        return false;

    uint8_t animationid = atoi(argv[1]);
    if (animationid >= ANIMATION_COUNT)
        return false;

    if (argc == 2)
    {
        CATSPRINTF("cnf %u %u\n", stripe, animationid);
        dump_configuration_for_animation(stripe, animationid);
        return true;
    }

    char *cmd = argv[2];

    CATSPRINTF("cnf %u %u %s ", stripe, animationid, cmd);

    if (argc == 4)
    {
        uint8_t value = atoi(argv[3]);

        if (strcmp_P(cmd, PSTR("fps")) == 0)
        {
            animation_set_fps(stripe, animationid, value);
        }

        else if (strcmp_P(cmd, PSTR("op0")) == 0)
        {
            animation_set_option(stripe, animationid, 0, value);
        }

        else if (strcmp_P(cmd, PSTR("op1")) == 0)
        {
            animation_set_option(stripe, animationid, 1, value);
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

    else if (strcmp_P(cmd, PSTR("op0")) == 0)
    {
        SPRINTF("%u", animation_get_option(stripe, animationid, 0));
    }

    else if (strcmp_P(cmd, PSTR("op1")) == 0)
    {
        SPRINTF("%u", animation_get_option(stripe, animationid, 1));
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

bool cmd_run(uint8_t argc, char **argv)
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

bool cmd_animation_desc(uint8_t argc, char **argv)
{
    if (argc < 1)
    {
        CATOUT("lst\n");
        for (uint8_t animationid = 0; animationid < ANIMATION_COUNT; ++animationid)
        {
            CATSPRINTF("%d: %s - %s\n", animationid, get_animation_name(animationid), get_animation_description(animationid));
            if (animation_has_options_description(animationid))
            {
                CATSPRINTF("\t%s\n", get_animation_options_description(animationid));
            }
        }
    }
    else
    {
        uint8_t animationid = atoi(argv[0]);
        if (animationid >= ANIMATION_COUNT)
            return false;

        CATSPRINTF("lst %u\n", animationid);
        CATSPRINTF("%u: %s - %s\n", animationid, get_animation_name(animationid), get_animation_description(animationid));
        if (animation_has_options_description(animationid))
        {
            CATSPRINTF("\t%s\n", get_animation_options_description(animationid));
        }
    }

    return true;
}

bool cmd_status(uint8_t argc, char **argv)
{
    if (argc < 1)
        return false;

    uint8_t stripe = atoi(argv[0]);
    if (stripe >= MAX_LED_STRIPES)
        return false;

    CATSPRINTF("sta %u\n", stripe);

    dump_current_status(stripe);
    // dump_configuration_for_animation(stripe, animation_get_active_animation(stripe));

    return true;
}

#endif
