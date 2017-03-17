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
	uint32_t ulValue;


	/* Do we have a device? */
	if( m_ptDpmDevice==NULL )
	{
		iResult = -1;
	}
	else
	{
		/* Setup the DPM registers.
		 * FIXME: Do not run the setup for a serial DPM.
		 */

		/* Activate the ready signal generation.
		 * DPM is ready when external RDY-signal is high.
		 */
        ulValue  = MSK_NX56_dpm_rdy_cfg_rdy_pol;
        /* ready is driven when active and inactive. Never highZ. (Push-Pull mode) */
        ulValue |= 1 << SRT_NX56_dpm_rdy_cfg_rdy_drv_mode;
		iResult = m_ptDpmDevice->dpm_write32(offsetof(DPM_REGISTER_NETX56_T,tCtrl)+offsetof(NX56_DPM_AREA_T,ulDpm_rdy_cfg), ulValue);
		if( iResult==0 )
		{
			/* Disable setup times and filter. */
			ulValue = 0;
			iResult = m_ptDpmDevice->dpm_write32(offsetof(DPM_REGISTER_NETX56_T,tCtrl)+offsetof(NX56_DPM_AREA_T,ulDpm_timing_cfg), ulValue);
			if( iResult==0 )
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
					printf("Detected running ROM code. Downloading the monitor code...\n");
					iResult = download_and_run_image(auc_dpmmon_netx56_monitor, sizeof(auc_dpmmon_netx56_monitor));
					printf("download_and_run_image: %d\n", iResult);
					if( iResult==0 )
					{
						/* Give the software some time to start and setup the DPM. */
						/* FIXME: maybe this could become a polling loop. */
						usleep(500);
						printf("--OK was sleeping\n--");
						/* Check the netX state again. */
						tState = probe_netx_state();
						if( tState!=NETX_DEVICE_STATE_Monitor )
						{
							/* Failed to start the monitor. */
							printf("Failed to start the monitor, the ROM cookie is still there!\n");
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
		}
	}

	return iResult;
}



int romloader_dpm_transfer_netx56::send_command(const uint8_t *pucCommand, uint32_t ulCommandSize)
{
	int iResult;


	printf("##send_command haha %p\n", m_ptDpmDevice);
	fflush(stdout);
	if( m_ptDpmDevice==NULL )
	{
		iResult = -1;
	}
	else if( ulCommandSize>NETX56_DPM_HOST_TO_NETX_BUFFERSIZE )
	{
		printf("Died of buffer overflow.");
		/* Command too long error! */
		printf("The command (%d bytes) is too long for the buffer (%d bytes).\n", ulCommandSize, NETX56_DPM_HOST_TO_NETX_BUFFERSIZE);
		iResult = -1;
	}
	else
	{
		printf("before send chunk");
		iResult = mailbox_send_chunk(pucCommand, ulCommandSize);
		printf("after send chunk");

	}

	return iResult;
}



/* NOTE: this routine receives a data block with a known size. This is nice
 *       for responses of the read and write command where the response has a
 *       fixed size. Print messages are a different story. The host does not
 *       know the size of the packet. Use receive_packet in this case.
 */
int romloader_dpm_transfer_netx56::receive_response(uint8_t *pucBuffer, uint32_t ulDataSize)
{
	int iResult;
	uint32_t ulReceiveCnt;
	uint32_t ulBytesLeft;
	uint32_t ulChunkSize;


	/* Be optimistic. */
	iResult = 0;

	if( m_ptDpmDevice==NULL )
	{
		iResult = -1;
	}
	else
	{
		ulReceiveCnt = 0;
		while( ulReceiveCnt<ulDataSize )
		{
			ulBytesLeft = ulDataSize - ulReceiveCnt;
			iResult = mailbox_get_data(&ulChunkSize);
			if( iResult==0 )
			{
				if( ulChunkSize==0 )
				{
					printf("Received an empty packet!\n");
					iResult = -1;
				}
				else if( ulChunkSize>NETX56_DPM_NETX_TO_HOST_BUFFERSIZE )
				{
					printf("Received a packet with an invalid size!\n");
					iResult = -1;
				}
				else if( ulChunkSize>ulBytesLeft )
				{
					printf("Received more data than expected!\n");
					iResult = -1;
				}
				else
				{
					/* Received one more chunk. */
					memcpy(pucBuffer+ulReceiveCnt, m_aucBufferNetxToHost, ulChunkSize);
					ulReceiveCnt += ulChunkSize;
				}
			}
		}
	}

	return iResult;
}



/* NOTE: this routine receives one packet with a maximum data size. */
int romloader_dpm_transfer_netx56::receive_packet(uint8_t *pucBuffer, uint32_t ulMaxDataSize, uint32_t *pulDataRead)
{
	int iResult;
	uint32_t ulChunkSize = 0;


	/* Be pessimistic. */
	iResult = 0;

	if( m_ptDpmDevice==NULL )
	{
		iResult = -1;
	}
	else
	{
		iResult = mailbox_get_data(&ulChunkSize);
		if( iResult==0 )
		{
			if( ulChunkSize>NETX56_DPM_NETX_TO_HOST_BUFFERSIZE )
			{
				printf("Received a packet with an invalid size!\n");
				iResult = -1;
			}
			else if( ulChunkSize>ulMaxDataSize )
			{
				printf("Received more data than expected!\n");
				for(int i = 0; i < ulChunkSize; i++){
					printf("%i:%p, ", i, m_aucBufferNetxToHost);

				}
				iResult = -1;
			}
			else
			{
				/* Copy the data part - if requested. */
				if( pucBuffer!=NULL )
				{
					printf("Received right data size!\n");

					memcpy(pucBuffer, m_aucBufferNetxToHost, ulChunkSize);

				}
				/* Copy the data size - if requested. */
				if( pulDataRead!=NULL )
				{
					*pulDataRead = ulChunkSize;
				}
				iResult = 0;
			}
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
	unsigned int uiRetryCnt;
	int iResult;


	/* Copy the chunk to the host->netX buffer. */
	printf("1\n");
	iResult = m_ptDpmDevice->dpm_write_area(OFFS_HBOOT(aucHostToNetxData), pucChunk, ulChunkSize);
	if( iResult==0 )
	{
		printf("2\n");
		/* Set the size of the chunk. */
		iResult = m_ptDpmDevice->dpm_write32(OFFS_HBOOT(ulHostToNetxDataSize), ulChunkSize);
		if( iResult==0 )
		{
			printf("3\n");
			/* Toggle the handshake bit. */
			iResult = m_ptDpmDevice->dpm_read32(OFFS_HBOOT(ulHandshake), &ulValue);
			if( iResult==0 )
			{
				printf("4\n");
				ulValue ^= NETX56_DPM_BOOT_HOST_SEND_CMD << SRT_NETX56_HANDSHAKE_REG_PC_DATA;
				iResult = m_ptDpmDevice->dpm_write32(OFFS_HBOOT(ulHandshake), ulValue);
				if( iResult==0 )
				{
					/* Wait until netX has processed the data. */
					uiRetryCnt = 80000;
					do
					{
//						printf("5\n");
						iResult = mailbox_purr(NETX56_DPM_BOOT_NETX_RECEIVED_CMD, &ulValue);
						if( iResult!=0 )
						{
							break;
						}
						else if( ulValue!=0 )
						{
							--uiRetryCnt;
							if( uiRetryCnt==0 )
							{
								/* Timeout! */
								iResult = -1;
								break;
							}
							else
							{
								usleep(2500);
							}
						}
					} while( ulValue!=0 );
				}
			}
		}
	}
	return iResult;
}



int romloader_dpm_transfer_netx56::mailbox_get_data(uint32_t *pulSize)
{
	int iResult;
	uint32_t ulValue;
	uint32_t ulCnt;
	uint32_t ulRec;
	uint8_t ucData;
	unsigned int uiRetryCnt;


	/* Wait for data to receive. */
	uiRetryCnt = 80000000;
	do
	{
		iResult = mailbox_purr(NETX56_DPM_BOOT_NETX_SEND_CMD, &ulValue);
		if( iResult!=0 )
		{
			break;
		}
		else if( ulValue==0 )
		{
			--uiRetryCnt;
			if( uiRetryCnt==0 )
			{
				/* Timeout! */

				/* FAIL HERE */
				iResult = -1;
				break;
			}
			else
			{
				usleep(250);
			}
		}
	} while( ulValue==0 );
//	printf("Retries left: %d\n", uiRetryCnt);

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
							*pulSize = ulRec;
						}
					}
				}
			}
		}
	}

	return iResult;
}



int romloader_dpm_transfer_netx56::download_and_run_image(const uint8_t *pucData, size_t sizData)
{
	int iResult;
	const uint8_t *pucCnt;
	const uint8_t *pucEnd;
	uint32_t ulChunkSize;
	uint32_t ulResultSize;
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
			iResult = mailbox_get_data(&ulResultSize);
			if( iResult==0 )
			{
				if( ulResultSize!=1 )
				{
					printf("Received status packet with %d bytes, expected 1!\n", ulResultSize);
					iResult = -1;
				}
				else
				{
					ucStatus = m_aucBufferNetxToHost[0];
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
						iResult = mailbox_get_data(&ulResultSize);
						if( iResult==0 )
						{
							if( ulResultSize!=1 )
							{
								printf("Received status packet with %d bytes, expected 1!\n", ulResultSize);
								iResult = -1;
							}
							else
							{
								ucStatus = m_aucBufferNetxToHost[0];
								printf("status: %d\n", ucStatus);
								if( ucStatus!=0 )
								{
									iResult = -1;
								}
							}
						}
					}
				}
			}
		}
	}

	return iResult;
}


