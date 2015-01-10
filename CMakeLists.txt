#=============================================================================#
# Author: enikulesku                                                          #
# Date:   10.01.2015                                                          #
#                                                                             #
# Description: Arduino Greenhouse                                             #
#                                                                             #
#=============================================================================#
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

set(ARDUINO_DEFAULT_BOARD uno)

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(ArduinoGreenhouse C CXX)

print_board_list()
print_programmer_list()

#====================================================================#
# Blink library
#====================================================================#
generate_arduino_library(blink_lib
    SRCS blink_lib.cpp
    HDRS blink_lib.h)

#====================================================================#
# RTC library
#====================================================================#
generate_arduino_library(RTClib
    SRCS libs/RTClib/RTClib.cpp
    HDRS libs/RTClib/RTClib.h)

#====================================================================#
# Firmware
#====================================================================#
generate_arduino_firmware(blink
    SRCS blink.cpp
    LIBS blink_lib RTClib
    PORT  /dev/ttyACM0
    SERIAL picocom @SERIAL_PORT@
    BOARD uno)
