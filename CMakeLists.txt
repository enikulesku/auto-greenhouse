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
generate_arduino_library(RTClib
    SRCS libs/RTClib/RTClib.cpp
    HDRS libs/RTClib/RTClib.h)
#====================================================================#
# DHT series of temperature/humidity sensors library
#====================================================================#
generate_arduino_library(DHTlib
    SRCS libs/DHTlib/DHT.cpp
    HDRS libs/DHTlib/DHT.h)

#====================================================================#
# Firmware
#====================================================================#
generate_arduino_firmware(blink
    SRCS blink.cpp
    LIBS blink_lib RTClib DHTlib
    PORT  /dev/ttyACM0
    SERIAL minicom -D @SERIAL_PORT@ -b 9600
    BOARD uno)
