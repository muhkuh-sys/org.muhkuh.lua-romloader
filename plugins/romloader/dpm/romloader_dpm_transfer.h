#include <stddef.h>
#include <stdint.h>

#include "../romloader.h"
#include "romloader_dpm_device.h"


#ifndef __ROMLOADER_DPM_TRANSFER_H__
#define __ROMLOADER_DPM_TRANSFER_H__


/* The ID "MMON" shows that the monitor is running. */
#define BOOT_ID_MONITOR 0x4e4f4d4d


class romloader_dpm_transfer
{
public:
	romloader_dpm_transfer(romloader_dpm_device *ptDpmDevice);
	virtual ~romloader_dpm_transfer(void);

	virtual int prepare_device(void) = 0;

	virtual int send_command(const uint8_t *pucCommand, uint32_t ulCommandSize) = 0;
	virtual int receive_response(uint8_t *pucBuffer, uint32_t ulDataSize) = 0;

protected:
	romloader_dpm_device *m_ptDpmDevice;

private:
};

#endif  /* __ROMLOADER_DPM_TRANSFER_H__ */

