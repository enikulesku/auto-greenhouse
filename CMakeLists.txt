#=============================================================================#
# Author: enikulesku                                                          #
# Date:   10.01.2015                                                          #
#                                                                             #
# Description: AutoGreenhouse                                                 #
#                                                                             #
#=============================================================================#
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

set(ARDUINO_DEFAULT_BOARD uno)

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(AutoGreenhouse C CXX)

#print_board_list()
#print_programmer_list()

#====================================================================#
# Greenhouse library
#====================================================================#
generate_arduino_library(greenhouse-lib
    SRCS libs/greenhouse/greenhouse.cpp
    HDRS libs/greenhouse/greenhouse.h)
#====================================================================#
# Greenhouse Debug library
#====================================================================#
generate_arduino_library(greenhouse-debug-lib
    SRCS libs/greenhouse-debug/greenhouse_debug.cpp
    HDRS libs/greenhouse-debug/greenhouse_debug.h)
#====================================================================#
# Greenhouse LCD library
#====================================================================#
generate_arduino_library(greenhouse-lcd-lib
    SRCS libs/greenhouse-lcd/greenhouse_lcd.cpp
    HDRS libs/greenhouse-lcd/greenhouse_lcd.h)

#====================================================================#
# Utils library
#====================================================================#
generate_arduino_library(utils-lib
    SRCS libs/utils/utils.cpp
    HDRS libs/utils/utils.h)

#====================================================================#
# Real-Time Clock library
#====================================================================#
generate_arduino_library(RTC-lib
    SRCS libs/RTC/RTClib.cpp
    HDRS libs/RTC/RTClib.h)
#====================================================================#
# DHT series of temperature/humidity sensors library
#====================================================================#
generate_arduino_library(DHT-lib
    SRCS libs/DHT/DHT.cpp
    HDRS libs/DHT/DHT.h)
#====================================================================#
# Sunrise library
#====================================================================#
generate_arduino_library(Sunrise-lib
    SRCS libs/Sunrise/Sunrise.cpp
    HDRS libs/Sunrise/Sunrise.h)
#====================================================================#
# LCDI2C library
#====================================================================#
generate_arduino_library(LCDI2C-lib
    SRCS libs/LCDI2C/LiquidCrystal_I2C.cpp
    HDRS libs/LCDI2C/LiquidCrystal_I2C.h)
#====================================================================#
# i2ckeypad library
#====================================================================#
generate_arduino_library(i2ckeypad-lib
    SRCS libs/i2ckeypad/i2ckeypad.cpp
    HDRS libs/i2ckeypad/i2ckeypad.h)

#====================================================================#
# Firmware
#====================================================================#
generate_arduino_firmware(greenhouse
    SRCS auto_greenhouse.cpp
    LIBS greenhouse-lib RTC-lib DHT-lib Sunrise-lib LCDI2C-lib i2ckeypad-lib greenhouse-debug-lib greenhouse-lcd-lib utils-lib
    PORT  /dev/ttyACM0
    SERIAL python2.7 ${CMAKE_CURRENT_SOURCE_DIR}/int_test.py -D @SERIAL_PORT@ -b 115200
    BOARD uno)

#====================================================================#
# Integration test
#====================================================================#
add_custom_target(int-test
    DEPENDS greenhouse-upload greenhouse-serial)
