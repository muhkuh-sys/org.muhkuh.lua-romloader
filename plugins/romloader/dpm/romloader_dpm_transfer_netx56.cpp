#include "romloader_dpm_transfer_netx56.h"
#include <unistd.h>

#include "netx/targets/dpmmon_netx56_monitor.h"
#include "netx/targets/dpmmon_netx56_monitor_run.h"


#define NETX56_DPM_BOOT_NETX_RECEIVED_CMD      0x01
#define NETX56_DPM_BOOT_NETX_SEND_CMD          0x02

#define NETX56_DPM_BOOT_HOST_SEND_CMD          0x01
#define NETX56_DPM_BOOT_HOST_RECEIVED_CMD      0x02

#define SRT_NETX56_HANDSHAKE_REG_ARM_DATA      16
#define SRT_NETX56_HANDSHAKE_REG_PC_DATA       24



#define OFFS_HBOOT(reg) (offsetof(DPM_REGISTER_NETX56_T,tHboot)+offsetof(HBOOT_DPM_NETX56_T,reg))


romloader_dpm_transfer_netx56::romloader_dpm_transfer_netx56(romloader_dpm_device *ptDpmDevice)
 : romloader_dpm_transfer(ptDpmDevice)
{
}



romloader_dpm_transfer_netx56::~romloader_dpm_transfer_netx56(void)
{
}



int romloader_dpm_transfer_netx56::prepare_device(void)
{
	int iResult;
	NETX_DEVICE_STATE_T tState;


	/* Do we have a device? */
	if( m_ptDpmDevice==NULL )
	{
		iResult = -1;
	}
	else
	{
		/* Is the monitor already running? */
		tState = probe_netx_state();
		switch( tState )
		{
		case NETX_DEVICE_STATE_Unknown:
			/* The device is in an unknown state and can not be changed to something useful. */
			iResult = -1;
			break;

		case NETX_DEVICE_STATE_Romloader:
			/* No, the monitor is not running yet. Download the monitor. */
			iResult = download_and_run_image(auc_dpmmon_netx56_monitor, sizeof(auc_dpmmon_netx56_monitor));
			printf("download_and_run_image: %d\n", iResult);
			if( iResult==0 )
			{
				/* Give the software some time to start and setup the DPM. */
				usleep(500);

				/* Check the netX state again. */
				tState = probe_netx_state();
				if( tState!=NETX_DEVICE_STATE_Monitor )
				{
					/* Failed to start the monitor. */
					iResult = -1;
				}
				else
				{
					/* Now the monitor is running and ready for commands. */
					iResult = 0;
				}
			}
			break;

		case NETX_DEVICE_STATE_Monitor:
			/* The monitor is already running. */
			iResult = 0;
			break;
		}
	}

	return iResult;
}



romloader_dpm_transfer_netx56::NETX_DEVICE_STATE_T romloader_dpm_transfer_netx56::probe_netx_state(void)
{
	NETX_DEVICE_STATE_T tState;
	int iResult;
	uint32_t ulValue;


	/* Be pessimistic. */
	tState = NETX_DEVICE_STATE_Unknown;

	if( m_ptDpmDevice!=NULL )
	{
		/* Try to read the ROM boot ID. */
		iResult = m_ptDpmDevice->dpm_read32(offsetof(DPM_REGISTER_NETX56_T,tHboot)+offsetof(HBOOT_DPM_NETX56_T,ulDpmBootId), &ulValue);
		if( iResult==0 )
		{
			if( ulValue==NETX56_BOOT_ID_ROM )
			{
				/* The ROM code is running. */
				tState = NETX_DEVICE_STATE_Romloader;
			}
			else if( ulValue==BOOT_ID_MONITOR )
			{
				/* The ROM code is running. */
				tState = NETX_DEVICE_STATE_Monitor;
			}
		}
	}

	return tState;
}



int romloader_dpm_transfer_netx56::mailbox_purr(uint32_t ulMask, uint32_t *ulResult)
{
	int iResult;
	uint32_t ulValue;
	uint32_t ulHostPart;
	uint32_t ulNetxPart;


	iResult = m_ptDpmDevice->dpm_read32(OFFS_HBOOT(ulHandshake), &ulValue);
	if( iResult==0 )
	{
		ulHostPart  = ulValue >> SRT_NETX56_HANDSHAKE_REG_PC_DATA;
		ulHostPart &= 3;
		ulNetxPart  = ulValue >> SRT_NETX56_HANDSHAKE_REG_ARM_DATA;
		ulNetxPart &= 3;
		/* Check for harmony. */
		ulValue  = ulHostPart ^ ulNetxPart;
		/* Mask */
		ulValue &= ulMask;

		/* Return the result. */
		*ulResult = ulValue;
	}

	return iResult;
}


int romloader_dpm_transfer_netx56::mailbox_send_chunk(const uint8_t *pucChunk, uint32_t ulChunkSize)
{
	uint32_t ulValue;
	int iResult;


	/* Copy the chunk to the host->netX buffer. */
	iResult = m_ptDpmDevice->dpm_write_area(OFFS_HBOOT(aucHostToNetxData), pucChunk, ulChunkSize);
	if( iResult==0 )
	{
		/* Set the size of the chunk. */
		iResult = m_ptDpmDevice->dpm_write32(OFFS_HBOOT(ulHostToNetxDataSize), ulChunkSize);
		if( iResult==0 )
		{
			/* Toggle the handshake bit. */
			iResult = m_ptDpmDevice->dpm_read32(OFFS_HBOOT(ulHandshake), &ulValue);
			if( iResult==0 )
			{
				ulValue ^= NETX56_DPM_BOOT_HOST_SEND_CMD << SRT_NETX56_HANDSHAKE_REG_PC_DATA;
				iResult = m_ptDpmDevice->dpm_write32(OFFS_HBOOT(ulHandshake), ulValue);
				if( iResult==0 )
				{
					/* Wait until netX has processed the data. */
					do
					{
						usleep(500);
						iResult = mailbox_purr(NETX56_DPM_BOOT_NETX_RECEIVED_CMD, &ulValue);
						if( iResult!=0 )
						{
							break;
						}
					} while( ulValue!=0 );
				}
			}
		}
	}

	return iResult;
}



int romloader_dpm_transfer_netx56::mailbox_get_status(uint8_t *pucStatus)
{
	int iResult;
	uint32_t ulValue;
	uint32_t ulCnt;
	uint32_t ulRec;
	uint8_t ucData;
	unsigned int uiResult;


	/* Wait for data to receive. */
	do
	{
		usleep(500);
		iResult = mailbox_purr(NETX56_DPM_BOOT_NETX_SEND_CMD, &ulValue);
		if( iResult!=0 )
		{
			break;
		}
	} while( ulValue==0 );

	if( iResult==0 )
	{
		iResult = m_ptDpmDevice->dpm_read32(OFFS_HBOOT(ulNetxToHostDataSize), &ulRec);
		if( iResult==0 )
		{
			if( ulRec>NETX56_DPM_NETX_TO_HOST_BUFFERSIZE )
			{
				fprintf(stderr, "received invalid data size: 0x%08x\n", ulRec);
				iResult = -1;
			}
			else
			{
				/* Read the complete packet. */
				iResult = m_ptDpmDevice->dpm_read_area(OFFS_HBOOT(aucNetxToHostData), m_aucBufferNetxToHost, ulRec);
				if( iResult==0 )
				{
					/* Toggle the handshake bit. */
					iResult = m_ptDpmDevice->dpm_read32(OFFS_HBOOT(ulHandshake), &ulValue);
					if( iResult==0 )
					{
						ulValue ^= NETX56_DPM_BOOT_HOST_RECEIVED_CMD << SRT_NETX56_HANDSHAKE_REG_PC_DATA;
						iResult = m_ptDpmDevice->dpm_write32(OFFS_HBOOT(ulHandshake), ulValue);
						if( iResult==0 )
						{
							*pucStatus = m_aucBufferNetxToHost[0];
						}
					}
				}
			}
		}
	}

	return uiResult;
}



int romloader_dpm_transfer_netx56::download_and_run_image(const uint8_t *pucData, size_t sizData)
{
	int iResult;
	const uint8_t *pucCnt;
	const uint8_t *pucEnd;
	uint32_t ulChunkSize;
	uint8_t ucStatus;


	/* The image must have at least 68 bytes.
	 * This is 64 bytes for the header and at least 1 DWORD of data.
	 */
	if( sizData<68 )
	{
		printf("The image is too small!\n");
		iResult = -1;
	}
	else
	{
		pucCnt = pucData;
		pucEnd = pucData + sizData;

		/* Send boot block first. */
		ulChunkSize = 64;
		printf("send %d bytes boot block.\n", ulChunkSize);
		iResult = mailbox_send_chunk(pucCnt, ulChunkSize);
		if( iResult==0 )
		{
			pucCnt += ulChunkSize;

			printf("wait for status...\n");
			iResult = mailbox_get_status(&ucStatus);
			if( iResult==0 )
			{
				printf("status: %d\n", ucStatus);

				if( ucStatus==0 )
				{
					while( pucCnt<pucEnd )
					{
						ulChunkSize = pucEnd - pucCnt;
						if( ulChunkSize>NETX56_DPM_HOST_TO_NETX_BUFFERSIZE )
						{
							ulChunkSize = NETX56_DPM_HOST_TO_NETX_BUFFERSIZE;
						}

						printf("send %d bytes\n", ulChunkSize);
						iResult = mailbox_send_chunk(pucCnt, ulChunkSize);
						if( iResult!=0 )
						{
							break;
						}
						pucCnt += ulChunkSize;
					}

					printf("wait for status...\n");
					iResult = mailbox_get_status(&ucStatus);
					if( iResult==0 )
					{
						printf("status: %d\n", ucStatus);
					}
				}
			}
		}
	}

	return iResult;
}

