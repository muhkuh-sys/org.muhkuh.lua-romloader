#include <stddef.h>

#ifndef __SHARED_LIBRARY_H__
#define __SHARED_LIBRARY_H__


void *sharedlib_open(const char *pcPath);
void sharedlib_close(void *pvSharedLibraryHandle);
void sharedlib_get_error(char *pcBuffer, size_t sizBuffer);
void *sharedlib_resolve_symbol(void *pvSharedLibraryHandle, const char *pcSymbol);


#endif  /* __SHARED_LIBRARY_H__ */
