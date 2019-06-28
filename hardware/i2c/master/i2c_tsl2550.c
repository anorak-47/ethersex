/*
 *
 * Copyrigth (c) 2019 by Moritz Wenk <max-1973@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

#include "autoconf.h"
#include "config.h"
#include "core/debug.h"
#include "i2c_master.h"
#include "i2c_tsl2550.h"

#ifdef I2C_TSL2550_SUPPORT

/*
 * Defines
 */

#define TSL2550_POWER_DOWN 0x00
#define TSL2550_POWER_UP 0x03
#define TSL2550_STANDARD_RANGE 0x18
#define TSL2550_EXTENDED_RANGE 0x1d
#define TSL2550_READ_c0 0x43
#define TSL2550_READ_c1 0x83

/*
 * Structs
 */

struct tsl2550_state_s {
  unsigned int power_state : 1;
  unsigned int operating_mode : 1;
};

/*
 * Global tsl2550_state
 */

static struct tsl2550_state_s tsl2550_state;

/*
 * LUX calculation
 *
 *
 * INTELLIGENT OPTO SENSOR DESIGNER’S NOTEBOOK
 * Simplified TSL2550 Lux Calculation for Embedded and Micro Controller
 */

// Lookup table for channel ratio (i.e. channel1 / channel0)
const unsigned char ratioLut[129] PROGMEM = {
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99,
    99,  99,  99,  99,  99,  99,  99,  99,  99,  99,  99,  99,  98,  98,  98,
    98,  98,  98,  98,  97,  97,  97,  97,  97,  96,  96,  96,  96,  95,  95,
    95,  94,  94,  93,  93,  93,  92,  92,  91,  91,  90,  89,  89,  88,  87,
    87,  86,  85,  84,  83,  82,  81,  80,  79,  78,  77,  75,  74,  73,  71,
    69,  68,  66,  64,  62,  60,  58,  56,  54,  52,  49,  47,  44,  42,  41,
    40,  40,  39,  39,  38,  38,  37,  37,  37,  36,  36,  36,  35,  35,  35,
    35,  34,  34,  34,  34,  33,  33,  33,  33,  32,  32,  32,  32,  32,  31,
    31,  31,  31,  31,  30,  30,  30,  30,  30};

// Lookup table to convert channel values to counts unsigned
const short countLut[128] PROGMEM = {
    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,
    12,   13,   14,   15,   16,   18,   20,   22,   24,   26,   28,   30,
    32,   34,   36,   38,   40,   42,   44,   46,   49,   53,   57,   61,
    65,   69,   73,   77,   81,   85,   89,   93,   97,   101,  105,  109,
    115,  123,  131,  139,  147,  155,  163,  171,  179,  187,  195,  203,
    211,  219,  227,  235,  247,  263,  279,  295,  311,  327,  343,  359,
    375,  391,  407,  423,  439,  455,  471,  487,  511,  543,  575,  607,
    639,  671,  703,  735,  767,  799,  831,  863,  895,  927,  959,  991,
    1039, 1103, 1167, 1231, 1295, 1359, 1423, 1487, 1551, 1615, 1679, 1743,
    1807, 1871, 1935, 1999, 2095, 2223, 2351, 2479, 2607, 2735, 2863, 2991,
    3119, 3247, 3375, 3503, 3631, 3759, 3887, 4015};

const short kMaxLux = 1846; // standard mode max

uint16_t tsl2550_compute_lux(const uint8_t channel0, const uint8_t channel1) {

  I2CDEBUG("tsl2550: clux: c0: %u, c1: %u\n", channel0, channel1);

  //    lookup count from channel value
  unsigned short count0 = pgm_read_word(&countLut[channel0 & 0x7f]);
  unsigned short count1 = pgm_read_word(&countLut[channel1 & 0x7f]);

  I2CDEBUG("tsl2550: clux: count0: %u, count1: %u\n", count0, count1);

  // calculate ratio
  // Note: the "128" is a scaling factor
  unsigned char ratio = 128; // default

  // avoid division by zero
  // and count1 cannot be greater than count0
  if ((count0) && (count1 <= count0))
    ratio = ((count1 * 128L) / count0);

  I2CDEBUG("tsl2550: clux: ratio: %u\n", ratio);

  // calculate lux
  // Note: the "256" is a scaling factor
  unsigned long lux =
      ((unsigned long)(count0 - count1) * pgm_read_byte(&ratioLut[ratio])) /
      256L;

  I2CDEBUG("tsl2550: clux: lux: %u\n", lux);

  // range check lux
  if (lux > kMaxLux)
    lux = kMaxLux;

  return ((unsigned short)lux);
}

uint16_t i2c_tsl2550_set_operating_mode(const uint8_t mode) {
  uint16_t ret = 0xffff;

  I2CDEBUG("tsl2550: set mode: %u\n", mode);

  if (!i2c_master_select(I2C_SLA_TSL2550, TW_WRITE))
    goto end;

  TWDR = (mode == 0 ? TSL2550_STANDARD_RANGE : TSL2550_EXTENDED_RANGE);
  if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK)
    goto end;

  tsl2550_state.operating_mode = mode;
  ret = mode;

end:
  i2c_master_stop();
  I2CDEBUG("tsl2550: mode: %u\n", ret);
  return ret;
}

uint8_t i2c_tsl2550_detect() {
  uint8_t ret = 0;
  uint8_t reg03 = 0;

  I2CDEBUG("tsl2550: detect\n");

  if (!i2c_master_select(I2C_SLA_TSL2550, TW_WRITE))
    goto end;

  TWDR = TSL2550_POWER_UP;
  if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK)
    goto end;

  i2c_master_stop();

  if (!i2c_master_select(I2C_SLA_TSL2550, TW_READ))
    goto end;

  /* read tsl2550_state */

  if (i2c_master_transmit() != TW_MR_DATA_NACK)
    goto end;

  reg03 = TWDR;

end:
  i2c_master_stop();
  I2CDEBUG("tsl2550: detect: %u\n", reg03);

  if (reg03 == TSL2550_POWER_UP) {
    I2CDEBUG("tsl2550: detect: found\n");
    ret = 1;
  }

  return ret;
}

uint16_t i2c_tsl2550_set_power_state(const uint8_t state) {
  uint16_t ret = 0xffff;

  I2CDEBUG("tsl2550: set pwr: %u\n", state);

  if (!i2c_master_select(I2C_SLA_TSL2550, TW_WRITE))
    goto end;

  TWDR = (state == 0 ? TSL2550_POWER_DOWN : TSL2550_POWER_UP);
  if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK)
    goto end;

  tsl2550_state.power_state = state;
  ret = state;

end:
  i2c_master_stop();
  I2CDEBUG("tsl2550: pwr: %u\n", ret);

  if (state > 0 && tsl2550_state.operating_mode > 0) {
    /* On power up we should reset operating mode too... */
    i2c_tsl2550_set_operating_mode(tsl2550_state.operating_mode);
  }

  return ret;
}

uint8_t c1tsl2550_read_channel(uint8_t adc) {
  uint8_t val = 0;

  I2CDEBUG("tsl2550: read_lux\n");

  if (!i2c_master_select(I2C_SLA_TSL2550, TW_WRITE))
    goto end;

  TWDR = (adc == 0 ? TSL2550_READ_c0 : TSL2550_READ_c1);
  if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK)
    goto end;

  i2c_master_stop();

  if (!i2c_master_select(I2C_SLA_TSL2550, TW_READ))
    goto end;

  /* read tsl2550_state */

  if (i2c_master_transmit() != TW_MR_DATA_NACK)
    goto end;

  val = TWDR;

end:
  i2c_master_stop();
  I2CDEBUG("tsl2550: read_lux: val: %u\n", val);
  return val;
}

uint8_t tsl2550_value_is_valid(uint8_t value) {
  return (value & 0x80) == 0x80;
}

uint16_t i2c_tsl2550_get_lux_level(void) {
  uint16_t ret = 0xffff;
  uint8_t c0;
  uint8_t c1;

  if (!tsl2550_state.power_state)
    return 0xdead;

  I2CDEBUG("tsl2550: get_lux_level");

  /* read adc channel 0 */
  c0 = c1tsl2550_read_channel(0);
  /* read adc channel 1 */
  c1 = c1tsl2550_read_channel(1);

  if (!tsl2550_value_is_valid(c0) || !tsl2550_value_is_valid(c1))
    return 0xffff;

  ret = tsl2550_compute_lux(c0, c1);

  if (tsl2550_state.operating_mode != 0)
    ret *= 5;

  I2CDEBUG("tsl2550: get_lux_level: %u\n", ret);

  return ret;
}

void tsl2550_get_state(uint8_t *power, uint8_t *mode) {
  *power = tsl2550_state.power_state;
  *mode = tsl2550_state.operating_mode;
}

void i2c_tsl2550_init(void) {
  tsl2550_state.power_state = 0;
  tsl2550_state.operating_mode = 0;
}

/*
 -- Ethersex META --
 header(hardware/i2c/master/i2c_tsl2550.h)
 init(i2c_tsl2550_init)
 */

#endif /* I2C_TSL2550_SUPPORT */
