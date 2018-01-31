#include "sensor.h"
#include "MCP980X.h"
#include "../ledstripe_debug.h"

extern "C" {
#include "../../i2c/master/i2c_master.h"
}

sensor_type sensors[MAX_SENSORS];
MCP980X mcp9801;

#define MCP9801_BASE_ADDRESS (0x48)

#ifdef __cplusplus
extern "C" {
#endif

void sensors_init(void)
{
    memset(sensors, 0, sizeof(sensor_type) * MAX_SENSORS);

    LV_("init %u sns", MAX_SENSORS);

    for (uint8_t i = 0; i < MAX_SENSORS; i++)
    {
        if (i2c_master_detect(MCP9801_BASE_ADDRESS + i, MCP9801_BASE_ADDRESS + i + 1) == 0xff)
        {
            LV_("na 0x%X", MCP9801_BASE_ADDRESS + i);
            continue;
        }

        mcp9801.useAddress(MCP9801_BASE_ADDRESS + i);

        LV_("init 0x%X", MCP9801_BASE_ADDRESS + i);

        mcp9801.setResolution(12);
        sensors[i].isvalid = true;
    }
}

void sensors_update(void)
{
    //LV_("upd %u sns", MAX_SENSORS);

    for (uint8_t i = 0; i < MAX_SENSORS; i++)
    {
        if (!sensors[i].isvalid)
        {
            //LV_("na 0x%X", MCP9801_BASE_ADDRESS + i);
            continue;
        }

        mcp9801.useAddress(MCP9801_BASE_ADDRESS + i);
        //LV_("read 0x%X", MCP9801_BASE_ADDRESS + i);

        uint16_t rawData = mcp9801.readRawData();

        sensors[i].value = (rawData >> 8);
        sensors[i].fraction = (rawData & 0xFF) >> 4;

        LV_("sns %u v:%i f:%u", i, sensors[i].value, sensors[i].fraction);

        // Nachkommastellen:
        // fraction * 625 / 1000
        // fraction * 0.0625

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
