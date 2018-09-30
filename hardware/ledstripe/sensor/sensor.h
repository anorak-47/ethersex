#pragma once

#include "sensor_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_SENSORS (5)

#define ONEDEGREE 16

#define TMAX (40)
#define TREF (20)
#define TDELTA (ONEDEGREE + (ONEDEGREE / 3) * 2)

    // Sensor 0: temperature bathtub
    // Sensor 1: temperature at floor under bathtub (reference)
    // Sensor 2: temperature at shower drain
    // Sensor 3: temperature of power supply
    // Sensor 4: adc 4

#define SENSOR_BATHTUB 0
#define SENSOR_SHOWER 2
#define SENSOR_REFERENCE 1
#define SENSOR_SUPPLY 3

    void sensors_init(void);
    void sensors_update(void);

    bool sensor_is_valid(uint8_t index);

    int16_t sensors_get_value16(uint8_t index);

    int8_t sensors_get_value(uint8_t index);
    uint8_t sensors_get_fraction(uint8_t index);

    void sensors_set_value(uint8_t index, int8_t value);
    void sensors_set_fraction(uint8_t index, uint8_t fraction);

#ifdef __cplusplus
}
#endif
