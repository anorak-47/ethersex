
#include "animation/animation.h"
#include "sensor/sensor.h"

void ledstripe_animation_init(void)
{
	sensors_init();
	animation_init();
}

/*
  - Ethersex META -
  header(hardware/ledstripe/animation/animation.h)
  header(hardware/ledstripe/ledstripe_loop.h)
  mainloop(animation_loop)
  init(ledstripe_animation_init)
*/
