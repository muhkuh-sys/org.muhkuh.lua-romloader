#include "romloader_dpm_transfer.h"
#include "romloader_dpm_memorylayout.h"


#ifndef __ROMLOADER_DPM_TRANSFER_NETX56_H__
#define __ROMLOADER_DPM_TRANSFER_NETX56_H__

class romloader_dpm_transfer_netx56 : public romloader_dpm_transfer
{
public:
	romloader_dpm_transfer_netx56(romloader_dpm_device *ptDpmDevice);
	virtual ~romloader_dpm_transfer_netx56(void);

	virtual int bitflip_boot(const uint8_t *pucData, size_t sizData);
	virtual int mailbox_boot(const uint8_t *pucData, size_t sizData);

protected:

private:
	int mailbox_purr(uint32_t ulMask, uint32_t *ulResult);
	int mailbox_send_chunk(const uint8_t *pucChunk, uint32_t ulChunkSize);
	int mailbox_get_status(uint8_t *pucStatus);

	uint8_t m_aucBufferNetxToHost[NETX56_DPM_NETX_TO_HOST_BUFFERSIZE];
	uint8_t m_aucBufferHostToNetx[NETX56_DPM_HOST_TO_NETX_BUFFERSIZE];
};

#endif  /* __ROMLOADER_DPM_TRANSFER_NETX56_H__ */

