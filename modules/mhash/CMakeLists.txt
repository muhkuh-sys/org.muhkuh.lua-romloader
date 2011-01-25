cmake_minimum_required(VERSION 2.8)

project("module_mhash")

set(module_mhash_VERSION 0.0.1)


find_package(SWIG REQUIRED)


include(${SWIG_USE_FILE})

if(${PYTHONLIBS_FOUND})
	include_directories(${PYTHON_INCLUDE_DIRS})
endif(${PYTHONLIBS_FOUND})

if(${LUA51_FOUND})
	include_directories(${LUA_INCLUDE_DIR})
endif(${LUA51_FOUND})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mhash-0.9.9-hilrom/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mhash-0.9.9-hilrom/include)


set_source_files_properties(mhash.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(mhash.i PROPERTIES SWIG_FLAGS "-I${CMAKE_HOME_DIRECTORY}/modules/typemaps")


set(mhash_SOURCES
	mhash_state.cpp
	mhash-0.9.9-hilrom/lib/adler32.c
	mhash-0.9.9-hilrom/lib/crc16.c
	mhash-0.9.9-hilrom/lib/crc32.c
	mhash-0.9.9-hilrom/lib/gosthash.c
	mhash-0.9.9-hilrom/lib/haval.c
	mhash-0.9.9-hilrom/lib/hilrom.c
	mhash-0.9.9-hilrom/lib/keygen.c
	mhash-0.9.9-hilrom/lib/keygen_asis.c
	mhash-0.9.9-hilrom/lib/keygen_hex.c
	mhash-0.9.9-hilrom/lib/keygen_mcrypt.c
	mhash-0.9.9-hilrom/lib/keygen_s2k.c
	mhash-0.9.9-hilrom/lib/md2.c
	mhash-0.9.9-hilrom/lib/md4.c
	mhash-0.9.9-hilrom/lib/md5.c
	mhash-0.9.9-hilrom/lib/mhash.c
	mhash-0.9.9-hilrom/lib/ripemd.c
	mhash-0.9.9-hilrom/lib/sha1.c
	mhash-0.9.9-hilrom/lib/sha256.c
	mhash-0.9.9-hilrom/lib/sha256_sha224.c
	mhash-0.9.9-hilrom/lib/sha512_sha384.c
	mhash-0.9.9-hilrom/lib/snefru.c
	mhash-0.9.9-hilrom/lib/stdfns.c
	mhash-0.9.9-hilrom/lib/tiger.c
	mhash-0.9.9-hilrom/lib/tiger_sboxes.c
	mhash-0.9.9-hilrom/lib/whirlpool.c
)

set(mhash_HEADERS
	mhash_state.h
	mhash_config.h
	mhash-0.9.9-hilrom/include/mhash.h
	mhash-0.9.9-hilrom/include/mutils/mglobal.h
	mhash-0.9.9-hilrom/include/mutils/mhash.h
	mhash-0.9.9-hilrom/include/mutils/mincludes.h
	mhash-0.9.9-hilrom/include/mutils/mtypes.h
	mhash-0.9.9-hilrom/include/mutils/mutils.h
	mhash-0.9.9-hilrom/lib/keygen.h
	mhash-0.9.9-hilrom/lib/libdefs.h
	mhash-0.9.9-hilrom/lib/mhash_adler32.h
	mhash-0.9.9-hilrom/lib/mhash_crc16.h
	mhash-0.9.9-hilrom/lib/mhash_crc32.h
	mhash-0.9.9-hilrom/lib/mhash_gost.h
	mhash-0.9.9-hilrom/lib/mhash_haval.h
	mhash-0.9.9-hilrom/lib/mhash_hilrom.h
	mhash-0.9.9-hilrom/lib/mhash_int.h
	mhash-0.9.9-hilrom/lib/mhash_md2.h
	mhash-0.9.9-hilrom/lib/mhash_md4.h
	mhash-0.9.9-hilrom/lib/mhash_md5.h
	mhash-0.9.9-hilrom/lib/mhash_ripemd.h
	mhash-0.9.9-hilrom/lib/mhash_sha1.h
	mhash-0.9.9-hilrom/lib/mhash_sha256.h
	mhash-0.9.9-hilrom/lib/mhash_sha256_sha224.h
	mhash-0.9.9-hilrom/lib/mhash_sha512_sha384.h
	mhash-0.9.9-hilrom/lib/mhash_snefru.h
	mhash-0.9.9-hilrom/lib/mhash_tiger.h
	mhash-0.9.9-hilrom/lib/mhash_whirlpool.h
)

# Build the library.
add_library(mhash_lib STATIC ${mhash_SOURCES})


if(${LUA51_FOUND})
	# Build the Lua module.
	swig_add_module(mhash lua mhash.i)
	swig_link_libraries(mhash mhash_lib ${LUA_LIBRARIES})
	set_property(TARGET mhash PROPERTY PREFIX "")

	# install the lua module
	INSTALL(TARGETS mhash DESTINATION ${INSTALL_DIR_LUA_MODULES})
endif(${LUA51_FOUND})


if(${PYTHONLIBS_FOUND})
	# Build the Python module.
	swig_add_module(mhashmodule python mhash.i)
	swig_link_libraries(mhashmodule mhash_lib ${PYTHON_LIBRARIES})
	set_target_properties(_mhashmodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_PYTHON_OUTPUT_PATH})
endif(${PYTHONLIBS_FOUND})

