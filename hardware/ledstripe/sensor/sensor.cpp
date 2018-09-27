#include "sensor.h"
#include "MCP980X.h"
#include "../ledstripe_debug.h"

extern "C" {
#include "../../i2c/master/i2c_master.h"
#include "../../adc/adc.h"
}

#define MCP9801_BASE_ADDRESS (0x48)
#define MAX_SENSORS_MCP (4)

#define ADC_OFFSET 4
#define ACD_AVG_LOOP 2

sensor_type sensors[MAX_SENSORS];
MCP980X mcp9801;

#ifdef __cplusplus
extern "C" {
#endif

void sensors_init(void)
{
    memset(sensors, 0, sizeof(sensor_type) * MAX_SENSORS);

    //LV_("init %u sns", MAX_SENSORS);

    for (uint8_t i = 0; i < MAX_SENSORS_MCP; i++)
    {
        if (i2c_master_detect(MCP9801_BASE_ADDRESS + i, MCP9801_BASE_ADDRESS + i + 1) == 0xff)
        {
            LV_("sns na 0x%X", MCP9801_BASE_ADDRESS + i);
            continue;
        }

        mcp9801.useAddress(MCP9801_BASE_ADDRESS + i);

        //LV_("init 0x%X", MCP9801_BASE_ADDRESS + i);

        mcp9801.setResolution(12);
        sensors[i].isvalid = true;
    }

#if (MAX_SENSORS - MAX_SENSORS_MCP > 0)
    for (uint8_t i = MAX_SENSORS_MCP; i < MAX_SENSORS; i++)
    {
        // used for ADC0-7
        sensors[i].isvalid = true;
    }
#endif
}

void sensors_update(void)
{
    for (uint8_t i = 0; i < MAX_SENSORS_MCP; i++)
    {
        if (!sensors[i].isvalid)
        {
            // LV_("na 0x%X", MCP9801_BASE_ADDRESS + i);
            continue;
        }

        mcp9801.useAddress(MCP9801_BASE_ADDRESS + i);
        // LV_("read 0x%X", MCP9801_BASE_ADDRESS + i);

        uint16_t rawData = mcp9801.readRawData();

        sensors[i].value = ((rawData & 0xFF00) >> 4) | ((rawData & 0xFF) >> 4);

        //LV_("sns %u v:%i f:%u", i, sensors[i].value, sensors[i].fraction);

        // Nachkommastellen:
        // fraction * 625 / 1000
        // fraction * 0.0625
    }

#if (MAX_SENSORS - MAX_SENSORS_MCP > 0)
    for (uint8_t i = MAX_SENSORS_MCP; i < MAX_SENSORS; i++)
    {
        uint16_t adc_value = 0;
        for (uint8_t a = 0; a < ACD_AVG_LOOP; a++)
        	adc_value += adc_get(i - MAX_SENSORS_MCP + ADC_OFFSET);
        adc_value /= ACD_AVG_LOOP;
        sensors[i].value = map(adc_value, 0, 1023, 10*ONEDEGREE, 45*ONEDEGREE);
        //LV_("adc %u: %u %i", i - MAX_SENSORS_MCP, adc_value, sensors[i].value);
    }
#endif
}

bool sensor_is_valid(uint8_t index)
{
    if (index >= MAX_SENSORS)
        return false;

    return sensors[index].isvalid;
}

int16_t sensors_get_value16(uint8_t index)
{
    if (index >= MAX_SENSORS)
        return 0;

    return sensors[index].value;
}

int8_t sensors_get_value8(uint8_t index)
{
    if (index >= MAX_SENSORS)
        return 0;

    return (sensors[index].value >> 4);
}

uint8_t sensors_get_fraction(uint8_t index)
{
    if (index >= MAX_SENSORS)
        return 0;

    return (sensors[index].value & 0x0F);
}

void sensors_set_value8(uint8_t index, int8_t value)
{
    if (index >= MAX_SENSORS)
        return;

    sensors[index].value = (int16_t)value << 4 | (sensors[index].value | 0x0F);
}

void sensors_set_fraction(uint8_t index, uint8_t fraction)
{
    if (index >= MAX_SENSORS)
        return;

    sensors[index].value = (sensors[index].value | 0xFFF0) | (fraction | 0x0F);
}

#ifdef __cplusplus
}
#endif
