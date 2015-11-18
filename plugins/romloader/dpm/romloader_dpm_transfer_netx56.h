#include "romloader_dpm_transfer.h"
#include "romloader_dpm_memorylayout.h"


#ifndef __ROMLOADER_DPM_TRANSFER_NETX56_H__
#define __ROMLOADER_DPM_TRANSFER_NETX56_H__

class romloader_dpm_transfer_netx56 : public romloader_dpm_transfer
{
public:
	romloader_dpm_transfer_netx56(romloader_dpm_device *ptDpmDevice);
	virtual ~romloader_dpm_transfer_netx56(void);

	virtual int prepare_device(void);

protected:


private:
	typedef enum NETX_DEVICE_STATE_ENUM
	{
		NETX_DEVICE_STATE_Unknown = 0,   /* The netX device has an unknown state. This is not recoverable as all known ways have been tried already. */
		NETX_DEVICE_STATE_Romloader = 1, /* The ROM loader is running. */
		NETX_DEVICE_STATE_Monitor = 2    /* The monitor is already running. */
	} NETX_DEVICE_STATE_T;

	NETX_DEVICE_STATE_T probe_netx_state(void);

	int download_and_run_image(const uint8_t *pucData, size_t sizData);

	int mailbox_purr(uint32_t ulMask, uint32_t *ulResult);
	int mailbox_send_chunk(const uint8_t *pucChunk, uint32_t ulChunkSize);
	int mailbox_get_status(uint8_t *pucStatus);

	uint8_t m_aucBufferNetxToHost[NETX56_DPM_NETX_TO_HOST_BUFFERSIZE];
	uint8_t m_aucBufferHostToNetx[NETX56_DPM_HOST_TO_NETX_BUFFERSIZE];
};

#endif  /* __ROMLOADER_DPM_TRANSFER_NETX56_H__ */

