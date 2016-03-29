#include <stddef.h>

#ifndef __ROMLOADER_JTAG_OPENOCD_H__
#define __ROMLOADER_JTAG_OPENOCD_H__


typedef void * (*PFN_MUHKUH_OPENOCD_INIT_T) (void);
typedef int (*PFN_MUHKUH_OPENOCD_GET_RESULT_T) (void *pvContext, char *pcBuffer, size_t sizBufferMax);
typedef int (*PFN_MUHKUH_OPENOCD_COMMAND_RUN_LINE_T) (void *pvContext, const char *pcLine);
typedef void (*PFN_MUHKUH_OPENOCD_UNINIT_T) (void *pvContext);



typedef struct ROMLOADER_JTAG_DEVICE_STRUCT
{
	void *pvSharedLibraryHandle;
	PFN_MUHKUH_OPENOCD_INIT_T pfnInit;
	PFN_MUHKUH_OPENOCD_GET_RESULT_T pfnGetResult;
	PFN_MUHKUH_OPENOCD_COMMAND_RUN_LINE_T pfnCommandRunLine;
	PFN_MUHKUH_OPENOCD_UNINIT_T pfnUninit;
	void *pvOpenocdContext;
} ROMLOADER_JTAG_DEVICE_T;


int romloader_jtag_openocd_init(void);

int romloader_jtag_openocd_detect(void);

void romloader_jtag_openocd_uninit(ROMLOADER_JTAG_DEVICE_T *ptJtagDevice);


#endif  /* __ROMLOADER_JTAG_OPENOCD_H__ */
