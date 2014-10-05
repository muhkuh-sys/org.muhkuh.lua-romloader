#----------------------------------------------------------------------------
#
# Read the version file and parse it.
#

# Set the filename for the muhkuh version file.
# NOTE: this file is included from 2 cmake projects with different home dirs. Use the current path as a base to get to muhkuh_version.txt .
SET(MUHKUH_VERSION_FILE "${CMAKE_CURRENT_LIST_DIR}/../muhkuh_version.txt")

# Set the regular expression for the muhkuh version. It is a sequence of 3 numbers separated by dots.
SET(MUHKUH_VERSION_REGEX "([0-9]+)\\.([0-9]+)\\.([0-9]+)")

# Read the contents of the muhkuh version file. Accept only data matching the version regex.
FILE(STRINGS ${MUHKUH_VERSION_FILE} MUHKUH_VERSION_ALL REGEX ${MUHKUH_VERSION_REGEX})

# Check if a valid version number was found.
IF("${MUHKUH_VERSION_ALL}" STREQUAL "")
	# No valid version number found. The file contains garbage, which is a fatal error!
	MESSAGE(FATAL_ERROR "The file ${MUHKUH_VERSION_FILE} contains no valid version in the form number.number.hash .")
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
	EXECUTE_PROCESS(COMMAND "${PYTHON_EXECUTABLE}" -c "import datetime; print datetime.date.today().year"
	                OUTPUT_VARIABLE MUHKUH_VERSION_YEAR
	                ERROR_QUIET
	                OUTPUT_STRIP_TRAILING_WHITESPACE)
ELSE(${CMAKE_VERSION} VERSION_LESS "2.8.11")
	# This is CMake 2.8.11 or later.
	STRING(TIMESTAMP MUHKUH_VERSION_YEAR "%Y")
ENDIF(${CMAKE_VERSION} VERSION_LESS "2.8.11")
