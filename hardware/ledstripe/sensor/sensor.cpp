#include "sensor.h"
#include "MCP980X.h"
#include "../ledstripe_debug.h"

sensor_type sensors[MAX_SENSORS];
MCP980X mcp9801;

#ifdef __cplusplus
extern "C" {
#endif

void sensors_init(void)
{
    memset(sensors, 0, sizeof(sensor_type) * MAX_SENSORS);
}

void sensors_update(void)
{
    for (uint8_t i = 0; i < MAX_SENSORS; i++)
    {
        mcp9801.useAddress(0x4B + i);
        uint16_t rawData = mcp9801.readRawData();

        sensors[i].value = (rawData >> 8);
        sensors[i].fraction = (rawData & 0xFF);

        LV_("sns %u v:%i f:%u", i, sensors[i].value, sensors[i].fraction);

        /*
        int8_t highByte = (temp >> 8);
            uint8_t lowByte = (temp & 0xFF);
            return ((highByte << 4) | (lowByte >> 4));
        */
    }
}

int8_t sensors_get_value(uint8_t index)
{
    if (index >= MAX_SENSORS)
        return 0;

    return sensors[index].value;
}

uint8_t sensors_get_fraction(uint8_t index)
{
    if (index >= MAX_SENSORS)
        return 0;

    return sensors[index].fraction;
}

void sensors_set_value(uint8_t index, int8_t value)
{
    if (index >= MAX_SENSORS)
        return;
    sensors[index].value = value;
}

void sensors_set_fraction(uint8_t index, uint8_t fraction)
{
    if (index >= MAX_SENSORS)
        return;
    sensors[index].fraction = fraction;
}

#ifdef __cplusplus
}
#endif
