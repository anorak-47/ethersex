/*
 * Copyright (c) 2009 by Christian Dietrich <stettberger@dokucode.de>
 * Copyright (c) 2009 by Stefan Riepenhausen <rhn@gmx.net>
 * Copyright (c) 2012 by Erik Kunze <ethersex@erik-kunze.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/twi.h>
#include <string.h>

#include "autoconf.h"
#include "config.h"
#include "core/debug.h"
#include "hardware/i2c/master/i2c_tsl2550.h"
#include "protocols/ecmd/ecmd-base.h"

int16_t parse_cmd_i2c_tsl2550_set_power_state(char *cmd, char *output,
                                              uint16_t len) {
  uint8_t state;
  sscanf_P(cmd, PSTR("%hhu"), &state);
  uint16_t ret = i2c_tsl2550_set_power_state(state);
  if (ret == 0xffff)
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
#ifdef ECMD_MIRROR_REQUEST
  return ECMD_FINAL(snprintf_P(output, len, PSTR("tsl2550 power %u"), state));
#else
  return ECMD_FINAL(snprintf_P(output, len, PSTR("ok")));
#endif
}

int16_t parse_cmd_i2c_tsl2550_set_operating_mode(char *cmd, char *output,
                                                 uint16_t len) {
  uint8_t mode;
  sscanf_P(cmd, PSTR("%hhu"), &mode);
  uint16_t temp = i2c_tsl2550_set_operating_mode(mode);
  if (temp == 0xffff)
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
#ifdef ECMD_MIRROR_REQUEST
  return ECMD_FINAL(snprintf_P(output, len, PSTR("tsl2550 range %u"), mode));
#else
  return ECMD_FINAL(snprintf_P(output, len, PSTR("ok")));
#endif
}

int16_t parse_cmd_i2c_tsl2550_set_auto_range(char *cmd, char *output,
                                                 uint16_t len) {
  uint8_t mode;
  sscanf_P(cmd, PSTR("%hhu"), &mode);
  i2c_tsl2550_set_auto_range(mode);
#ifdef ECMD_MIRROR_REQUEST
  return ECMD_FINAL(snprintf_P(output, len, PSTR("tsl2550 auto %u"), mode));
#else
  return ECMD_FINAL(snprintf_P(output, len, PSTR("ok")));
#endif
}

int16_t parse_cmd_i2c_tsl2550_show_lux_level(char *cmd, char *output,
                                             uint16_t len) {
  uint16_t ret = i2c_tsl2550_get_lux_level();
  if (ret == 0xffff)
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
  if (ret == 0xdead)
    return ECMD_FINAL(snprintf_P(output, len, PSTR("power is off")));
  if (ret == 0xfafa)
    return ECMD_FINAL(snprintf_P(output, len, PSTR("not ready")));
#ifdef ECMD_MIRROR_REQUEST
  return ECMD_FINAL(snprintf_P(output, len, PSTR("tsl2550 lux %u"), ret));
#else
  return ECMD_FINAL(snprintf_P(output, len, PSTR("%u"), ret));
#endif
}

int16_t parse_cmd_i2c_tsl2550_get_state(char *cmd, char *output,
                                        uint16_t len) {
  uint8_t power;
  uint8_t mode;
  uint8_t auto_range;
  tsl2550_get_state(&power, &mode, &auto_range);
#ifdef ECMD_MIRROR_REQUEST
  return ECMD_FINAL(snprintf_P(
      output, len, PSTR("tsl2550 state power %u range %u auto %u"), power, mode, auto_range));
#else
  return ECMD_FINAL(snprintf_P(output, len, PSTR("power %u range %u auto %u"), power, mode, auto_range));
#endif
}

/*
-- Ethersex META --
  block([[I2C]] (TWI))
  ecmd_feature(i2c_tsl2550_show_lux_level, "tsl2550 lux",, Show light level by reading adc registers and computing level)
  ecmd_feature(i2c_tsl2550_set_power_state, "tsl2550 power", VALUE, Set the TSL2550s power state (0: down, 1:up))
  ecmd_feature(i2c_tsl2550_set_operating_mode, "tsl2550 range", VALUE, Set the TSL2550s operating mode (0: standard range, 1: extended range))
  ecmd_feature(i2c_tsl2550_set_auto_range, "tsl2550 auto", VALUE, Enable automatic range changing (0: off, 1: on))
  ecmd_feature(i2c_tsl2550_get_state, "tsl2550 state",, Get TSL2550s power and operating mode)
*/
