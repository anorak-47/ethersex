

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

#ifndef MCP980X_H
#define MCP980X_H

#include <stddef.h>
#include <stdint.h>

/**
 * Temperature sensors Microchip MCP9800, MCP9801, MCP9802 and MCP9803.
 */
class MCP980X
{
public:
    /**
     * Alert mode type
     */
    typedef enum {
        /// Alert pin is kept active (see setAlertPolarity()) when temperature greater than alert value
        MCP_COMPARATOR = 0,
        /// Alert pin asserted and reset from a microcontroller using resetAlert()
        MCP_INTERRUPT = 1
    } mcp_alert_mode_t;

    /**
     * Fault queue type
     */
    typedef enum {
        /// Allow 1 fault before alert
        MCP_1_FAULT = 0,
        /// Allow 2 fault before alert
        MCP_2_FAULTS = 1,
        /// Allow 4 fault before alert
        MCP_4_FAULTS = 2,
        /// Allow 4 fault before alert
        MCP_6_FAULTS = 3
    } mcp_fault_queue_t;

    MCP980X();

    void useAddress(uint8_t address);

    bool available();
    void setShutdown(bool shutdown);
    void setOneShot(bool enabled);
    void setResolution(uint8_t resolution);
    void setFaultQueue(mcp_fault_queue_t numOfFaults);
    void setAlertMode(mcp_alert_mode_t mode, bool polarity);
    void setAlertLimits(int8_t lower, int8_t upper);
    void resetAlert();
    int16_t readRawData();
    int16_t readTemperatureInt();
    float readTemperature();
    int16_t toFahrenheit(int16_t temp);
    float toFahrenheit(float temp);

private:
    void write(uint8_t reg, uint8_t *data, uint8_t lenght);
    void read(uint8_t reg, uint8_t *buffer, uint8_t lenght);

    /**
     * I2C registers
     */
    enum registers
    {
        /// Temperature register
        REG_TEMP = 0,
        /// Config register
        REG_CONFIG = 1,
        /// Temperature Hysteresis register
        REG_HYSTERESIS = 2,
        /// Temperature Limit-set register
        REG_LIMIT = 3
    };

    /**
     * Module configuration bits
     */
    enum configuration
    {
        /// Configuration of shutdown mode
        CONFIG_SHUTDOWN = 0,
        /// Configuration of comparator/interrupt mode
        CONFIG_COMP_INT = 1,
        /// Configuration of alert polarity
        CONFIG_ALERT_POL = 2,
        /// Configuration of fault queue
        CONFIG_FAULT_QUEUE = 3,
        /// Configuration of ADC resolution
        CONFIG_ADC_RES = 5,
        /// Configuration of one shot mode
        CONFIG_ONE_SHOT = 7
    };

    /// I2C sensor address
    uint8_t address;
};

#endif
