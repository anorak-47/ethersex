/*
 * ECMD-commands to handle ZACwire reads (for TSic temp-sensors)
 *
 * Copyright (c) 2009 by Gerd v. Egidy <gerd@egidy.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "config.h"
#include "core/debug.h"
#include "core/bit-macros.h"
#include "core/util/fixedpoint.h"

#include "protocols/ecmd/ecmd-base.h"
#include "animation/animation_config.h"
#include "ledstripe_debug.h"
#include "shell.h"
#include "shell_animation.h"
#include "sensor.h"

#define ECMD_LEDSTRIPE_OUTPUT_SIZE 2048

char *es_output_buffer = 0;
uint16_t es_output_buffer_len = 0;

static char output_buffer[ECMD_LEDSTRIPE_OUTPUT_SIZE];

#if FASTLED_SUPPORTED
bool cmd_fastled(uint8_t argc, char **argv)
{
    return sub_shell_command(animation_shell_cmd, argc, argv);
}
#endif

bool sub_shell_command(const struct _s_shell_cmd *sub_cmd, uint8_t argc, char **argv)
{
    if (argc == 0)
        return false;

    struct _s_shell_cmd shell_cmd;
    const char *cmd = argv[0];
    uint8_t cmdid = 0;

    memcpy_P(&shell_cmd, &sub_cmd[cmdid++], sizeof(struct _s_shell_cmd));

    while (shell_cmd.cmd != 0)
    {
        // fprintf_P(_sf, PSTR("subc %s\n"), shell_cmd.cmd);
        if (strcmp(cmd, shell_cmd.cmd) == 0)
        {
            return shell_cmd.func(argc - 1, argv + 1);
        }

        memcpy_P(&shell_cmd, &sub_cmd[cmdid++], sizeof(struct _s_shell_cmd));
    }

    return false;
}

bool shell_command(uint8_t *buffer, uint8_t length)
{
    char *str = (char *)buffer;
    char *token;
    uint8_t argc = 0;
    char *argv[10];

    strcpy_P(es_output_buffer, PSTR("str "));
    es_output_buffer += 4;
    es_output_buffer_len -= 4;

    // LV_("buffer: <%s> l:%u o:%u\n", buffer, length, output_len);

    while ((token = strsep(&str, " ")))
    {
        argv[argc] = token;
        argc++;
    }

    return sub_shell_command(animation_shell_cmd, argc, argv);
}

int16_t parse_cmd_ledstripe_sensor(char *cmd, char *output, uint16_t len)
{
    // skip leading spaces
    while (*cmd == ' ')
        cmd++;

    uint8_t index;
    sscanf_P(cmd, PSTR("%hhu"), &index);
    if (index >= MAX_SENSORS)
        return ECMD_ERR_PARSE_ERROR;

    if (!sensor_is_valid(index))
        return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));

#ifdef ECMD_MIRROR_REQUEST
    return ECMD_FINAL(snprintf_P(output, len, PSTR("sns %u %d.%04u"), index, sensors_get_value(index), (uint16_t)sensors_get_fraction(index) * 625));
#else
    return ECMD_FINAL(snprintf_P(output, len, PSTR("%d.%04u"), sensors_get_value(index), (uint16_t)sensors_get_fraction(index) * 625));
#endif
}

int16_t parse_cmd_ledstripe_animation(char *cmd, char *output, uint16_t len)
{
    char *ocmd = cmd;
    // LV_("cmd <%s> %u", cmd, (uint8_t)cmd[0]);
    // set s# anc|asc|ani|ans|fps|apl|asw|op0/1|sn0/1|c

    if (cmd[0] == ECMD_STATE_MAGIC)
    {
        // LS_("magic");

#ifdef ECMD_MAGIC_TOKENIZE
        char *token = strsep(&es_output_buffer, "\n");
        if (token != 0 && *token != '\0')
        {
            uint16_t l = strlen(token);
            // LV_("tok %u %s", l, token);
            strncpy(output, token, len - 1);
            return ECMD_AGAIN(l);
        }
#else
        uint16_t olen = strlen(es_output_buffer);
        if (olen > 0)
        {
            if (olen > len - 1)
            {
                uint16_t l = len - 1;
                strncpy(output, es_output_buffer, l);
                es_output_buffer += l;
                output[l + 1] = ECMD_NO_NEWLINE;
                return ECMD_AGAIN(l + 1);
            }
            else
            {
                strncpy(output, es_output_buffer, len);
                return ECMD_FINAL(olen);
            }
        }
#endif

        // LS_("magic ok");
        return ECMD_FINAL_OK;
    }

    es_output_buffer = output_buffer;
    es_output_buffer_len = ECMD_LEDSTRIPE_OUTPUT_SIZE;

    // skip leading spaces
    while (*cmd == ' ')
        cmd++;

    if (shell_command((uint8_t *)cmd, strlen(cmd)))
    {
        es_output_buffer = output_buffer;

        if (strlen(es_output_buffer) >= len)
        {
#ifdef ECMD_MAGIC_TOKENIZE
            // LV_("ecmd eso %u", strlen(es_output_buffer));
            char *token = strsep(&es_output_buffer, "\n");
            uint16_t l = strlen(token);
            // LV_("tok %u %s", l, token);
            strncpy(output, token, len - 1);
#else
            uint16_t l = len - 1;
            strncpy(output, es_output_buffer, l);
            output[len] = ECMD_NO_NEWLINE;
            es_output_buffer += l;
#endif

            ocmd[0] = ECMD_STATE_MAGIC;
            ocmd[1] = 0;

            return ECMD_AGAIN(len);
        }

        uint16_t l = strlen(es_output_buffer);
        strncpy(output, es_output_buffer, len - 1);
        return ECMD_FINAL(l);
    }

    return ECMD_ERR_PARSE_ERROR;
}

/*
  -- Ethersex META --
  block(LEDStripe)
  ecmd_ifdef(LEDSTRIPE_SUPPORT)
    ecmd_feature(ledstripe_animation, "str", STRING, Control led stripe animation)
    ecmd_feature(ledstripe_sensor, "sns", STRING, Read temperature sensor value)
  ecmd_endif()
*/
