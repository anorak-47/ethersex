#pragma once

#include "led_stripe_animation.h"
#include "colorutils.h"

namespace fastled
{

class AnimationTwinkles : public LedStripeAnimation
{
public:
    virtual ~AnimationTwinkles();
    virtual void setOption(uint8_t option) override;

protected:
    AnimationTwinkles(CRGB *leds, uint16_t _led_count, animation_configuration_t *animation_info);
    uint8_t _option = 0;

    // based on ColorTwinkles by Mark Kriegsman: https://gist.github.com/kriegsman/5408ecd397744ba0393e

#define STARTING_BRIGHTNESS 64
#define FADE_IN_SPEED 32
#define FADE_OUT_SPEED 20
#define DENSITY 255

    CRGBPalette16 gCurrentPalette;

    enum
    {
        GETTING_DARKER = 0,
        GETTING_BRIGHTER = 1
    };
    // uint8_t  directionFlags[ (_led_count + 7) / 8];
    uint8_t *directionFlags;

    CRGB makeBrighter(const CRGB &color, fract8 howMuchBrighter);
    CRGB makeDarker(const CRGB &color, fract8 howMuchDarker);

    // Compact implementation of
    // the directionFlags array, using just one BIT of RAM
    // per pixel.  This requires a bunch of bit wrangling,
    // but conserves precious RAM.  The cost is a few
    // cycles and about 100 bytes of flash program memory.

    bool getPixelDirection(uint16_t i);
    void setPixelDirection(uint16_t i, bool dir);
    void brightenOrDarkenEachPixel(fract8 fadeUpAmount, fract8 fadeDownAmount);
    void colortwinkles();

    void cloudTwinkles();
    void rainbowTwinkles();
    void snowTwinkles();
    void incandescentTwinkles();

    //  TwinkleFOX by Mark Kriegsman: https://gist.github.com/kriegsman/756ea6dcae8e30845b5a
    //
    //  TwinkleFOX: Twinkling 'holiday' lights that fade in and out.
    //  Colors are chosen from a palette; a few palettes are provided.
    //
    //  This December 2015 implementation improves on the December 2014 version
    //  in several ways:
    //  - smoother fading, compatible with any colors and any palettes
    //  - easier control of twinkle speed and twinkle density
    //  - supports an optional 'background color'
    //  - takes even less RAM: zero RAM overhead per pixel
    //  - illustrates a couple of interesting techniques (uh oh...)
    //
    //  The idea behind this (new) implementation is that there's one
    //  basic, repeating pattern that each pixel follows like a waveform:
    //  The brightness rises from 0..255 and then falls back down to 0.
    //  The brightness at any given point in time can be determined as
    //  as a function of time, for example:
    //    brightness = sine( time ); // a sine wave of brightness over time
    //
    //  So the way this implementation works is that every pixel follows
    //  the exact same wave function over time.  In this particular case,
    //  I chose a sawtooth triangle wave (triwave8) rather than a sine wave,
    //  but the idea is the same: brightness = triwave8( time ).
    //
    //  Of course, if all the pixels used the exact same wave form, and
    //  if they all used the exact same 'clock' for their 'time base', all
    //  the pixels would brighten and dim at once -- which does not look
    //  like twinkling at all.
    //
    //  So to achieve random-looking twinkling, each pixel is given a
    //  slightly different 'clock' signal.  Some of the clocks run faster,
    //  some run slower, and each 'clock' also has a random offset from zero.
    //  The net result is that the 'clocks' for all the pixels are always out
    //  of sync from each other, producing a nice random distribution
    //  of twinkles.
    //
    //  The 'clock speed adjustment' and 'time offset' for each pixel
    //  are generated randomly.  One (normal) approach to implementing that
    //  would be to randomly generate the clock parameters for each pixel
    //  at startup, and store them in some arrays.  However, that consumes
    //  a great deal of precious RAM, and it turns out to be totally
    //  unnessary!  If the random number generate is 'seeded' with the
    //  same starting value every time, it will generate the same sequence
    //  of values every time.  So the clock adjustment parameters for each
    //  pixel are 'stored' in a pseudo-random number generator!  The PRNG
    //  is reset, and then the first numbers out of it are the clock
    //  adjustment parameters for the first pixel, the second numbers out
    //  of it are the parameters for the second pixel, and so on.
    //  In this way, we can 'store' a stable sequence of thousands of
    //  random clock adjustment parameters in literally two bytes of RAM.
    //
    //  There's a little bit of fixed-point math involved in applying the
    //  clock speed adjustments, which are expressed in eighths.  Each pixel's
    //  clock speed ranges from 8/8ths of the system clock (i.e. 1x) to
    //  23/8ths of the system clock (i.e. nearly 3x).
    //
    //  On a basic Arduino Uno or Leonardo, this code can twinkle 300+ pixels
    //  smoothly at over 50 updates per seond.
    //
    //  -Mark Kriegsman, December 2015

    // Overall twinkle speed.
    // 0 (VERY slow) to 8 (VERY fast).
    // 4, 5, and 6 are recommended, default is 4.
    uint8_t twinkleSpeed = 4;

    // Overall twinkle density.
    // 0 (NONE lit) to 8 (ALL lit at once).
    // Default is 5.
    uint8_t twinkleDensity = 5;

    // Background color for 'unlit' pixels
    // Can be set to CRGB::Black if desired.
    CRGB gBackgroundColor = CRGB::Black;

    CRGBPalette16 twinkleFoxPalette;

    // This function is like 'triwave8', which produces a
    // symmetrical up-and-down triangle sawtooth waveform, except that this
    // function produces a triangle wave with a faster attack and a slower decay:
    //
    //     / \
    //    /     \
    //   /         \
    //  /             \
    //

    uint8_t attackDecayWave8(uint8_t i);

    // This function takes a pixel, and if its in the 'fading down'
    // part of the cycle, it adjusts the color a little bit like the
    // way that incandescent bulbs fade toward 'red' as they dim.
    void coolLikeIncandescent(CRGB &c, uint8_t phase);

    //  This function takes a time in pseudo-milliseconds,
    //  figures out brightness = f( time ), and also hue = f( time )
    //  The 'low digits' of the millisecond time are used as
    //  input to the brightness wave function.
    //  The 'high digits' are used to select a color, so that the color
    //  does not change over the course of the fade-in, fade-out
    //  of one cycle of the brightness wave function.
    //  The 'high digits' are also used to determine whether this pixel
    //  should light at all during this cycle, based on the twinkleDensity.
    CRGB computeOneTwinkle(uint32_t ms, uint8_t salt);

    //  This function loops over each pixel, calculates the
    //  adjusted 'clock' that this pixel should use, and calls
    //  "CalculateOneTwinkle" on each pixel.  It then displays
    //  either the twinkle color of the background color,
    //  whichever is brighter.
    void drawTwinkles();

    void redGreenWhiteTwinkles();
    void hollyTwinkles();
    void redWhiteTwinkles();
    void blueWhiteTwinkles();
    void fairyLightTwinkles();
    void snow2Twinkles();
    void iceTwinkles();
    void retroC9Twinkles();
    void partyTwinkles();
    void forestTwinkles();
    void lavaTwinkles();
    void fireTwinkles();
    void cloud2Twinkles();
    void oceanTwinkles();
};

#define TWINKLE_DEF(name, loop_function)                                                                                                             \
    class AnimationTwinkles##name : public AnimationTwinkles                                                                                         \
    {                                                                                                                                                \
    public:                                                                                                                                          \
        static LedStripeAnimation *create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);                                \
        virtual ~AnimationTwinkles##name();                                                                                                          \
        virtual bool loop() override;                                                                                                                \
                                                                                                                                                     \
    protected:                                                                                                                                       \
        AnimationTwinkles##name(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info);                                          \
    };

TWINKLE_DEF(IncandescentTwinkles, incandescentTwinkles)
TWINKLE_DEF(SnowTwinkles, snowTwinkles)
TWINKLE_DEF(RainbowTwinkles, rainbowTwinkles)
TWINKLE_DEF(CloudTwinkles, cloudTwinkles)

TWINKLE_DEF(RetroC9Twinkles, retroC9Twinkles)
TWINKLE_DEF(RedWhiteTwinkles, redWhiteTwinkles)
TWINKLE_DEF(BlueWhiteTwinkles, blueWhiteTwinkles)
TWINKLE_DEF(RedGreenWhiteTwinkles, redGreenWhiteTwinkles)
TWINKLE_DEF(FairyLightTwinkles, fairyLightTwinkles)
TWINKLE_DEF(Snow2Twinkles, snow2Twinkles)
TWINKLE_DEF(HollyTwinkles, hollyTwinkles)
TWINKLE_DEF(IceTwinkles, iceTwinkles)
TWINKLE_DEF(PartyTwinkles, partyTwinkles)
TWINKLE_DEF(ForestTwinkles, forestTwinkles)
TWINKLE_DEF(LavaTwinkles, lavaTwinkles)
TWINKLE_DEF(FireTwinkles, fireTwinkles)
TWINKLE_DEF(Cloud2Twinkles, cloud2Twinkles)
TWINKLE_DEF(OceanTwinkles, oceanTwinkles)

} /* namespace fastled */
