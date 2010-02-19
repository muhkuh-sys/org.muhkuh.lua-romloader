cmake_minimum_required(VERSION 2.8)

PROJECT("muhkuh")

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)
MAKE_DIRECTORY(${LIBRARY_OUTPUT_PATH})

SET(SWIG_RUNTIME_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/swig_runtime/)
MAKE_DIRECTORY(${SWIG_RUNTIME_OUTPUT_PATH})

SET(MODULE_LUA_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/lua)
MAKE_DIRECTORY(${MODULE_LUA_OUTPUT_PATH})

SET(MODULE_PYTHON_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/python)
MAKE_DIRECTORY(${MODULE_PYTHON_OUTPUT_PATH})
# NOTE: This is a little bit dirty as it sets the path for all generated modules, but it is only needed for python.
# TODO: How to specify this for a single target?
SET(CMAKE_SWIG_OUTDIR ${MODULE_PYTHON_OUTPUT_PATH})

FIND_PACKAGE(wxWidgets REQUIRED base core xml net aui adv html xrc)


OPTION(USE_SYSTEM_LUA "Use the systemwide installed lua version instead of the muhkuh internal one." OFF)
IF(USE_SYSTEM_LUA)
	# Use the system version.
	FIND_PACKAGE(Lua51 REQUIRED)
ELSE(USE_SYSTEM_LUA)
	# Use the internal lua version.
	SET(LUA51_FOUND TRUE)
	SET(LUA_LIBRARIES lua)
	SET(LUA_INCLUDE_DIR ${CMAKE_HOME_DIRECTORY}/lua/lib/lua-5.1.4/src/)
ENDIF(USE_SYSTEM_LUA)

FIND_PACKAGE(PythonLibs)

ADD_CUSTOM_COMMAND(
	OUTPUT ${SWIG_RUNTIME_OUTPUT_PATH}swigluarun.h
	COMMAND "${SWIG_EXECUTABLE}" -lua -external-runtime ${SWIG_RUNTIME_OUTPUT_PATH}swigluarun.h
)
ADD_CUSTOM_TARGET(SWIG_LUA_RUNTIME DEPENDS ${SWIG_RUNTIME_OUTPUT_PATH}swigluarun.h)

ADD_CUSTOM_COMMAND(
	OUTPUT ${SWIG_RUNTIME_OUTPUT_PATH}swigpythonrun.h
	COMMAND "${SWIG_EXECUTABLE}" -python -external-runtime ${SWIG_RUNTIME_OUTPUT_PATH}swigpythonrun.h
)
ADD_CUSTOM_TARGET(SWIG_PYTHON_RUNTIME DEPENDS ${SWIG_RUNTIME_OUTPUT_PATH}swigpythonrun.h)


# NOTE: Most of this macro is copied from the UseSWIG.cmake module without
#       understanding a single bit. :)
MACRO(SWIG_BUILD_WRAPPER target_variable language infile)
	SET(swig_full_infile ${infile})
	GET_FILENAME_COMPONENT(swig_source_file_path "${infile}" PATH)
	GET_FILENAME_COMPONENT(swig_source_file_name_we "${infile}" NAME_WE)
	GET_SOURCE_FILE_PROPERTY(swig_source_file_generated ${infile} GENERATED)
	GET_SOURCE_FILE_PROPERTY(swig_source_file_cplusplus ${infile} CPLUSPLUS)
	GET_SOURCE_FILE_PROPERTY(swig_source_file_flags ${infile} SWIG_FLAGS)
	IF("${swig_source_file_flags}" STREQUAL "NOTFOUND")
		SET(swig_source_file_flags "")
	ENDIF("${swig_source_file_flags}" STREQUAL "NOTFOUND")
	SET(swig_source_file_fullname "${infile}")
	IF(${swig_source_file_path} MATCHES "^${CMAKE_CURRENT_SOURCE_DIR}")
		STRING(REGEX REPLACE 
			"^${CMAKE_CURRENT_SOURCE_DIR}" ""
			swig_source_file_relative_path
			"${swig_source_file_path}")
	ELSE(${swig_source_file_path} MATCHES "^${CMAKE_CURRENT_SOURCE_DIR}")
		IF(${swig_source_file_path} MATCHES "^${CMAKE_CURRENT_BINARY_DIR}")
			STRING(REGEX REPLACE
				"^${CMAKE_CURRENT_BINARY_DIR}" ""
				swig_source_file_relative_path
				"${swig_source_file_path}")
			SET(swig_source_file_generated 1)
		ELSE(${swig_source_file_path} MATCHES "^${CMAKE_CURRENT_BINARY_DIR}")
			SET(swig_source_file_relative_path "${swig_source_file_path}")
			IF(swig_source_file_generated)
				SET(swig_source_file_fullname "${CMAKE_CURRENT_BINARY_DIR}/${infile}")
			ELSE(swig_source_file_generated)
				SET(swig_source_file_fullname "${CMAKE_CURRENT_SOURCE_DIR}/${infile}")
			ENDIF(swig_source_file_generated)
		ENDIF(${swig_source_file_path} MATCHES "^${CMAKE_CURRENT_BINARY_DIR}")
	ENDIF(${swig_source_file_path} MATCHES "^${CMAKE_CURRENT_SOURCE_DIR}")

	IF(swig_source_file_cplusplus)
		SET(swig_generated_file_suffix "cpp")
	ELSE(swig_source_file_cplusplus)
		SET(swig_generated_file_suffix "c")
	ENDIF(swig_source_file_cplusplus)

	SET(swig_generated_file_fullname "${CMAKE_CURRENT_BINARY_DIR}/${swig_source_file_name_we}_${language}_wrapper.${swig_generated_file_suffix}")

	ADD_CUSTOM_COMMAND(
		OUTPUT "${swig_generated_file_fullname}"
		COMMAND "${SWIG_EXECUTABLE}"
		ARGS	"-${language}"
			${swig_source_file_flags}
			${CMAKE_SWIG_FLAGS}
			${swig_special_flags}
			${swig_extra_flags}
			${swig_include_dirs}
			-o "${swig_generated_file_fullname}"
			"${swig_source_file_fullname}"
		MAIN_DEPENDENCY "${swig_source_file_fullname}"
		COMMENT "Generate Swig wrapper")
	SET_SOURCE_FILES_PROPERTIES("${target}" PROPERTIES GENERATED 1)

	SET(${target_variable} ${swig_generated_file_fullname})
ENDMACRO(SWIG_BUILD_WRAPPER)


# Do not show msvs security warnings about unsafe functions.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)


add_subdirectory(lua)
add_subdirectory(modules)
add_subdirectory(plugins)
