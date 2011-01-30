cmake_minimum_required(VERSION 2.8)


#-----------------------------------------------------------------------------

PROJECT("romloader_uart")

SET(romloader_uart_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

if(${PYTHONLIBS_FOUND})
	include_directories(${PYTHON_INCLUDE_DIRS})
endif(${PYTHONLIBS_FOUND})

if(${LUA51_FOUND})
	include_directories(${LUA_INCLUDE_DIR})
endif(${LUA51_FOUND})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})


set_source_files_properties(romloader_uart.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE romloader_uart.i PROPERTY SWIG_FLAGS ${USB_DEFINES} -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)


IF(UNIX)
	SET(romloader_uart_platform_SOURCES
		romloader_uart_device_linux.cpp
	)

	SET(romloader_uart_platform_HEADERS
		romloader_uart_device_linux.h
	)
ELSE(UNIX)
	IF(WIN32)
		SET(romloader_uart_platform_SOURCES
			romloader_uart_device_win.cpp
		)
		
		SET(romloader_uart_platform_HEADERS
			romloader_uart_device_win.h
		)
	ENDIF(WIN32)
ENDIF(UNIX)


SET(romloader_uart_SOURCES
	romloader_uart_device.cpp
	romloader_uart_main.cpp
	${romloader_uart_platform_SOURCES}
)

SET(romloader_uart_HEADERS
	romloader_uart_device.h
	romloader_uart_main.h
	${romloader_uart_platform_HEADERS}
)


add_library(romloader_uart_lib STATIC ${romloader_uart_SOURCES})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	SET_TARGET_PROPERTIES(romloader_uart_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL GNU)


if(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(romloader_uart lua romloader_uart.i)
	SWIG_LINK_LIBRARIES(romloader_uart romloader_uart_lib ${wxWidgets_LIBRARIES} ${LUA_LIBRARIES} romloader_lib)
	set_property(TARGET romloader_uart PROPERTY PREFIX "")

	# Install the targets.
	INSTALL(TARGETS romloader_uart DESTINATION ${INSTALL_DIR_LUA_MODULES})
endif(${LUA51_FOUND})


#if(${PYTHONLIBS_FOUND})
#	# Build the Python module.
#	swig_add_module(romloader_uartmodule python romloader_uart.i)
#	swig_link_libraries(romloader_uartmodule romloader_uart_lib ${PYTHON_LIBRARIES} ${USB_LIBRARIES} romloader)
#	set_target_properties(_romloader_uartmodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_PYTHON_OUTPUT_PATH})
#endif(${PYTHONLIBS_FOUND})

