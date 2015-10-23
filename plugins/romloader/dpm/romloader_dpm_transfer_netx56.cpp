#include "romloader_dpm_transfer_netx56.h"
#include <unistd.h>


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



int romloader_dpm_transfer_netx56::bitflip_boot(const uint8_t *pucData, size_t sizData)
{
	int iResult;
	uint32_t ulValue;


	/* Does the image fit into the bit-flip area? */
	if( sizData>sizeof(((DPM_REGISTER_NETX56_T*)(NULL))->aucBitflipImage) )
	{
		printf("The image exceeds the bitflip area!\n");
		iResult = -1;
	}
	else
	{
		/* Copy the boot image. */
		iResult = m_ptDpmDevice->dpm_write_area(offsetof(DPM_REGISTER_NETX56_T,aucBitflipImage), pucData, sizData);
		if( iResult==0 )
		{
			/* Flip the bit. */
			iResult = m_ptDpmDevice->dpm_read32(offsetof(DPM_REGISTER_NETX56_T,tCtrl)+offsetof(NX56_DPM_AREA_T,ulDpm_sys_sta), &ulValue);
			if( iResult==0 )
			{
				ulValue ^= 0x80;
				iResult = m_ptDpmDevice->dpm_write32(offsetof(DPM_REGISTER_NETX56_T,tCtrl)+offsetof(NX56_DPM_AREA_T,ulDpm_sys_sta), ulValue);
			}
		}
	}

	return iResult;
}



int romloader_dpm_transfer_netx56::mailbox_boot(const uint8_t *pucData, size_t sizData)
{
	int iResult;
	const uint8_t *pucCnt;
	const uint8_t *pucEnd;
	uint32_t ulChunkSize;
	uint8_t ucStatus;


	/* The image must have at least 68 bytes. */
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


