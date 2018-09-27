
#include "animation_twinkles.h"

namespace fastled
{

// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

// If AUTO_SELECT_BACKGROUND_COLOR is set to 1,
// then for any palette where the first two entries
// are the same, a dimmed version of that color will
// automatically be used as the background color.
#define AUTO_SELECT_BACKGROUND_COLOR 0

// If COOL_LIKE_INCANDESCENT is set to 1, colors will
// fade out slighted 'reddened', similar to how
// incandescent bulbs change color as they get dim down.
#define COOL_LIKE_INCANDESCENT 1

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM = {CRGB::Red,   CRGB::Red,   CRGB::Red,   CRGB::Red,  CRGB::Red,  CRGB::Red,
                                                         CRGB::Red,   CRGB::Red,   CRGB::Red,   CRGB::Red,  CRGB::Gray, CRGB::Gray,
                                                         CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green};

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red 0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM = {Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green,
                                                 Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green,
                                                 Holly_Green, Holly_Green, Holly_Green, Holly_Red};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM = {CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
                                                    CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray};

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM = {CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
                                                     CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray};

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM = {
    CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight,
    QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight};

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM = {0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048,
                                                0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0xE0F0FF};

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red 0xB80400
#define C9_Orange 0x902C02
#define C9_Green 0x046002
#define C9_Blue 0x070758
#define C9_White 0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM = {C9_Red,   C9_Orange, C9_Red,   C9_Orange, C9_Orange, C9_Red,  C9_Orange, C9_Red,
                                                   C9_Green, C9_Green,  C9_Green, C9_Green,  C9_Blue,   C9_Blue, C9_Blue,   C9_White};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM = {Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
                                               Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3};

AnimationTwinkles::AnimationTwinkles(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
    : LedStripeAnimation(leds, led_count, animation_info)
{
    directionFlags = (uint8_t *)malloc((led_count + 7) / 8);
    gCurrentPalette = CRGBPalette16(CRGB::Black);
}

AnimationTwinkles::~AnimationTwinkles()
{
    free(directionFlags);
}

void AnimationTwinkles::setOption(uint8_t option)
{
    _option = option;
}

CRGB AnimationTwinkles::makeBrighter(const CRGB &color, fract8 howMuchBrighter)
{
    CRGB incrementalColor = color;
    incrementalColor.nscale8(howMuchBrighter);
    return color + incrementalColor;
}

CRGB AnimationTwinkles::makeDarker(const CRGB &color, fract8 howMuchDarker)
{
    CRGB newcolor = color;
    newcolor.nscale8(255 - howMuchDarker);
    return newcolor;
}

// Compact implementation of
// the directionFlags array, using just one BIT of RAM
// per pixel.  This requires a bunch of bit wrangling,
// but conserves precious RAM.  The cost is a few
// cycles and about 100 bytes of flash program memory.

bool AnimationTwinkles::getPixelDirection(uint16_t i)
{
    uint16_t index = i / 8;
    uint8_t bitNum = i & 0x07;

    uint8_t andMask = 1 << bitNum;
    return (directionFlags[index] & andMask) != 0;
}

void AnimationTwinkles::setPixelDirection(uint16_t i, bool dir)
{
    uint16_t index = i / 8;
    uint8_t bitNum = i & 0x07;

    uint8_t orMask = 1 << bitNum;
    uint8_t andMask = 255 - orMask;
    uint8_t value = directionFlags[index] & andMask;
    if (dir)
    {
        value += orMask;
    }
    directionFlags[index] = value;
}

void AnimationTwinkles::brightenOrDarkenEachPixel(fract8 fadeUpAmount, fract8 fadeDownAmount)
{
    for (uint16_t i = 0; i < _led_count; i++)
    {
        if (getPixelDirection(i) == GETTING_DARKER)
        {
            // This pixel is getting darker
            _leds[i] = makeDarker(_leds[i], fadeDownAmount);
        }
        else
        {
            // This pixel is getting brighter
            _leds[i] = makeBrighter(_leds[i], fadeUpAmount);
            // now check to see if we've maxxed out the brightness
            if (_leds[i].r == 255 || _leds[i].g == 255 || _leds[i].b == 255)
            {
                // if so, turn around and start getting darker
                setPixelDirection(i, GETTING_DARKER);
            }
        }
    }
}

void AnimationTwinkles::colortwinkles()
{
    EVERY_N_MILLIS(30)
    {
        // Make each pixel brighter or darker, depending on
        // its 'direction' flag.
        brightenOrDarkenEachPixel(FADE_IN_SPEED, FADE_OUT_SPEED);

        // Now consider adding a new random twinkle
        if (random8() < DENSITY)
        {
            int pos = random16(_led_count);
            if (!_leds[pos])
            {
                _leds[pos] = ColorFromPalette(gCurrentPalette, random8(), STARTING_BRIGHTNESS, NOBLEND);
                setPixelDirection(pos, GETTING_BRIGHTER);
            }
        }
    }
}

void AnimationTwinkles::cloudTwinkles()
{
    gCurrentPalette = CloudColors_p; // Blues and whites!
    colortwinkles();
}

void AnimationTwinkles::rainbowTwinkles()
{
    gCurrentPalette = RainbowColors_p;
    colortwinkles();
}

void AnimationTwinkles::snowTwinkles()
{
    CRGB w(85, 85, 85), W(CRGB::White);

    gCurrentPalette = CRGBPalette16(W, W, W, W, w, w, w, w, w, w, w, w, w, w, w, w);
    colortwinkles();
}

void AnimationTwinkles::incandescentTwinkles()
{
    CRGB l(0xE1A024);

    gCurrentPalette = CRGBPalette16(l, l, l, l, l, l, l, l, l, l, l, l, l, l, l, l);
    colortwinkles();
}

uint8_t AnimationTwinkles::attackDecayWave8(uint8_t i)
{
    if (i < 86)
    {
        return i * 3;
    }
    else
    {
        i -= 86;
        return 255 - (i + (i / 2));
    }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the
// way that incandescent bulbs fade toward 'red' as they dim.
void AnimationTwinkles::coolLikeIncandescent(CRGB &c, uint8_t phase)
{
    if (phase < 128)
        return;

    uint8_t cooling = (phase - 128) >> 4;
    c.g = qsub8(c.g, cooling);
    c.b = qsub8(c.b, cooling * 2);
}

//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as
//  input to the brightness wave function.
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the twinkleDensity.
CRGB AnimationTwinkles::computeOneTwinkle(uint32_t ms, uint8_t salt)
{
    uint16_t ticks = ms >> (8 - twinkleSpeed);
    uint8_t fastcycle8 = ticks;
    uint16_t slowcycle16 = (ticks >> 8) + salt;
    slowcycle16 += sin8(slowcycle16);
    slowcycle16 = (slowcycle16 * 2053) + 1384;
    uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

    uint8_t bright = 0;
    if (((slowcycle8 & 0x0E) / 2) < twinkleDensity)
    {
        bright = attackDecayWave8(fastcycle8);
    }

    uint8_t hue = slowcycle8 - salt;
    CRGB c;
    if (bright > 0)
    {
        c = ColorFromPalette(twinkleFoxPalette, hue, bright, NOBLEND);
        if (COOL_LIKE_INCANDESCENT == 1)
        {
            coolLikeIncandescent(c, fastcycle8);
        }
    }
    else
    {
        c = CRGB::Black;
    }
    return c;
}

//  This function loops over each pixel, calculates the
//  adjusted 'clock' that this pixel should use, and calls
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color,
//  whichever is brighter.
void AnimationTwinkles::drawTwinkles()
{
    // "PRNG16" is the pseudorandom number generator
    // It MUST be reset to the same starting value each time
    // this function is called, so that the sequence of 'random'
    // numbers that it generates is (paradoxically) stable.
    uint16_t PRNG16 = 11337;

    uint32_t clock32 = millis();

    // Set up the background color, "bg".
    // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
    // the current palette are identical, then a deeply faded version of
    // that color is used for the background color
    CRGB bg;
    if ((AUTO_SELECT_BACKGROUND_COLOR == 1) && (twinkleFoxPalette[0] == twinkleFoxPalette[1]))
    {
        bg = twinkleFoxPalette[0];
        uint8_t bglight = bg.getAverageLight();
        if (bglight > 64)
        {
            bg.nscale8_video(16); // very bright, so scale to 1/16th
        }
        else if (bglight > 16)
        {
            bg.nscale8_video(64); // not that bright, so scale to 1/4th
        }
        else
        {
            bg.nscale8_video(86); // dim, scale to 1/3rd.
        }
    }
    else
    {
        bg = gBackgroundColor; // just use the explicitly defined background color
    }

    uint8_t backgroundBrightness = bg.getAverageLight();

    for (uint16_t i = 0; i < _led_count; i++)
    {
        CRGB &pixel = _leds[i];

        PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
        uint16_t myclockoffset16 = PRNG16;         // use that number as clock offset
        PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
        // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
        uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
        uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
        uint8_t myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

        // We now have the adjusted 'clock' for this pixel, now we call
        // the function that computes what color the pixel should be based
        // on the "brightness = f( time )" idea.
        CRGB c = computeOneTwinkle(myclock30, myunique8);

        uint8_t cbright = c.getAverageLight();
        int16_t deltabright = cbright - backgroundBrightness;
        if (deltabright >= 32 || (!bg))
        {
            // If the new pixel is significantly brighter than the background color,
            // use the new color.
            pixel = c;
        }
        else if (deltabright > 0)
        {
            // If the new pixel is just slightly brighter than the background color,
            // mix a blend of the new color and the background color
            pixel = blend(bg, c, deltabright * 8);
        }
        else
        {
            // if the new pixel is not at all brighter than the background color,
            // just use the background color.
            pixel = bg;
        }
    }
}

void AnimationTwinkles::redGreenWhiteTwinkles()
{
    twinkleFoxPalette = RedGreenWhite_p;
    drawTwinkles();
}

void AnimationTwinkles::hollyTwinkles()
{
    twinkleFoxPalette = Holly_p;
    drawTwinkles();
}

void AnimationTwinkles::redWhiteTwinkles()
{
    twinkleFoxPalette = RedWhite_p;
    drawTwinkles();
}

void AnimationTwinkles::blueWhiteTwinkles()
{
    twinkleFoxPalette = BlueWhite_p;
    drawTwinkles();
}

void AnimationTwinkles::fairyLightTwinkles()
{
    twinkleFoxPalette = FairyLight_p;
    drawTwinkles();
}

void AnimationTwinkles::snow2Twinkles()
{
    twinkleFoxPalette = Snow_p;
    drawTwinkles();
}

void AnimationTwinkles::iceTwinkles()
{
    twinkleFoxPalette = Ice_p;
    drawTwinkles();
}

void AnimationTwinkles::retroC9Twinkles()
{
    twinkleFoxPalette = RetroC9_p;
    drawTwinkles();
}

void AnimationTwinkles::partyTwinkles()
{
    twinkleFoxPalette = PartyColors_p;
    drawTwinkles();
}

void AnimationTwinkles::forestTwinkles()
{
    twinkleFoxPalette = ForestColors_p;
    drawTwinkles();
}

void AnimationTwinkles::lavaTwinkles()
{
    twinkleFoxPalette = LavaColors_p;
    drawTwinkles();
}

void AnimationTwinkles::fireTwinkles()
{
    twinkleFoxPalette = HeatColors_p;
    drawTwinkles();
}

void AnimationTwinkles::cloud2Twinkles()
{
    twinkleFoxPalette = CloudColors_p;
    drawTwinkles();
}

void AnimationTwinkles::oceanTwinkles()
{
    twinkleFoxPalette = OceanColors_p;
    drawTwinkles();
}

#define TWINKLE_IMPL(name, loop_function)                                                                                                            \
    LedStripeAnimation *AnimationTwinkles##name::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)                   \
    {                                                                                                                                                \
        return new AnimationTwinkles##name(leds, led_count, animation_info);                                                                         \
    }                                                                                                                                                \
    AnimationTwinkles##name::AnimationTwinkles##name(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)                      \
        : AnimationTwinkles(leds, led_count, animation_info)                                                                                         \
    {                                                                                                                                                \
    }                                                                                                                                                \
    AnimationTwinkles##name::~AnimationTwinkles##name()                                                                                              \
    {                                                                                                                                                \
    }                                                                                                                                                \
    bool AnimationTwinkles##name::loop()                                                                                                             \
    {                                                                                                                                                \
        loop_function();                                                                                                                             \
        return true;                                                                                                                                 \
    }

TWINKLE_IMPL(IncandescentTwinkles, incandescentTwinkles)
TWINKLE_IMPL(SnowTwinkles, snowTwinkles)
TWINKLE_IMPL(RainbowTwinkles, rainbowTwinkles)
TWINKLE_IMPL(CloudTwinkles, cloudTwinkles)

TWINKLE_IMPL(RetroC9Twinkles, retroC9Twinkles)
TWINKLE_IMPL(RedWhiteTwinkles, redWhiteTwinkles)
TWINKLE_IMPL(BlueWhiteTwinkles, blueWhiteTwinkles)
TWINKLE_IMPL(RedGreenWhiteTwinkles, redGreenWhiteTwinkles)
TWINKLE_IMPL(FairyLightTwinkles, fairyLightTwinkles)
TWINKLE_IMPL(Snow2Twinkles, snow2Twinkles)
TWINKLE_IMPL(HollyTwinkles, hollyTwinkles)
TWINKLE_IMPL(IceTwinkles, iceTwinkles)
TWINKLE_IMPL(PartyTwinkles, partyTwinkles)
TWINKLE_IMPL(ForestTwinkles, forestTwinkles)
TWINKLE_IMPL(LavaTwinkles, lavaTwinkles)
TWINKLE_IMPL(FireTwinkles, fireTwinkles)
TWINKLE_IMPL(Cloud2Twinkles, cloud2Twinkles)
TWINKLE_IMPL(OceanTwinkles, oceanTwinkles)

} /* namespace fastled */
