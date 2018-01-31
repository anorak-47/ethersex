/**
 * @file
 * @author Roman Ondráček <ondracek.roman@centrum.cz>
 * @version 1.0
 *
 * Copyright 2016 Roman Ondráček
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MCP980X.h"
#include <avr/io.h>
#include <util/twi.h>

extern "C" {
#include "../../i2c/master/i2c_master.h"
}

#include "../ledstripe_debug.h"

/**
 * MCP980X constructor
 * @param address I2C address of sensor
 */
MCP980X::MCP980X()
{
    address = 0;
}

/**
 * Initialize the I2C bus
 */
void MCP980X::useAddress(uint8_t address)
{
    this->address = address;
}

/**
 * Get status of sensor
 * @return True if sensor is succesfully initialized
 */
bool MCP980X::available()
{
    uint8_t config;
    this->read(REG_CONFIG, &config, 1);
    return (config == 0);
}

/**
 * Write bit(s) to sensor
 * @param reg Register of sensor
 * @param data Pointer to data buffer
 * @param lenght Length of data to send
 */
void MCP980X::write(uint8_t reg, uint8_t *data, uint8_t lenght)
{
    if (!i2c_master_select(this->address, TW_WRITE))
        goto end;

    TWDR = reg;

    if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK)
        goto end;

    while (lenght--)
    {
        TWDR = *data++;

        if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK)
            goto end;
    }

end:
    i2c_master_stop();

    /*
        Wire.beginTransmission(this->address);
        Wire.write(reg);
        while (lenght--)
        {
                Wire.write(*data++);
        }
        Wire.endTransmission();
        */
}

/**
 * Read bit(s) from sensor
 * @param reg Register of sensor
 * @param buffer Pointer to buffer
 * @param lenght Lenght of data to read
 */
void MCP980X::read(uint8_t reg, uint8_t *buffer, uint8_t lenght)
{
    //LV_("mcp read 0x%X", this->address);

    /*select slave in write mode */
    if (!i2c_master_select(this->address, TW_WRITE))
    {
        //LS_("mcp select fail");
        goto end;
    }

    /*send the register address */
    TWDR = reg;
    if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK)
    {
        //LS_("mcp tack fail");
        goto end;
    }

    /* Do an repeated start condition */
    if (i2c_master_start() != TW_REP_START)
    {
        //LS_("mcp start fail");
        goto end;
    }

    /*select the slave in read mode */
    TWDR = (this->address << 1) | TW_READ;
    if (i2c_master_transmit() != TW_MR_SLA_ACK)
    {
        //LS_("mcp tmit fail");
        goto end;
    }

    /*get the first byte from the slave */
    while (lenght > 1)
    {
        if (i2c_master_transmit_with_ack() != TW_MR_DATA_ACK)
        {
            //LS_("mcp ltack fail");
            goto end;
        }

        *buffer = TWDR;
        //LV_("b %u: 0x%X", lenght, *buffer);
        buffer++;
        lenght--;
    }

    /*get the last byte from the slave */
    if (i2c_master_transmit() != TW_MR_DATA_NACK)
    {
        //LS_("mcp fmit fail");
        goto end;
    }

    *buffer = TWDR;
    //LV_("b %u: 0x%X", lenght, *buffer);

end:
    i2c_master_stop();

    /*
        Wire.beginTransmission(this->address);
        Wire.write(reg);
        Wire.endTransmission();
        Wire.requestFrom(this->address, lenght);
        // Wait for data
        while (lenght--)
        {
                *buffer++ = Wire.read();
        }
        */
}

/**
 * Set shutdown the sensor
 * @param shutdown True to enable shutdown mode
 */
void MCP980X::setShutdown(bool shutdown)
{
    uint8_t config = 0;
    this->read(REG_CONFIG, &config, 1);
    config &= ~(1 << CONFIG_SHUTDOWN);
    config |= (shutdown << CONFIG_SHUTDOWN);
    this->write(REG_CONFIG, &config, 1);
}

/**
 * Set one shot mode
 * @details One shot keeps the chip powered down and only wakes up when temperature is requested.
 * @details Consumes 0.1uA between reads and wakes up for 200uA for 30ms when reading temp.
 * @param enabled True to enable one shot mode
 */
void MCP980X::setOneShot(bool enabled)
{
    // Device needs to be in shutdown mode first
    this->setShutdown(enabled);
    uint8_t config = 0;
    this->read(REG_CONFIG, &config, 1);
    config &= ~(1 << CONFIG_ONE_SHOT);
    config |= (enabled << CONFIG_ONE_SHOT);
    this->write(REG_CONFIG, &config, 1);
}

/**
 * Set ADC conversion
 * @details Sets conversion resolution from 9 to 12 bit, 0.5-0.0625c accuracy. Higher resolutions take longer to acquire
 * @param resolution ADC resolution to use
 * | Bit resolution | Temperature resolution |
 * | :------------: | :--------------------: |
 * |      9 bit     |          0.5°C         |
 * |     10 bit     |          0.25°C        |
 * |     11 bit     |         0.125°C        |
 * |     12 bit     |         0.0625°C       |
 */
void MCP980X::setResolution(uint8_t resolution)
{
    uint8_t config = 0;
    this->read(REG_CONFIG, &config, 1);
    uint8_t res = 0;
    if (resolution >= 9 && resolution <= 12)
    {
        res = resolution - 9;
    }
    config &= ~(3 << CONFIG_ADC_RES);
    config |= (res << CONFIG_ADC_RES);
    this->write(REG_CONFIG, &config, 1);
}

/**
 * Set number of temperatures outside of the temperature range before triggering an alarm
 * @details The fault queue feature can be used as a filter to lessen
 * @details The probability of spurious activation of the ALERT pin
 * @see mcp_fault_queue_t
 * @param numOfFaults Number of temperatures outside of the temperature range before triggering an alarm
 */
void MCP980X::setFaultQueue(mcp_fault_queue_t numOfFaults)
{
    uint8_t config = 0;
    this->read(REG_CONFIG, &config, 1);
    uint8_t number = 0;
    if (numOfFaults >= 0 && numOfFaults <= 6)
    {
        number = numOfFaults;
    }
    config &= ~(3 << CONFIG_FAULT_QUEUE);
    config |= (number << CONFIG_FAULT_QUEUE);
    this->write(REG_CONFIG, &config, 1);
}

/**
 * Set alert pin behavior
 * @details Set the pin to either continuously keep the alert pin active or only assert it until an MCU has reacted to it and cleared it using
 * resetAlert()
 * @see setAlertLimits()
 * @see resetAlert()
 * @param mode MCP_COMPARATOR: pin remains active until temperature drops under hysterisis. MCP_INTERRUPT: pin can be reset using resetAlert()
 * @param polarity Alarm pin polarity. true for active high, false for active low
 */
void MCP980X::setAlertMode(mcp_alert_mode_t mode, bool polarity)
{
    uint8_t config = 0;
    this->read(REG_CONFIG, &config, 1);
    config &= ~(1 << CONFIG_COMP_INT);
    config &= ~(1 << CONFIG_ALERT_POL);
    config |= (mode << CONFIG_COMP_INT) | (polarity << CONFIG_ALERT_POL);
    this->write(REG_CONFIG, &config, 1);
}

/**
 * Set temperature alarm limits
 * @details Set temperature alarm limits (in Celsius)
 * @see setAlertMode()
 * @see resetAlert()
 * @param lower Lowest allowed temperature
 * @param upper Highest allowed temperature
 */
void MCP980X::setAlertLimits(int8_t lower, int8_t upper)
{
    if (lower < -127)
    {
        lower = -127;
    }
    if (upper < -127)
    {
        upper = -127;
    }
    bool negativeLower = lower < 0;
    bool negativeUpper = upper < 0;
    uint8_t hysterisis = (negativeLower << 7) | lower;
    uint8_t limit = (negativeUpper << 7) | upper;
    this->write(REG_HYSTERESIS, &hysterisis, 1);
    this->write(REG_LIMIT, &limit, 1);
}

/**
 * Resets alarm if alert mode is set to MCP_INTERRUPT. In MCP_INTERRUPT mode a new alert is not triggered until this is called.
 */
void MCP980X::resetAlert()
{
    // Reading any register resets the alarm
    read(REG_CONFIG, NULL, 1);
}

/**
 * Read raw temperature data from sensor
 * @details Return number of bits as defined in setResolution()
 * @see setResolution()
 * @return Raw temperature data
 */
int16_t MCP980X::readRawData()
{
    uint8_t temp[2] = {0, 0};
    this->read(REG_TEMP, temp, 2);
    //LV_("mcp raw hi %u lo %u", temp[0], temp[1]);
    return ((temp[0] << 8) | temp[1]);
}

/**
 * Read temperature from sensor in Celsius * 16
 * @return Current temperature in Celsius. Divide by 16 to get actual reading.
 */
int16_t MCP980X::readTemperatureInt()
{
    int16_t temp = this->readRawData();
    int8_t highByte = (temp >> 8);
    uint8_t lowByte = (temp & 0xFF);
    return ((highByte << 4) | (lowByte >> 4));
}

/**
 * Read tempetarure from sensor in Celsius
 * @details Temperature from readTemperatureInt() devided by 16
 * @see readTemperatureInt()
 * @return Temperature in Celsius
 */
float MCP980X::readTemperature()
{
    return (this->readTemperatureInt() / 16.0f);
}

/**
 * Convert temperature form Celsius to Fahrenheit
 * @param temp Temperature in Celsius
 * @return Temperature in Fahrenheit
 */
int16_t MCP980X::toFahrenheit(int16_t temp)
{
    return ((int16_t)(temp * (9 / 5)) + 32);
}

/**
 * Convert temperature form Celsius to Fahrenheit
 * @param temp Temperature in Celsius
 * @return Temperature in Fahrenheit
 */
float MCP980X::toFahrenheit(float temp)
{
    return ((temp * (9.0 / 5.0)) + 32);
}
