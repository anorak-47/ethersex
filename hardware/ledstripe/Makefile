TOPDIR ?= ../..
include $(TOPDIR)/.config

ARDUINO_VARIANT = 40pin	

FASTLED_PATH    = hardware/ledstripe/FastLED
ARDUINO_PATH    = hardware/ledstripe/arduino-1.0.1
ANIMATION_PATH  = hardware/ledstripe/animation
SENSOR_PATH  = hardware/ledstripe/sensor

ANIMAITON_CPPSRC = \
		$(ANIMATION_PATH)/animation.cpp \
		$(ANIMATION_PATH)/animation_names.cpp \
		$(ANIMATION_PATH)/animation_setup.cpp \
		$(ANIMATION_PATH)/led_stripe_animation.cpp \
		$(ANIMATION_PATH)/animation_fire2012.cpp \
		$(ANIMATION_PATH)/animation_gradient.cpp \
		$(ANIMATION_PATH)/animation_rotating_palette.cpp \
		$(ANIMATION_PATH)/animation_basic_tests.cpp \
		$(ANIMATION_PATH)/animation_little.cpp \
		$(ANIMATION_PATH)/animation_inoise_fire.cpp \
		$(ANIMATION_PATH)/animation_inoise_mover.cpp \
		$(ANIMATION_PATH)/animation_inoise_pal.cpp \
		$(ANIMATION_PATH)/animation_dot_beat.cpp \
		$(ANIMATION_PATH)/animation_serendipitous.cpp \
		$(ANIMATION_PATH)/animation_mover.cpp \
		$(ANIMATION_PATH)/animation_beatwave.cpp \
		$(ANIMATION_PATH)/animation_sensor.cpp \
		$(ANIMATION_PATH)/animation_noise16.cpp \
		$(ANIMATION_PATH)/animation_rainbow_beat.cpp \
		$(ANIMATION_PATH)/animation_twinkles.cpp \
		
		
				
ANIMAITON_SRC = \
		$(ANIMATION_PATH)/shitty_arduino_include_stuff.c
		
		
SENSOR_CPPSRC = \
		$(SENSOR_PATH)/sensor.cpp \
		$(SENSOR_PATH)/MCP980X.cpp \
		
		
		

include hardware/ledstripe/FastLED.mk
include hardware/ledstripe/arduino.mk

$(LEDSTRIPE_SUPPORT)_CPPSRC += $(FASTLED_CPPSRC) $(ARDUINO_CPPSRC) $(ANIMAITON_CPPSRC) $(SENSOR_CPPSRC)
$(LEDSTRIPE_SUPPORT)_SRC += $(ARDUINO_SRC) $(ANIMAITON_SRC) hardware/ledstripe/ledstripe_loop.c hardware/ledstripe/shell_animation.c	
$(LEDSTRIPE_SUPPORT)_ECMD_SRC += hardware/ledstripe/ledstripe_ecmd.c

CPPFLAGS += -Ihardware/ledstripe/ -I$(FASTLED_PATH)/ -I$(SENSOR_PATH)/ -I$(ARDUINO_PATH)/cores/arduino/ -I$(ARDUINO_PATH)/variants/$(ARDUINO_VARIANT) -I$(ANIMATION_PATH)/
CXXFLAGS += -DFASTLED_HAS_MILLIS  -include Arduino.h

##############################################################################
# generic fluff
include $(TOPDIR)/scripts/rules.mk



