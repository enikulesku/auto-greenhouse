#=============================================================================#
# Author: enikulesku                                                          #
# Date:   10.01.2015                                                          #
#                                                                             #
# Description: AutoGreenhouse                                             #
#                                                                             #
#=============================================================================#
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

set(ARDUINO_DEFAULT_BOARD uno)

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(AutoGreenhouse C CXX)

print_board_list()
print_programmer_list()

#====================================================================#
# Blink library
#====================================================================#
generate_arduino_library(blink_lib
    SRCS blink_lib.cpp
    HDRS blink_lib.h)

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
# Firmware
#====================================================================#
generate_arduino_firmware(blink
    SRCS blink.cpp
    LIBS blink_lib RTC-lib DHT-lib Sunrise-lib LCDI2C-lib
    PORT  /dev/ttyACM0
    SERIAL minicom -D @SERIAL_PORT@ -b 9600
    BOARD uno)
