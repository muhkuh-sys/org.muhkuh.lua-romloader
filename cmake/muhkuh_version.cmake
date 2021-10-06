#----------------------------------------------------------------------------
#
# Read the version file and parse it.
#

# Python is needed for the MBS setup tool.
IF(CMAKE_VERSION VERSION_LESS 3.12.0)
	FIND_PACKAGE(PythonInterp 3 REQUIRED)
	SET(PYTHON_INTERPRETER "${PYTHON_EXECUTABLE}")
ELSE(CMAKE_VERSION VERSION_LESS 3.12.0)
	FIND_PACKAGE(Python3 REQUIRED COMPONENTS Interpreter)
	SET(PYTHON_INTERPRETER "${Python3_EXECUTABLE}")
ENDIF(CMAKE_VERSION VERSION_LESS 3.12.0)

# Set the filename for the setup.xml file.
# NOTE: this file is included from 2 cmake projects with different home dirs. Use the current path as a base to get to setup.xml .
SET(MUHKUH_VERSION_FILE "${CMAKE_CURRENT_LIST_DIR}/../setup.xml")

# This is the path to the helper Python script.
SET(MUHKUH_SETUP_TOOL "${CMAKE_CURRENT_LIST_DIR}/mbs_setup_tool.py")

# Set the regular expression for the muhkuh version. It is a sequence of 3 numbers separated by dots.
SET(MUHKUH_VERSION_REGEX "([0-9]+)\\.([0-9]+)\\.([0-9]+)")

# Run the setup tool and capture the output.
MESSAGE("Running ${PYTHON_INTERPRETER} ${MUHKUH_SETUP_TOOL} ${MUHKUH_VERSION_FILE}")
EXECUTE_PROCESS(COMMAND "${PYTHON_INTERPRETER}" "${MUHKUH_SETUP_TOOL}" "${MUHKUH_VERSION_FILE}"
                OUTPUT_VARIABLE MUHKUH_VERSION_ALL
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# Check if a valid version number was found.
IF("${MUHKUH_VERSION_ALL}" STREQUAL "")
	# No valid version number found. The file contains garbage, which is a fatal error!
	MESSAGE(FATAL_ERROR "Failed to extract the version number from  ${MUHKUH_VERSION_FILE}.")
ENDIF("${MUHKUH_VERSION_ALL}" STREQUAL "")

STRING(REGEX MATCH ${MUHKUH_VERSION_REGEX} MUHKUH_VERSION_ALL ${MUHKUH_VERSION_ALL})
SET(MUHKUH_VERSION_MAJ ${CMAKE_MATCH_1})
SET(MUHKUH_VERSION_MIN ${CMAKE_MATCH_2})
SET(MUHKUH_VERSION_SUB ${CMAKE_MATCH_3})


MESSAGE("Version info: ${MUHKUH_VERSION_MAJ}.${MUHKUH_VERSION_MIN}.${MUHKUH_VERSION_SUB}.")


# Get the current year.
# There was no no built-in way in cmake to get the current year before
# version 2.8.11.
IF(${CMAKE_VERSION} VERSION_LESS "2.8.11")
	# This is a CMake below 2.8.11. Use python for this task.
	EXECUTE_PROCESS(COMMAND "${PYTHON_INTERPRETER}" -c "import datetime; print datetime.date.today().year"
	                OUTPUT_VARIABLE MUHKUH_VERSION_YEAR
	                ERROR_QUIET
	                OUTPUT_STRIP_TRAILING_WHITESPACE)
ELSE(${CMAKE_VERSION} VERSION_LESS "2.8.11")
	# This is CMake 2.8.11 or later.
	STRING(TIMESTAMP MUHKUH_VERSION_YEAR "%Y")
ENDIF(${CMAKE_VERSION} VERSION_LESS "2.8.11")
