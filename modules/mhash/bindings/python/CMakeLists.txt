CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT("module_mhash_python")

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})

# Build the Python module.
SET_SOURCE_FILES_PROPERTIES(../mhash.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(../mhash.i PROPERTIES SWIG_FLAGS "-I${CMAKE_HOME_DIRECTORY}/modules/typemaps")
SWIG_ADD_MODULE(module_mhash_python python mhash.i)
SWIG_LINK_LIBRARIES(module_mhash_python module_mhash_lib ${PYTHON_LIBRARIES})

# Rename the output file to "_mhash".
SET_PROPERTY(TARGET module_mhash_python PROPERTY PREFIX "")
SET_PROPERTY(TARGET module_mhash_python PROPERTY OUTPUT_NAME "_mhash")

# Install the python module and the import helper.
INSTALL(TARGETS module_mhash_python DESTINATION ${INSTALL_DIR_PYTHON_MODULES})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/mhash.py DESTINATION ${INSTALL_DIR_PYTHON_MODULES})
