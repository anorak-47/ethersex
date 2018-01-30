#pragma once

#include <inttypes.h>

struct _s_sensor_type
{
    int8_t value;
    uint8_t fraction;
};

typedef struct _s_sensor_type sensor_type;
