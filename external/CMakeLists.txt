CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

PROJECT("romloader_externals")

# Python is used for the firmware build script and some test scripts.
FIND_PACKAGE(PythonInterp 2.7 REQUIRED)

ADD_SUBDIRECTORY(libusb)
