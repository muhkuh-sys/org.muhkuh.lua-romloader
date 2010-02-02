#if !defined(__LIBDEFS_H)
#define LIBDEFS_H

/*
   {{{ includes 
 */

#define MUTILS_USE_MHASH_CONFIG

#include <mutils/mincludes.h>
#include <mutils/mglobal.h>
#include <mutils/mtypes.h>
#include <mutils/mutils.h>

#ifdef WIN32
/* NOTE: The define is used in the *.c files, and not in the headers.
         This is an error for MSVS2008. The solution with the fewest
		 changes is to keep the WIN32DLL_DEFINE empty.
		 This does not export any functions, but as the dll is wrapped
		 by Swig it does not matter. :)
*/
/* # define WIN32DLL_DEFINE __declspec( dllexport) */
# define WIN32DLL_DEFINE
#else
# define WIN32DLL_DEFINE
#endif

#define RAND32 (mutils_word32) ((mutils_word32)rand() << 17 ^ (mutils_word32)rand() << 9 ^ rand())

#endif
