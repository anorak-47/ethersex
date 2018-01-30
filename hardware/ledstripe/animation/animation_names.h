#pragma once

#include <inttypes.h>
#include "animation_config.h"

#if FASTLED_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

enum _e_animation_names
{
	AnimationTests = 0,
	AnimationLittle,
	AnimationMover,
	AnimationPalette,
	AnimationGradient,
	AnimationFire2012,
	AnimationInoiseFire,
	AnimationInoiseMover,
	AnimationInoisePal,
	AnimationDotBeat,
	AnimationSerendipitous,
	AnimationBeatWave,
	AnimationNoise16,
	AnimationRainbowBeat,
	AnimationSensorVisualisation
};

typedef enum _e_animation_names animation_names;

#ifdef __cplusplus
}
#endif

#define ANIMATION_COUNT 15

void animation_set_for_stripe(uint8_t stripe, animation_names animation);

#endif

