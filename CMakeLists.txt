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
generate_arduino_firmware(blink
    SRCS auto_greenhouse.cpp
    LIBS greenhouse-lib RTC-lib DHT-lib Sunrise-lib LCDI2C-lib i2ckeypad-lib
    PORT  /dev/ttyACM0
    SERIAL minicom -D @SERIAL_PORT@ -b 9600
    BOARD uno)
