#----------------------------------------------------------------------------
#
# Read the version file and parse it.
#

# Set the filename for the muhkuh version file.
SET(MUHKUH_VERSION_FILE "${CMAKE_HOME_DIRECTORY}/muhkuh_version.txt")

# Set the regular expression for the muhkuh version. It is a sequence of 3 numbers separated by dots.
SET(MUHKUH_VERSION_REGEX "([0-9]+)\\.([0-9]+)\\.([0-9]+)")

# Set the regular expression for the ID in the .hg_archival.txt file.
SET(HG_ARCHIVAL_VERSION_REGEX "node: ([0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])[0-9a-fA-F]+")

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



# Get the VCS version.
IF(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg")
	FIND_PROGRAM(HG_EXECUTABLE
	             NAMES hg
	             PATH_SUFFIXES Mercurial
	             DOC "hg command line client"
	)
	
	IF(HG_EXECUTABLE)
		EXECUTE_PROCESS(COMMAND ${HG_EXECUTABLE} id -i ${CMAKE_CURRENT_SOURCE_DIR}
		                OUTPUT_VARIABLE MUHKUH_VERSION_VCS
		                ERROR_QUIET
		                OUTPUT_STRIP_TRAILING_WHITESPACE)
	ELSE(HG_EXECUTABLE)
		MESSAGE(WARNING "No Mercurial client found, can not get id of the working folder.")
		SET(MUHKUH_VERSION_VCS "unknown")
	ENDIF(HG_EXECUTABLE)
ELSEIF(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg_archival.txt")
	FILE(STRINGS "${CMAKE_HOME_DIRECTORY}/.hg_archival.txt" TMP_NODE_LINE REGEX ${HG_ARCHIVAL_VERSION_REGEX})
	IF("${TMP_NODE_LINE}" STREQUAL "")
		# No valid version number found. The file contains garbage, which is a fatal error!
		MESSAGE(FATAL_ERROR "The file ${CMAKE_HOME_DIRECTORY}/.hg_archival.txt has an unknown format!")
	ENDIF("${TMP_NODE_LINE}" STREQUAL "")
	
	STRING(REGEX MATCH ${HG_ARCHIVAL_VERSION_REGEX} TMP_NODE_LINE ${TMP_NODE_LINE})
	SET(MUHKUH_VERSION_VCS ${CMAKE_MATCH_1})
ELSE(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg")
	MESSAGE(WARNING "The code is no mercurial repository and no mercurial archive.")
	SET(MUHKUH_VERSION_VCS "unknown")
ENDIF(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg")


MESSAGE("Version info: ${MUHKUH_VERSION_MAJ}.${MUHKUH_VERSION_MIN}.${MUHKUH_VERSION_SUB}, VCS ID is ${MUHKUH_VERSION_VCS} .")


# Get the current year.
# There seems to be no built-in way in cmake to get the current year.
# But we have python for the MBS build system anyway so we can use it.
FIND_PACKAGE(PythonInterp REQUIRED)
EXECUTE_PROCESS(COMMAND "${PYTHON_EXECUTABLE}" -c "import datetime; print datetime.date.today().year"
                OUTPUT_VARIABLE MUHKUH_VERSION_YEAR
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)


