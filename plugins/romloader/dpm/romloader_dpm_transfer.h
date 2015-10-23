#include <stddef.h>
#include <stdint.h>

#include "../romloader.h"
#include "romloader_dpm_device.h"


#ifndef __ROMLOADER_DPM_TRANSFER_H__
#define __ROMLOADER_DPM_TRANSFER_H__

class romloader_dpm_transfer
{
public:
	romloader_dpm_transfer(romloader_dpm_device *ptDpmDevice);
	virtual ~romloader_dpm_transfer(void);

	virtual int bitflip_boot(const uint8_t *pucData, size_t sizData) = 0;
	virtual int mailbox_boot(const uint8_t *pucData, size_t sizData) = 0;

protected:
	romloader_dpm_device *m_ptDpmDevice;

private:
};

#endif  /* __ROMLOADER_DPM_TRANSFER_H__ */

