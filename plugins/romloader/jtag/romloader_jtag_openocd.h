#ifndef __ROMLOADER_JTAG_OPENOCD_H__
#define __ROMLOADER_JTAG_OPENOCD_H__


typedef struct ROMLOADER_JTAG_DEVICE_STRUCT
{
	void *pvOpenocdContext;
} ROMLOADER_JTAG_DEVICE_T;


int romloader_jtag_openocd_init(ROMLOADER_JTAG_DEVICE_T *ptJtagDevice);
void romloader_jtag_openocd_uninit(ROMLOADER_JTAG_DEVICE_T *ptJtagDevice);

#endif  /* __ROMLOADER_JTAG_OPENOCD_H__ */
