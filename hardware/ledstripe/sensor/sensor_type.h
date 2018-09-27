#pragma once

#include <inttypes.h>

struct _s_sensor_type
{
    int16_t value;
    bool isvalid;
};

typedef struct _s_sensor_type sensor_type;
