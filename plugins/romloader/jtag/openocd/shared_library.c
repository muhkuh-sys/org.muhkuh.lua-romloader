#include <string.h>
#include <stdio.h>


#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#       include <windows.h>
#elif defined(__GNUC__)
	/* This is for Linux. */
#       include <dlfcn.h>
#endif



void *sharedlib_open(const char *pcPath)
{
	void *pvSharedLibraryHandle;


#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	pvSharedLibraryHandle = LoadLibrary(pcPath);
#elif defined(__GNUC__)
	pvSharedLibraryHandle = dlopen(pcPath, RTLD_NOW|RTLD_LOCAL);
#else
#       error "Add sharedlib_open for your platform."
#endif

	return pvSharedLibraryHandle;
}



void sharedlib_close(void *pvSharedLibraryHandle)
{
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	FreeLibrary((HANDLE)pvSharedLibraryHandle);
#elif defined(__GNUC__)
	dlclose(pvSharedLibraryHandle);
#else
#       error "Add sharedlib_close for your platform."
#endif
}



void sharedlib_get_error(char *pcBuffer, size_t sizBuffer)
{
	/* No buffer -> no errormessage. */
	if( pcBuffer!=NULL )
	{
		if( sizBuffer!=0 )
		{
			/* Clear the complete buffer with 0. */
			memset(pcBuffer, 0, sizBuffer);

			if( sizBuffer>0 )
			{
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
				FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), LANG_NEUTRAL, pcBuffer, sizBuffer-1, NULL);
#elif defined(__GNUC__)
				strncpy(pcBuffer, dlerror(), sizBuffer-1);
#else
#       error "Add sharedlib_get_error for your platform."
#endif
			}
		}
	}
}



void *sharedlib_resolve_symbol(void *pvSharedLibraryHandle, const char *pcSymbol)
{
	void *pvFn;
	char acError[1024];


#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	pvFn = GetProcAddress((HMODULE)pvSharedLibraryHandle, pcSymbol);
#elif defined(__GNUC__)
	pvFn = dlsym(pvSharedLibraryHandle, pcSymbol);
#else
#       error "Add sharedlib_resolve_symbol for your platform."
#endif

	if( pvFn==NULL )
	{
		sharedlib_get_error(acError, sizeof(acError));
		fprintf(stderr, "Failed to resolve function %s in shared library.\n", pcSymbol);
		fprintf(stderr, "Error: %s\n", acError);
	}

	return pvFn;
}

