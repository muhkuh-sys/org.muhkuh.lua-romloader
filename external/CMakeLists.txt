CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

PROJECT("romloader_externals")

# Python is used for the firmware build script and some test scripts.
FIND_PACKAGE(Python3 REQUIRED COMPONENTS Interpreter)

ADD_SUBDIRECTORY(libusb)
