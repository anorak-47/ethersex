
#include "animation_little.h"

namespace fastled
{

AnimationLittle::AnimationLittle(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)
    : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationLittle::~AnimationLittle()
{
}

/*
static LedStripeAnimation *AnimationLittle::create(CRGB *leds, uint16_t led_count, animation *animation_info)
{
    return new AnimationLittle(leds, led_count, animation_info);
}
*/

void AnimationLittle::setOption(uint8_t option)
{
    _option = option;
}

void AnimationLittle::rainbow()
{
    // FastLED's built-in rainbow generator
    fill_rainbow(_leds, _led_count, gHue, 7);
}

void AnimationLittle::rainbowWithGlitter()
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void AnimationLittle::addGlitter(fract8 chanceOfGlitter)
{
    if (random8() < chanceOfGlitter)
    {
        _leds[random16(_led_count)] += CRGB::White;
    }
}

void AnimationLittle::confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(_leds, _led_count, 10);
    int pos = random16(_led_count);
    _leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void AnimationLittle::sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(_leds, _led_count, 20);
    int pos = beatsin16(13, 0, _led_count - 1);
    _leds[pos] += CHSV(gHue, 255, 192);
}

void AnimationLittle::bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < _led_count; i++)
    { // 9948
        _leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void AnimationLittle::juggle()
{
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(_leds, _led_count, 20);
    uint8_t dothue = 0;
    for (int i = 0; i < 8; i++)
    {
        _leds[beatsin16(i + 7, 0, _led_count - 1)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

bool AnimationLittle::loop()
{
    // do some periodic updates
    EVERY_N_MILLISECONDS(20)
    {
        gHue++;
    } // slowly cycle the "base color" through the rainbow

    switch (_option)
    {
    case 0:
        rainbow();
        break;
    case 1:
        rainbowWithGlitter();
        break;
    case 2:
        confetti();
        break;
    case 3:
        sinelon();
        break;
    case 4:
        bpm();
        break;
    case 5:
        juggle();
        break;
    }

    return (_option <= 5);
}

#define LITTLE_IMPL(name, loop_function)                                                                                                             \
    LedStripeAnimation *AnimationLittle##name::create(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)                     \
    {                                                                                                                                                \
        return new AnimationLittle##name(leds, led_count, animation_info);                                                                           \
    }                                                                                                                                                \
    AnimationLittle##name::AnimationLittle##name(CRGB *leds, uint16_t led_count, animation_configuration_t *animation_info)                          \
        : AnimationLittle(leds, led_count, animation_info)                                                                                           \
    {                                                                                                                                                \
    }                                                                                                                                                \
    AnimationLittle##name::~AnimationLittle##name()                                                                                                  \
    {                                                                                                                                                \
    }                                                                                                                                                \
    bool AnimationLittle##name::loop()                                                                                                               \
    {                                                                                                                                                \
        loop_function();                                                                                                                             \
        return true;                                                                                                                                 \
    }

LITTLE_IMPL(Rainbow, rainbow)
LITTLE_IMPL(RainbowWithGlitter, rainbowWithGlitter)
LITTLE_IMPL(Confetti, confetti)
LITTLE_IMPL(Sinelon, sinelon)
LITTLE_IMPL(Bpm, bpm)
LITTLE_IMPL(Juggle, juggle)

} /* namespace fastled */
