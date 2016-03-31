#include <stddef.h>
#include <stdint.h>

#ifndef __ROMLOADER_JTAG_OPENOCD_H__
#define __ROMLOADER_JTAG_OPENOCD_H__


typedef void * (*PFN_MUHKUH_OPENOCD_INIT_T) (void);
typedef int (*PFN_MUHKUH_OPENOCD_GET_RESULT_T) (void *pvContext, char *pcBuffer, size_t sizBufferMax);
typedef int (*PFN_MUHKUH_OPENOCD_COMMAND_RUN_LINE_T) (void *pvContext, const char *pcLine);
typedef void (*PFN_MUHKUH_OPENOCD_UNINIT_T) (void *pvContext);

typedef int (*PFN_MUHKUH_OPENOCD_READ_DATA08) (void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData);
typedef int (*PFN_MUHKUH_OPENOCD_READ_DATA16) (void *pvContext, uint32_t ulNetxAddress, uint16_t *pusData);
typedef int (*PFN_MUHKUH_OPENOCD_READ_DATA32) (void *pvContext, uint32_t ulNetxAddress, uint32_t *pulData);
typedef int (*PFN_MUHKUH_OPENOCD_READ_IMAGE) (void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData, uint32_t ulSize);
typedef int (*PFN_MUHKUH_OPENOCD_WRITE_DATA08) (void *pvContext, uint32_t ulNetxAddress, uint8_t ucData);
typedef int (*PFN_MUHKUH_OPENOCD_WRITE_DATA16) (void *pvContext, uint32_t ulNetxAddress, uint16_t usData);
typedef int (*PFN_MUHKUH_OPENOCD_WRITE_DATA32) (void *pvContext, uint32_t ulNetxAddress, uint32_t ulData);
typedef int (*PFN_MUHKUH_OPENOCD_WRITE_IMAGE) (void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData, uint32_t ulSize);


typedef struct ROMLOADER_JTAG_DEVICE_STRUCT
{
	void *pvSharedLibraryHandle;
	PFN_MUHKUH_OPENOCD_INIT_T pfnInit;
	PFN_MUHKUH_OPENOCD_GET_RESULT_T pfnGetResult;
	PFN_MUHKUH_OPENOCD_COMMAND_RUN_LINE_T pfnCommandRunLine;
	PFN_MUHKUH_OPENOCD_UNINIT_T pfnUninit;
	PFN_MUHKUH_OPENOCD_READ_DATA08 pfnReadData08;
	PFN_MUHKUH_OPENOCD_READ_DATA16 pfnReadData16;
	PFN_MUHKUH_OPENOCD_READ_DATA32 pfnReadData32;
	PFN_MUHKUH_OPENOCD_READ_IMAGE pfnReadImage;
	PFN_MUHKUH_OPENOCD_WRITE_DATA08 pfnWriteData08;
	PFN_MUHKUH_OPENOCD_WRITE_DATA16 pfnWriteData16;
	PFN_MUHKUH_OPENOCD_WRITE_DATA32 pfnWriteData32;
	PFN_MUHKUH_OPENOCD_WRITE_IMAGE pfnWriteImage;
	void *pvOpenocdContext;
} ROMLOADER_JTAG_DEVICE_T;


int romloader_jtag_openocd_init(void);

int romloader_jtag_openocd_detect(void);

void romloader_jtag_openocd_uninit(ROMLOADER_JTAG_DEVICE_T *ptJtagDevice);


#endif  /* __ROMLOADER_JTAG_OPENOCD_H__ */
