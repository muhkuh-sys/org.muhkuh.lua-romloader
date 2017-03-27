#include "romloader_dpm_transfer.h"
#include "romloader_dpm_memorylayout.h"


#ifndef __ROMLOADER_DPM_TRANSFER_NETX4000_H__
#define __ROMLOADER_DPM_TRANSFER_NETX4000_H__

class romloader_dpm_transfer_netx4000 : public romloader_dpm_transfer
{
public:
	romloader_dpm_transfer_netx4000(romloader_dpm_device *ptDpmDevice);
	virtual ~romloader_dpm_transfer_netx4000(void);

	virtual int prepare_device(void);
	virtual int send_command(const uint8_t *pucCommand, uint32_t ulCommandSize);
	virtual int send_escape_command();
	virtual int receive_response(uint8_t *pucBuffer, uint32_t ulDataSize);
	virtual int receive_packet(uint8_t *pucBuffer, uint32_t ulMaxDataSize, uint32_t *pulDataRead);
	virtual int receive_escape_acknowledge_command();
	virtual int send_is_ready_to_execute();


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
	int mailbox_get_data(uint32_t *pulSize);

	uint8_t m_aucBufferNetxToHost[NETX4000_RELAXED_DPM_NETX_TO_HOST_BUFFERSIZE];
};

#endif  /* __ROMLOADER_DPM_TRANSFER_NETX4000_H__ */

