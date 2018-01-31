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

char* es_output_buffer = 0;
uint16_t es_output_buffer_len = 0;

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

    while (shell_cmd.name != 0)
    {
        // fprintf_P(_sf, PSTR("subc %s\n"), shell_cmd.name);
        if (strcmp(cmd, shell_cmd.name) == 0)
        {
            return shell_cmd.func(argc - 1, argv + 1);
        }

        memcpy_P(&shell_cmd, &sub_cmd[cmdid++], sizeof(struct _s_shell_cmd));
    }

    return false;
}

int16_t shell_command(uint8_t *buffer, uint8_t length, char *output, uint16_t output_len)
{
    char *str = (char *)buffer;
    char *token;
    uint8_t argc = 0;
    char *argv[10];

    strcpy_P(output, PSTR("str "));

    es_output_buffer = output + 4;
    es_output_buffer_len = output_len;

    //LV_("buffer: <%s> l:%u o:%u\n", buffer, length, output_len);

    while ((token = strsep(&str, " ")))
    {
        argv[argc] = token;
        argc++;
    }

    if (!sub_shell_command(animation_shell_cmd, argc, argv))
    {
    	return ECMD_ERR_PARSE_ERROR;
    }

    //LV_("obl: %u", strlen(output));

    return ECMD_FINAL(strlen(output));
}

int16_t parse_cmd_ledstripe_animation(char *cmd, char *output, uint16_t len)
{
    // skip leading spaces
    while (*cmd == ' ')
        cmd++;

    return shell_command((uint8_t *)cmd, strlen(cmd), output, len);
}

/*
  -- Ethersex META --
  block(LEDStripe)
  ecmd_ifdef(LEDSTRIPE_SUPPORT)
    ecmd_feature(ledstripe_animation, "str", STRING, Control led stripe animation)
  ecmd_endif()
*/
