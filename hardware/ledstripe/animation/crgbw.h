/* FastLED_RGBW
 *
 * http://www.partsnotincluded.com/neopixels/fastled-rgbw-neopixels-sk6812/
 * Hack to enable SK6812 RGBW strips to work with FastLED.
 *
 * Original code by Jim Bumgardner (http://krazydad.com).
 * Modified by David Madison (http://partsnotincluded.com).
 *
*/

#pragma once

#define FASTLED_INTERNAL
#include "FastLED.h"
#include "Arduino.h"
#include <inttypes.h>

struct CRGBW
{
    union {
        struct
        {
            union {
                uint8_t g;
                uint8_t green;
            };
            union {
                uint8_t r;
                uint8_t red;
            };
            union {
                uint8_t b;
                uint8_t blue;
            };
            union {
                uint8_t w;
                uint8_t white;
            };
        };
        uint8_t raw[4];
    };

    // default values are UNINITIALIZED
	inline CRGBW() __attribute__((always_inline))
    {
    }

    /// allow copy construction
	inline CRGBW(const CRGBW& rhs) __attribute__((always_inline))
    {
        r = rhs.r;
        g = rhs.g;
        b = rhs.b;
        w = rhs.w;
    }

    CRGBW(uint8_t rd, uint8_t grn, uint8_t blu, uint8_t wht)
    {
        r = rd;
        g = grn;
        b = blu;
        w = wht;
    }

    inline CRGBW& operator= (const CRGBW& rhs) __attribute__((always_inline))
    {
        r = rhs.r;
        g = rhs.g;
        b = rhs.b;
        w = rhs.w;
        return *this;
    }

    inline CRGBW& operator= (const CRGB& rhs) __attribute__((always_inline))
    {
    	uint8_t minc = min(rhs.r, rhs.g);
    	minc = min(rhs.b, minc);

        r = rhs.r - minc;
        g = rhs.g - minc;
        b = rhs.b - minc;
        w = minc;
        return *this;
    }
};

inline uint16_t getRGBWsize(uint16_t nleds)
{
    uint16_t nbytes = nleds * 4;
    if (nbytes % 3 > 0)
        return nbytes / 3 + 1;
    else
        return nbytes / 3;
}
