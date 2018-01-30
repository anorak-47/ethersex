
#include "animation_basic_tests.h"

namespace fastled
{

AnimationBasicTests::AnimationBasicTests(CRGB *leds, uint16_t led_count, animation *animation_info)
    : LedStripeAnimation(leds, led_count, animation_info)
{
}

AnimationBasicTests::~AnimationBasicTests()
{
}

void AnimationBasicTests::setOption(uint8_t option)
{
    _option = option;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// RGB Calibration code
//
// Use this sketch to determine what the RGB ordering for your chipset should be.  Steps for setting up to use:

// * Uncomment the line in setup that corresponds to the LED chipset that you are using.  (Note that they
//   all explicitly specify the RGB order as RGB)
// * Define DATA_PIN to the pin that data is connected to.
// * (Optional) if using software SPI for chipsets that are SPI based, define CLOCK_PIN to the clock pin
// * Compile/upload/run the sketch

// You should see six leds on.  If the RGB ordering is correct, you should see 1 red led, 2 green
// leds, and 3 blue leds.  If you see different colors, the count of each color tells you what the
// position for that color in the rgb orering should be.  So, for example, if you see 1 Blue, and 2
// Red, and 3 Green leds then the rgb ordering should be BRG (Blue, Red, Green).

// You can then test this ordering by setting the RGB ordering in the addLeds line below to the new ordering
// and it should come out correctly, 1 red, 2 green, and 3 blue.
//
//////////////////////////////////////////////////

void AnimationBasicTests::color_test()
{
    _leds[0] = CRGB(255, 0, 0);
    _leds[1] = CRGB(0, 255, 0);
    _leds[2] = CRGB(0, 255, 0);
    _leds[3] = CRGB(0, 0, 255);
    _leds[4] = CRGB(0, 0, 255);
    _leds[5] = CRGB(0, 0, 255);
}

void AnimationBasicTests::blink()
{
    _leds[0] = _startIndex++ % 2 ? CRGB::Red : CRGB::Black;
}

void AnimationBasicTests::mover()
{
	_leds[_startIndex] = CRGB::Black;
	_startIndex++;
    if (_startIndex >= _led_count)
        _startIndex = 0;
    _leds[_startIndex] = CRGB::White;
}

bool AnimationBasicTests::loop()
{
    switch (_option)
    {
    case 0:
    	color_test();
    	break;
    case 1:
    	blink();
    	break;
    case 2:
    	mover();
    	break;
    }

    return (_option <= 2);
}

} /* namespace fastled */
