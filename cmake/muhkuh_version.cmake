#----------------------------------------------------------------------------
#
# Read the version file and parse it.
#

# Set the filename for the muhkuh version file.
SET(MUHKUH_VERSION_FILE "${CMAKE_HOME_DIRECTORY}/muhkuh_version.txt")
IF(EXISTS ${MUHKUH_VERSION_FILE})
	# Set the regular expression for the muhkuh version. It is a sequence of 3 numbers separated by dots.
	SET(MUHKUH_VERSION_REXEX "([0-9]+)\\.([0-9]+)\\.([0-9]+)")
	# Read the contents of the muhkuh version file. Accept only data matching the version regex.
	FILE(STRINGS ${MUHKUH_VERSION_FILE} MUHKUH_VERSION_ALL REGEX ${MUHKUH_VERSION_REXEX})
	# Check if a valid version number was found.
	IF("${MUHKUH_VERSION_ALL}" STREQUAL "")
		# No valid version number found. The file contains garbage, which is a fatal error!
		MESSAGE(FATAL_ERROR "The file ${MUHKUH_VERSION_FILE} contains no valid version in the form number.number.number .")
	ELSE("${MUHKUH_VERSION_ALL}" STREQUAL "")
		STRING(REGEX MATCH "${MUHKUH_VERSION_REXEX}" MUHKUH_VERSION_ALL ${MUHKUH_VERSION_ALL})
		SET(MUHKUH_VERSION_MAJ ${CMAKE_MATCH_1})
		SET(MUHKUH_VERSION_MIN ${CMAKE_MATCH_2})
		SET(MUHKUH_VERSION_SUB ${CMAKE_MATCH_3})
		MESSAGE("File ${MUHKUH_VERSION_FILE} found, version info: ${MUHKUH_VERSION_MAJ}.${MUHKUH_VERSION_MIN}.${MUHKUH_VERSION_SUB}")
	ENDIF("${MUHKUH_VERSION_ALL}" STREQUAL "")
ELSE(EXISTS ${MUHKUH_VERSION_FILE})
	SET(MUHKUH_VERSION_MAJ 1)
	SET(MUHKUH_VERSION_MIN 0)
	SET(MUHKUH_VERSION_SUB 0)

	IF(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg")
		FIND_PROGRAM(HG_EXECUTABLE
		             NAMES hg
		             PATH_SUFFIXES Mercurial
		             DOC "hg command line client"
		)

		IF(HG_EXECUTABLE)
			MESSAGE("Mercurial data and client found, generating version info from revision number.")
			EXECUTE_PROCESS(COMMAND ${HG_EXECUTABLE} id -i ${CMAKE_CURRENT_SOURCE_DIR}
			                OUTPUT_VARIABLE MUHKUH_VERSION_SUB
			                ERROR_QUIET
			                OUTPUT_STRIP_TRAILING_WHITESPACE)
			MESSAGE("Current revision is ${MUHKUH_VERSION_SUB}")
		ELSE(HG_EXECUTABLE)
			MESSAGE(WARNING "No Mercurial client found, using a fixed version.")
		ENDIF(HG_EXECUTABLE)
	ELSE(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg")
		MESSAGE(WARNING "No file ${MUHKUH_VERSION_FILE} and no mercurial data found, using a fixed version.")
	ENDIF(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg")
ENDIF(EXISTS ${MUHKUH_VERSION_FILE})

# Get the current year.
# There seems to be no built-in way in cmake to get the current year.
# But we have python for the MBS build system anyway so we can use it.
FIND_PACKAGE(PythonInterp REQUIRED)
EXECUTE_PROCESS(COMMAND "${PYTHON_EXECUTABLE}" -c "import datetime; print datetime.date.today().year"
                OUTPUT_VARIABLE MUHKUH_VERSION_YEAR
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)


