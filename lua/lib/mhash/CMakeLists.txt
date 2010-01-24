cmake_minimum_required(VERSION 2.8)

PROJECT("lua_lib_mhash")

SET(lua_lib_mhash_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)


INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/mhash-0.9.9-hilrom/lib)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/mhash-0.9.9-hilrom/include)


SET_SOURCE_FILES_PROPERTIES(mhash.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(mhash.i PROPERTIES SWIG_FLAGS "")


SET(lua_lib_mhash_SOURCES
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

SET(lua_lib_mhash_HEADERS
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

SWIG_ADD_MODULE(mhash lua mhash.i ${lua_lib_mhash_SOURCES})
SWIG_LINK_LIBRARIES(mhash ${LUA_LIBRARIES})
SET_TARGET_PROPERTIES(mhash PROPERTIES PREFIX "")
