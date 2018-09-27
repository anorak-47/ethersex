#pragma once

#include "sensor_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SENSORS (5)

#define ONEDEGREE 16

#define TMAX (40)
#define TREF (20)
#define TDELTA (ONEDEGREE+(ONEDEGREE/3)*2)

void sensors_init(void);
void sensors_update(void);

bool sensor_is_valid(uint8_t index);
int16_t sensors_get_value16(uint8_t index);
int8_t sensors_get_value8(uint8_t index);
uint8_t sensors_get_fraction(uint8_t index);

void sensors_set_value8(uint8_t index, int8_t value);
void sensors_set_fraction(uint8_t index, uint8_t fraction);

#ifdef __cplusplus
}
#endif
