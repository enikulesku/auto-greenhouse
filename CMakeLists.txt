#=============================================================================#
# Author: QueezyTheGreat                                                      #
# Date:   26.04.2011                                                          #
#                                                                             #
# Description: Arduino CMake example                                          #
#                                                                             #
#=============================================================================#
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

#set(ARDUINO_DEFAULT_BOARD uno) # Default Board ID, when not specified
#set(ARDUINO_DEFAULT_PORT /dev/ttyACM0) # Default Port, when not specified

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(ArduinoGarden C CXX)

print_board_list()
print_programmer_list()

generate_arduino_library(blink_lib
    SRCS blink_lib.cpp
    HDRS blink_lib.h
    BOARD uno)

#====================================================================#
# Advanced firwmare example
#====================================================================#
generate_arduino_firmware(blink
    SRCS blink.cpp
    LIBS blink_lib
    PORT  /dev/ttyACM0
    SERIAL picocom @SERIAL_PORT@
    BOARD uno)
