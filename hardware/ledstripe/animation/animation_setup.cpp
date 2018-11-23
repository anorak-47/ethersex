#include "animation.h"
#include "animation_setup.h"
#include "animation_config.h"
#include "animation_type.h"
#include "crgbw.h"

#include "../ledstripe_debug.h"

#define FASTLED_CRGBW // SK6812 hack
#define FASTLED_MIRROR_LED_STRIPE_0

/* Pinout
 *
 * arduino variant: 40pin
 * This matches the definition in FastLED/platforms/avr/fastpin_avr.h
 * https://github.com/MCUdude/MightyCore

                 MIGHTYCORE STANDARD PINOUT
         ATmega8535, ATmega16, ATmega32, ATmega164,
              ATmega324, ATmega644, ATmega1284

                         +---\/---+
         LED (D 0) PB0  1|        |40  PA0 (A 0 / D24)
             (D 1) PB1  2|        |39  PA1 (A 1 / D25)
        INT2 (D 2) PB2  3|        |38  PA2 (A 2 / D26)
         PWM (D 3) PB3  4|        |37  PA3 (A 3 / D27)
    PWM* /SS (D 4) PB4  5|        |36  PA4 (A 4 / D28)
        MOSI (D 5) PB5  6|        |35  PA5 (A 5 / D29)
 PWM** /MISO (D 6) PB6  7|        |34  PA6 (A 6 / D30)
  PWM** /SCK (D 7) PB7  8|        |33  PA7 (A 7 / D31)
                   RST  9|        |32  AREF
                   VCC 10|        |31  GND
                   GND 11|        |30  AVCC
                 XTAL2 12|        |29  PC7 (D 23)
                 XTAL1 13|        |28  PC6 (D 22)
        RX0 (D 8)  PD0 14|        |27  PC5 (D 21) TDI
        TX0 (D 9)  PD1 15|        |26  PC4 (D 20) TDO
 (RX1)/INT0 (D 10) PD2 16|        |25  PC3 (D 19) TMS
 (TX1)/INT1 (D 11) PD3 17|        |24  PC2 (D 18) TCK
        PWM (D 12) PD4 18|        |23  PC1 (D 17) SDA
        PWM (D 13) PD5 19|        |22  PC0 (D 16) SCL
       PWM* (D 14) PD6 20|        |21  PD7 (D 15) PWM
                         +--------+
       PWM: ATmega8535/16/32/164/324/644/1284
       PWM*: ATmega164/324/644/1284
       PWM**: ATmega1284
       PCINT ONLY ON ATmega164/324/644/1284

PCINT15-8: D7-0    : bit 1
PCINT31-24: D15-8  : bit 3
PCINT23-16: D23-16 : bit 2
PCINT7-0: D31-24   : bit 0
*/

// Data pin that led data will be written out over
#define DATA_PIN_0 24 // A0
#define DATA_PIN_1 25 // A1

#ifdef FASTLED_CRGBW

// led stripe 0 has 261 leds
// so nearest macht for the SK6812 hack is 264 leds

#define NUM_LEDS_STRIPE_0_REAL (264)

#ifdef FASTLED_MIRROR_LED_STRIPE_0
#define NUM_LEDS_STRIPE_0 (NUM_LEDS_STRIPE_0_REAL / 2)
#else
#define NUM_LEDS_STRIPE_0 (NUM_LEDS_STRIPE_0_REAL)
#endif

CRGB leds_stripe_0[NUM_LEDS_STRIPE_0_REAL];
CRGBW leds_stripe_0_rgbw[NUM_LEDS_STRIPE_0_REAL];
CRGB *leds_stripe_0_rgb = (CRGB *)&leds_stripe_0_rgbw[0];

#define NUM_LEDS_STRIPE_1 (19)
#define NUM_LEDS_STRIPE_1_REAL (24)

CRGB leds_stripe_1[NUM_LEDS_STRIPE_1_REAL];
CRGBW leds_stripe_1_rgbw[NUM_LEDS_STRIPE_1_REAL];
CRGB *leds_stripe_1_rgb = (CRGB *)&leds_stripe_1_rgbw[0];

void led_strands_all_back()
{
    fill_solid(leds_stripe_0, NUM_LEDS_STRIPE_0_REAL, CRGB::Black);
    fill_solid(leds_stripe_1, NUM_LEDS_STRIPE_1_REAL, CRGB::Black);

    led_strands_prepare_before_show();
    FastLED.show();
}

void led_strands_test_1()
{
    fill_solid(leds_stripe_0, NUM_LEDS_STRIPE_0_REAL, CRGB::AliceBlue);
    fill_solid(leds_stripe_1, NUM_LEDS_STRIPE_1_REAL, CRGB::DarkMagenta);

    led_strands_prepare_before_show();
    FastLED.show();
}

void led_strands_test_2()
{
    for (uint8_t i = 0; i < (NUM_LEDS_STRIPE_0 / 3); ++i)
    {
        leds_stripe_0[i * 3] = CRGB::Red;
        leds_stripe_0[i * 3 + 1] = CRGB::Green;
        leds_stripe_0[i * 3 + 2] = CRGB::Blue;
    }

    for (uint8_t i = 0; i < (NUM_LEDS_STRIPE_1 / 3); ++i)
    {
        leds_stripe_1[i * 3] = CRGB::Red;
        leds_stripe_1[i * 3 + 1] = CRGB::Green;
        leds_stripe_1[i * 3 + 2] = CRGB::Blue;
    }

    led_strands_prepare_before_show();
    FastLED.show();
}

void led_strands_test_rgb_mover()
{
    static uint8_t pos = 0;

    pos++;

    for (uint8_t i = 0; i < (NUM_LEDS_STRIPE_0 / 3); ++i)
    {
        leds_stripe_0[pos + i * 3] = CRGB::Red;
        leds_stripe_0[pos + i * 3 + 1] = CRGB::Green;
        leds_stripe_0[pos + i * 3 + 2] = CRGB::Blue;
    }

    for (uint8_t i = 0; i < (NUM_LEDS_STRIPE_1 / 3); ++i)
    {
        leds_stripe_1[pos + i * 3] = CRGB::Red;
        leds_stripe_1[pos + i * 3 + 1] = CRGB::Green;
        leds_stripe_1[pos + i * 3 + 2] = CRGB::Blue;
    }
}

void led_strands_prepare_before_show()
{
#ifdef FASTLED_MIRROR_LED_STRIPE_0
    for (uint8_t i = 0; i < NUM_LEDS_STRIPE_0; ++i)
    {
        leds_stripe_0_rgbw[i] = leds_stripe_0[i];
        leds_stripe_0_rgbw[NUM_LEDS_STRIPE_0_REAL - i - 1] = leds_stripe_0_rgbw[i];
    }
#else
    for (uint8_t i = 0; i < NUM_LEDS_STRIPE_0_REAL; ++i)
    {
        leds_stripe_0_rgbw[i] = leds_stripe_0[i];
    }
#endif

    for (uint8_t i = 0; i < NUM_LEDS_STRIPE_1; ++i)
    {
        leds_stripe_1_rgbw[i] = leds_stripe_1[i];
    }
}

void led_strands_setup()
{
    LS_("led_strands_setup");

    FastLED.addLeds<SK6812, DATA_PIN_0, RGB>(leds_stripe_0_rgb, getRGBWsize(NUM_LEDS_STRIPE_0_REAL)).setCorrection(TypicalSMD5050);
    FastLED.addLeds<SK6812, DATA_PIN_1, RGB>(leds_stripe_1_rgb, getRGBWsize(NUM_LEDS_STRIPE_1_REAL)).setCorrection(TypicalSMD5050);

    FastLED.setDither(DISABLE_DITHER);
    // FastLED.setDither(BINARY_DITHER);

    FastLED.setBrightness(FASTLED_DEFAULT_BRIGHTNESS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 6000);

    animation_state[0].leds = leds_stripe_0;
    animation_state[0].led_count = NUM_LEDS_STRIPE_0;

    animation_state[1].leds = leds_stripe_1;
    animation_state[1].led_count = NUM_LEDS_STRIPE_1;

    led_strands_all_back();
}

#else

#ifdef FASTLED_MIRROR_LED_STRIPE_0

#define NUM_LEDS_STRIPE_0_FULL (200)
#define NUM_LEDS_STRIPE_0 (NUM_LEDS_STRIPE_0_FULL / 2)
CRGB leds_stripe_0[NUM_LEDS_STRIPE_0_FULL];

#else

#define NUM_LEDS_STRIPE_0_FULL (300)
#define NUM_LEDS_STRIPE_0 (NUM_LEDS_STRIPE_0_FULL)
CRGB leds_stripe_0[NUM_LEDS_STRIPE_0];

#endif

#define NUM_LEDS_STRIPE_1 (19)
CRGB leds_stripe_1[NUM_LEDS_STRIPE_1];

void led_strands_all_back()
{
    fill_solid(leds_stripe_0, NUM_LEDS_STRIPE_0_FULL, CRGB::Black);
    fill_solid(leds_stripe_1, NUM_LEDS_STRIPE_1, CRGB::Black);

    FastLED.show();
}

void led_strands_prepare_before_show()
{
#ifdef FASTLED_MIRROR_LED_STRIPE_0
    for (uint8_t i = 0; i < NUM_LEDS_STRIPE_0; ++i)
    {
        leds_stripe_0[NUM_LEDS_STRIPE_0_FULL - i - 1] = leds_stripe_0[i];
    }
#endif
}

void led_strands_setup()
{
    FastLED.addLeds<WS2812B, DATA_PIN_0, GRB>(leds_stripe_0, NUM_LEDS_STRIPE_0_FULL).setCorrection(TypicalSMD5050);
    FastLED.addLeds<WS2812B, DATA_PIN_1, GRB>(leds_stripe_1, NUM_LEDS_STRIPE_1).setCorrection(TypicalSMD5050);

    FastLED.setBrightness(FASTLED_DEFAULT_BRIGHTNESS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);

    animation_state[0].leds = leds_stripe_0;
    animation_state[0].led_count = NUM_LEDS_STRIPE_0;

    animation_state[1].leds = leds_stripe_1;
    animation_state[1].led_count = NUM_LEDS_STRIPE_1;

    led_strands_all_back();
}

#endif
