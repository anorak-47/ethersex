
# Add inputs and outputs from these tool invocations to the build variables 
ARDUINO_CPPSRC := \
$(ARDUINO_PATH)/cores/arduino/WMath.cpp \
$(ARDUINO_PATH)/cores/arduino/WString.cpp \
$(ARDUINO_PATH)/cores/arduino/new.cpp \
$(ARDUINO_PATH)/cores/arduino/abi.cpp \
$(ARDUINO_PATH)/cores/arduino/wiring_extras.cpp

ARDUINO_SRC := \
$(ARDUINO_PATH)/cores/arduino/wiring.c \
$(ARDUINO_PATH)/cores/arduino/hooks.c
