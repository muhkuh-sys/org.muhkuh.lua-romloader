/***************************************************************************
 *   Copyright (C) 2015 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <ctype.h>
#include <stdio.h>

#include "romloader_dpm_main.h"
#include "romloader_dpm_device_linux_uio.h"
#include "romloader_dpm_transfer_netx56.h"

/*-------------------------------------*/

const char *romloader_dpm_provider::m_pcPluginNamePattern = "romloader_dpm_%s";

romloader_dpm_provider::romloader_dpm_provider(swig_type_info *p_romloader_dpm,
		swig_type_info *p_romloader_dpm_reference) :
		muhkuh_plugin_provider("romloader_dpm") {
	printf("%s(%p): provider create\n", m_pcPluginId, this);

	/* Get the romloader_dpm LUA type. */
	m_ptPluginTypeInfo = p_romloader_dpm;
	m_ptReferenceTypeInfo = p_romloader_dpm_reference;
}

romloader_dpm_provider::~romloader_dpm_provider(void) {
	printf("%s(%p): provider delete\n", m_pcPluginId, this);
}

int romloader_dpm_provider::DetectInterfaces(
		lua_State *ptLuaStateForTableAccess) {
	size_t sizDeviceNames;
	char **ppcDeviceNames;
	char **ppcDeviceNamesCnt;
	char **ppcDeviceNamesEnd;
	romloader_dpm_reference *ptReference;
	bool fDeviceIsBusy;

	/* Detect all interfaces. */
	sizDeviceNames = romloader_dpm_device_linux_uio::ScanForDevices(
			&ppcDeviceNames);

	if (ppcDeviceNames != NULL) {
		/* Add all detected devices to the table. */
		ppcDeviceNamesCnt = ppcDeviceNames;
		ppcDeviceNamesEnd = ppcDeviceNames + sizDeviceNames;
		while (ppcDeviceNamesCnt < ppcDeviceNamesEnd) {
			/* DPM devices are never busy. */
			fDeviceIsBusy = false;

			/* Create the new instance. */
			ptReference = new romloader_dpm_reference(*ppcDeviceNamesCnt,
					m_pcPluginId, fDeviceIsBusy, this);
			add_reference_to_table(ptLuaStateForTableAccess, ptReference);

			/* Free the name. */
			free(*ppcDeviceNamesCnt);

			/* Next device. */
			++ppcDeviceNamesCnt;
		}

		/* Free the list. */
		free(ppcDeviceNames);
	}

	return sizDeviceNames;
}

romloader_dpm *romloader_dpm_provider::ClaimInterface(
		const muhkuh_plugin_reference *ptReference) {
	romloader_dpm *ptPlugin;
	const char *pcName;
	char acDevice[PATH_MAX];

	/* expect error */
	ptPlugin = NULL;

	if (ptReference == NULL) {
		fprintf(stderr, "%s(%p): claim_interface(): missing reference!\n",
				m_pcPluginId, this);
	} else {
		pcName = ptReference->GetName();
		if (pcName == NULL) {
			fprintf(stderr, "%s(%p): claim_interface(): missing name!\n",
					m_pcPluginId, this);
		}
		/* NOTE: this is just a general test if the device is some kind
		 *       of DPM thing. A more closer look is done in the
		 *       romloader_dpm class.
		 */
		else if (sscanf(pcName, m_pcPluginNamePattern, acDevice) != 1) {
			fprintf(stderr, "%s(%p): claim_interface(): invalid name: %s\n",
					m_pcPluginId, this, pcName);
		} else {
			/* NOTE: pass the complete name (i.e. pass "pcName" and
			 *       not "acDevice". It is used to distinguish the
			 *       transport.
			 */
			ptPlugin = new romloader_dpm(pcName, m_pcPluginId, this, pcName);
			printf("%s(%p): claim_interface(): claimed interface %s.\n",
					m_pcPluginId, this, pcName);
		}
	}

	return ptPlugin;
}

/*
 Increment the sequence number
 Call when a packet has been received from the netX (the sequence number is incremented on the netX side, too).
 - after synchronize()
 - after receiving a response packet
 Do not increment if the packet send/receive operation failed or no response was received.
 If the netX did not receive the packet, it is still assuming the current sequence number.
 */
void romloader_dpm::next_sequence_number() {
	printf("\n INCREMENT SEQUENCE NUMBER: %i", m_uiMonitorSequence);
	m_uiMonitorSequence = (m_uiMonitorSequence + 1)
			& (MONITOR_SEQUENCE_MSK >> MONITOR_SEQUENCE_SRT);
}

/**
 * @return response or -1 in case of error
 * @todo error code for different cases?
 */
int romloader_dpm::execute_command(uint8_t aucCommand[],
		size_t sizAucCommandXXX, uint8_t ** aucResponse,
		uint32_t * sizAucResponse) {

	uint8_t aucLocalResponse[*sizAucResponse + 1]; // assume that we getting exactly what we asked for (+ 2 byte haeder)

	uint32_t ulPacketSize;

	int iResult = -1;

	printf("executing command...\n");
	printf("requested adr: 0x%02x%02x%02x%02x\n", aucCommand[6], aucCommand[5],
			aucCommand[4], aucCommand[3]);
	iResult = m_ptTransfer->send_command(aucCommand, sizAucCommandXXX);

	printf("send done: %d\n", iResult);
	// break if error to avoid crashing in receive
	if (iResult != 0) {
		return iResult;
	}
	printf("Receive_packet. \n");
	iResult = m_ptTransfer->receive_packet(aucLocalResponse,
			sizeof(aucLocalResponse), &ulPacketSize);
	// break if error to avoid copying crap
	if (iResult != 0) {
		return iResult;
	}

	if (*aucResponse == NULL) {
		*aucResponse = (uint8_t*) malloc(ulPacketSize * sizeof(uint8_t));
	}

	memcpy(*aucResponse, &aucLocalResponse, ulPacketSize);
	*sizAucResponse = ulPacketSize;
	printf("receive finished: %d\n", iResult);
	printf("ERG: 0x%x\n", *(*aucResponse + 1));
	return iResult;
}

bool romloader_dpm_provider::ReleaseInterface(muhkuh_plugin *ptPlugin) {
	bool fOk;
	const char *pcName;
	char acDevice[PATH_MAX];

	/* expect error */
	fOk = false;

	if (ptPlugin == NULL) {
		fprintf(stderr, "%s(%p): release_interface(): missing plugin!\n",
				m_pcPluginId, this);
	} else {
		pcName = ptPlugin->GetName();
		if (pcName == NULL) {
			fprintf(stderr, "%s(%p): release_interface(): missing name!\n",
					m_pcPluginId, this);
		} else if (sscanf(pcName, m_pcPluginNamePattern, acDevice) != 1) {
			fprintf(stderr, "%s(%p): release_interface(): invalid name: %s\n",
					m_pcPluginId, this, pcName);
		} else {
			printf("%s(%p): released interface %s.\n", m_pcPluginId, this,
					pcName);
			fOk = true;
		}
	}

	return fOk;
}

/*-------------------------------------*/

romloader_dpm::romloader_dpm(const char *pcName, const char *pcTyp,
		romloader_dpm_provider *ptProvider, const char *pcDeviceName) :
		romloader(pcName, pcTyp, ptProvider), m_pcDeviceName(NULL), m_ptTransfer(
				NULL) {
	printf("%s(%p): created in romloader_dpm\n", m_pcName, this);

	m_pcDeviceName = strdup(pcDeviceName);
}

romloader_dpm::~romloader_dpm(void) {
	printf("%s(%p): deleted in romloader_dpm\n", m_pcName, this);

	if (m_pcDeviceName != NULL) {
		free(m_pcDeviceName);
	}

	if (m_ptTransfer != NULL) {
		delete m_ptTransfer;
	}
}

void romloader_dpm::Connect(lua_State *ptClientData) {
	int iResult;
	romloader_dpm_device *ptDevice;
	ROMLOADER_CHIPTYP tChipTyp;
	romloader_dpm_transfer *ptTransfer;

	/* No device created yet. */
	ptDevice = NULL;
	/* No transfer object created yet. */
	ptTransfer = NULL;

	printf("%s(%p): connect\n", m_pcName, this);

	if (m_fIsConnected == false) {
		/* Is this a UIO device? */
		iResult = romloader_dpm_device_linux_uio::IsMyDeviceName(
				m_pcDeviceName);
		if (iResult == 0) {
			/* Yes, this is a UIO device. */
			ptDevice = new romloader_dpm_device_linux_uio(m_pcDeviceName);
		}
		/* TODO: Add more devices like the FTDI here. */

		/* Was a device created? */
		if (ptDevice != NULL) {
			/* Yes, we have a device. */

			/* Try to open the device. */
			iResult = ptDevice->Open();
			if (iResult == 0) {
				/* Create a new transfer device. This depends on the chip type. */
				ptTransfer = NULL;
				tChipTyp = ptDevice->get_chiptyp();
				switch (tChipTyp) {
				case ROMLOADER_CHIPTYP_UNKNOWN:
					break;

				case ROMLOADER_CHIPTYP_NETX500:
				case ROMLOADER_CHIPTYP_NETX100:
					/* Not yet! */
					printf("netX500/100 is not supported yet.\n");
					break;

				case ROMLOADER_CHIPTYP_NETX50:
					/* Not yet! */
					printf("netX50 is not supported yet.\n");
					break;

				case ROMLOADER_CHIPTYP_NETX5:
					/* Not yet! */
					printf("netX5 is not supported yet.\n");
					break;

				case ROMLOADER_CHIPTYP_NETX10:
					/* Not yet! */
					printf("netX10 is not supported yet.\n");
					break;

				case ROMLOADER_CHIPTYP_NETX56:
				case ROMLOADER_CHIPTYP_NETX56B:
					/* Create a new transfer object. */
					ptTransfer = new romloader_dpm_transfer_netx56(ptDevice);
					printf("ptTransfer %p \n", ptTransfer);

					/* The device is now owned by the transfer object. */
					ptDevice = NULL;
					break;

				case ROMLOADER_CHIPTYP_NETX4000_RELAXED:
				case ROMLOADER_CHIPTYP_NETX4000_FULL_SMALL:
					/* Not yet! */
					printf("netX4000 is not supported yet.\n");
					break;

				case ROMLOADER_CHIPTYP_NETX90_MPW:
					/* Not yet! */
					printf("netX90 is not supported yet.\n");
					break;
				}

				/* Do we have a transfer object now? */
				if (ptTransfer != NULL) {
					/* Prepare the device so that a monitor is running. */
					iResult = ptTransfer->prepare_device();
					if (iResult != 0) {
						/* Failed to prepare the device. It can not be used. */
						delete ptTransfer;
						ptTransfer = NULL;
						printf("ptTransfer %p \n", ptTransfer);

					}
				}
			}
		}

		/* If a device is left here, this is an error. It should be owned by a transfer object. Delete it. */
		if (ptDevice != NULL) {
			delete ptDevice;
		}

		/* Do we have a transfer object? */
		if (ptTransfer != NULL) {
			/* Get the magic packet. */
			iResult = synchronize(ptTransfer);
			printf("ptTransfer %p \n", ptTransfer);

			printf("iResult: %i", iResult);
			if (iResult != 0) {
				/* Failed to synchronize with the netX. */
				delete ptTransfer;
				ptTransfer = NULL;
				printf("ptTransfer %p \n", ptTransfer);

			} else {
				m_fIsConnected = true;
				/* Delete any previous transfer object. */
				if (m_ptTransfer != NULL) {
					delete m_ptTransfer;

				}

				/* Use the current transfer. */
				m_ptTransfer = ptTransfer;
				printf("ptTransfer %p \n", ptTransfer);
				printf("set new pt transfer object");
			}
		} else {
			m_fIsConnected = false;
			MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
		}
	}
}

void romloader_dpm::Disconnect(lua_State *ptClientData) {
	printf("%s(%p): disconnect\n", m_pcName, this);

	if (m_ptTransfer != NULL) {
		delete m_ptTransfer;
		m_ptTransfer = NULL;
	}

	m_fIsConnected = false;
}

int romloader_dpm::synchronize(romloader_dpm_transfer *ptTransfer) {
	const uint8_t aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	int iResult;
	uint32_t ulPacketSize;
	unsigned int uiCnt;
	uint8_t aucCommand[1];
	uint8_t aucResponse[32];
	uint8_t ucData;
	uint8_t aucInBuf[64];
	size_t sizInBuf;

	const size_t sizExpectedResponse = 12;
	uint8_t ucSequence;
	uint32_t ulMiVersionMin;
	uint32_t ulMiVersionMaj;
	ROMLOADER_CHIPTYP tChipType;
	size_t sizMaxPacketSize;

	aucCommand[0] = MONITOR_COMMAND_Magic;
	iResult = ptTransfer->send_command(aucCommand, sizeof(aucCommand));
	if (iResult != 0) {
		fprintf(stderr, "Failed to send the sync packet: %d\n", iResult);
	} else {
		printf("Receive_packet. \n");
		iResult = ptTransfer->receive_packet(aucResponse, sizeof(aucResponse),
				&ulPacketSize);
		if (iResult != 0) {
			fprintf(stderr,
					"Failed to receive the response of the SYNC packet: %d\n",
					iResult);
		} else if (ulPacketSize == 0) {
			fprintf(stderr, "%s(%p): synchronize: received empty answer!\n",
					m_pcName, this);

		} else if (memcmp(aucMagicMooh, aucResponse + 1, sizeof(aucMagicMooh))
				!= 0) {
			fprintf(stderr, "Received knock sequence has no magic.\n");
			for (uiCnt = 0; uiCnt < ulPacketSize; ++uiCnt) {
				printf(" 0x%x", aucResponse[uiCnt]);
			}
		} else {
			/* Dump the response. */
			printf("Got %d bytes\n", ulPacketSize);
			for (uiCnt = 0; uiCnt < ulPacketSize; ++uiCnt) {
				printf(" 0x%x", aucResponse[uiCnt]);
			}
			printf("\n");

			/* Get the sequence number from the status byte. */
			ucSequence = (aucResponse[0x00] & MONITOR_SEQUENCE_MSK)
					>> MONITOR_SEQUENCE_SRT;
			fprintf(stderr, "Sequence number: 0x%02x\n", ucSequence);

			ulMiVersionMin = ((uint32_t)(aucResponse[0x05]))
					| (((uint32_t)(aucResponse[0x06])) << 8U);
			ulMiVersionMaj = ((uint32_t)(aucResponse[0x07]))
					| (((uint32_t)(aucResponse[0x08])) << 8U);
			printf("Machine interface V%d.%d.\n", ulMiVersionMaj,
					ulMiVersionMin);

			tChipType = (ROMLOADER_CHIPTYP) (aucResponse[0x09]);
			printf("Chip type : %d\n", tChipType);

			sizMaxPacketSize = ((size_t)(aucResponse[0x0a]))
					| (((size_t)(aucResponse[0x0b])) << 8U);
			printf("Maximum packet size: 0x%04lx\n", sizMaxPacketSize);
			/* Limit the packet size to the buffer size. */
			if (sizMaxPacketSize > m_sizMaxPacketSizeHost) {
				sizMaxPacketSize = m_sizMaxPacketSizeHost;
				printf("Limit maximum packet size to 0x%04lx\n",
						sizMaxPacketSize);
			}

			/* Set the new values. */
			m_uiMonitorSequence = ucSequence;
			m_tChiptyp = tChipType;
			m_sizMaxPacketSizeClient = sizMaxPacketSize;

			next_sequence_number();
		}
	}
	/* Send a packet. */
	return iResult;
}

uint8_t romloader_dpm::read_data08(lua_State *ptClientData,
		uint32_t ulNetxAddress) {
	uint8_t aucCommand[7];
	uint8_t ucValue;
	uint8_t * aucResponse;
	uint32_t * sizAucResponse;
	uint32_t ulPacketSize;
	int iResult;

	/* Expect error. */
	iResult = -1;
	ucValue = 0;
	aucResponse = NULL;
	sizAucResponse = NULL;

	if (m_fIsConnected == false) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!",
				m_pcName, this);
	} else if (m_ptTransfer == NULL) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): no transfer object!",
				m_pcName, this);
	} else {
		aucCommand[0] = MONITOR_COMMAND_Read
				| (MONITOR_ACCESSSIZE_Byte << MONITOR_ACCESSSIZE_SRT)
				| (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 1;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)(ulNetxAddress & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress >> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress >> 16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress >> 24U) & 0xffU);

		sizAucResponse = (uint32_t*) malloc(sizeof(uint32_t));
		*sizAucResponse = 1 << MONITOR_ACCESSSIZE_Byte; // give a hint how many space is needed.

		iResult = execute_command(aucCommand, sizeof(aucCommand), &aucResponse,
				sizAucResponse);

		// @todo use return for dynamically compute ucValue
		ucValue = (uint32_t)(aucResponse[1]);
		free(sizAucResponse);
		free(aucResponse);
		aucResponse = NULL;

		next_sequence_number();
	}

	if (iResult != 0) {
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucValue;
}

uint16_t romloader_dpm::read_data16(lua_State *ptClientData,
		uint32_t ulNetxAddress) {
	uint8_t aucCommand[7];
	uint16_t usValue;
	uint8_t * aucResponse;
	uint32_t * sizAucResponse;

	bool fOk;
	uint32_t ulPacketSize;
	int iResult;

	/* Expect error. */
	iResult = -1;
	usValue = 0;
	aucResponse = NULL;
	sizAucResponse = NULL;

	if (m_fIsConnected == false) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!",
				m_pcName, this);
	} else if (m_ptTransfer == NULL) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): no transfer object!",
				m_pcName, this);
	} else {
		aucResponse = NULL;
		aucCommand[0] = MONITOR_COMMAND_Read
				| (MONITOR_ACCESSSIZE_Word << MONITOR_ACCESSSIZE_SRT)
				| (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 2;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)(ulNetxAddress & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress >> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress >> 16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress >> 24U) & 0xffU);

		sizAucResponse = (uint32_t*) malloc(sizeof(uint32_t));
		*sizAucResponse = 1 << MONITOR_ACCESSSIZE_Word; // give a hint how many space is needed.

		iResult = execute_command(aucCommand, sizeof(aucCommand), &aucResponse,
				sizAucResponse);

		// @todo use return for dynamically compute ucValue
		usValue = ((uint32_t)(aucResponse[1]))
				| ((uint32_t)(aucResponse[2])) << 8U;
		free(aucResponse);
		free(sizAucResponse);
		aucResponse = NULL;

		next_sequence_number();
	}

	if (iResult != 0) {
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usValue;
}

uint32_t romloader_dpm::read_data32(lua_State *ptClientData,
		uint32_t ulNetxAddress) {
	uint8_t aucCommand[7];
	uint8_t * aucResponse;
	uint32_t * sizAucResponse;

	uint32_t ulValue;
	unsigned char ucStatus;
	uint32_t ulPacketSize;
	int iResult;

	/* Expect error. */
	iResult = -1;
	ulValue = 0;
	aucResponse = NULL;
	sizAucResponse = NULL;

	ulValue = 0;
	printf("m_fIsConnected: %i", m_fIsConnected);

	if (m_fIsConnected == false) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!",
				m_pcName, this);
	} else if (m_ptTransfer == NULL) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): no transfer object!",
				m_pcName, this);
	} else {
		aucCommand[0] = MONITOR_COMMAND_Read
				| (MONITOR_ACCESSSIZE_Long << MONITOR_ACCESSSIZE_SRT)
				| (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 4;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)(ulNetxAddress & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress >> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress >> 16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress >> 24U) & 0xffU);

		sizAucResponse = (uint32_t*) malloc(sizeof(uint32_t));
		*sizAucResponse = 1 << MONITOR_ACCESSSIZE_Long; // give a hint how many space is needed.
		iResult = execute_command(aucCommand, sizeof(aucCommand), &aucResponse,
				sizAucResponse);

		ulValue = ((uint32_t)(aucResponse[1]))
				| ((uint32_t)(aucResponse[2])) << 8U
				| ((uint32_t)(aucResponse[3])) << 16U
				| ((uint32_t)(aucResponse[4])) << 24U;
		free(sizAucResponse);
		free(aucResponse);
		next_sequence_number();
	}

	if (iResult != 0) {
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulValue;
}

#define tmp 0

void romloader_dpm::read_image(uint32_t ulNetxAddress, uint32_t ulSize,
		char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn,
		long lCallbackUserData) {
	/* lua return param example */
//	*ppcBUFFER_OUT = (char*) malloc(10 * sizeof(char));
//	**ppcBUFFER_OUT = 'x';
//	*psizBUFFER_OUT = ulSize;
	char *pcBufferStart;
	char *pcBuffer;
	size_t sizBuffer;
	bool fOk;
	size_t sizChunk;

#if tmp == 1
	uint8_t aucResponse[m_sizMaxPacketSizeHost];
	uint32_t ulPacketSize;
	uint8_t aucCommand[m_sizMaxPacketSizeHost];

#else

	uint8_t * aucResponse;
	uint32_t * sizAucResponse;

	uint32_t ulPacketSize;

	uint8_t aucCommand[7]; // from m_sizMaxPacketSizeClient --> doesn't make any sense?

	aucResponse = NULL;

#endif

	long lBytesProcessed;
	uint32_t ulValue;

	int iResult;

	/* Be optimistic. */
	fOk = true;

	pcBufferStart = NULL;
	sizBuffer = 0;

	if (m_fIsConnected == false) {
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName,
				this);
		fOk = false;
	} else if (m_ptTransfer == NULL) {
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): no transfer object!",
				m_pcName, this);
	}
	/* if ulSize == 0, we return with fOk == true, pcBufferStart == NULL and sizBuffer == 0 */
	else if (ulSize > 0) {

		pcBufferStart = (char*) malloc(ulSize);
		if (pcBufferStart == NULL) {
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
					"%s(%p): failed to allocate %d bytes!", m_pcName, this,
					ulSize);
			fOk = false;
		} else {
			sizBuffer = ulSize;

			pcBuffer = pcBufferStart;
			lBytesProcessed = 0;
			do {
				sizChunk = ulSize;
				if (sizChunk > m_sizMaxPacketSizeClient - 6) {
					sizChunk = m_sizMaxPacketSizeClient - 6;
				}

				aucCommand[0] = MONITOR_COMMAND_Read
						| (MONITOR_ACCESSSIZE_Byte << MONITOR_ACCESSSIZE_SRT)
						| (uint8_t)(
								m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
				aucCommand[1] = (uint8_t)(sizChunk & 0xffU);
				aucCommand[2] = (uint8_t)((sizChunk >> 8U) & 0xffU);
				aucCommand[3] = (uint8_t)(ulNetxAddress & 0xffU);
				aucCommand[4] = (uint8_t)((ulNetxAddress >> 8U) & 0xffU);
				aucCommand[5] = (uint8_t)((ulNetxAddress >> 16U) & 0xffU);
				aucCommand[6] = (uint8_t)((ulNetxAddress >> 24U) & 0xffU);
#if tmp == 1
				printf("sending command...\n");
				printf("requested adr: 0x%02x%02x%02x%02x\n", aucCommand[6], aucCommand[5], aucCommand[4], aucCommand[3]);
				iResult = m_ptTransfer->send_command(aucCommand, 7);

				printf("send done: %d\n", iResult);
				if (iResult != 0)
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L/*ptClientData*/,"%s(%p): failed to execute command!", m_pcName, this);

					//							MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
				}
				else
				{
					printf("Receive_packet. \n");
					iResult = m_ptTransfer->receive_packet(aucResponse, sizeof(aucResponse), &ulPacketSize);

					printf("receive finished: %d\n", iResult);
					if (iResult != 0)
					{
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): answer to read_data32 has wrong packet size!", m_pcName, this);
					}
					else
					{
						ulValue = ((uint32_t)(aucResponse[2])) | ((uint32_t)(aucResponse[3])) << 8U | ((uint32_t)(aucResponse[4])) << 16U | ((uint32_t)(aucResponse[5])) << 24U;
						fOk = true;
					}
				}
#else
				sizAucResponse = (uint32_t*) malloc(sizeof(uint32_t));
				*sizAucResponse = ulSize; // give a hint how much space is needed.
				iResult = execute_command(aucCommand, sizeof(aucCommand),
						&aucResponse, sizAucResponse);
#endif
				if (iResult != 0) {
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
							"%s(%p): failed to execute command!", m_pcName,
							this);
					fOk = false;
					break;
				} else {
					ulValue = ((uint32_t)(aucResponse[1]))
							| ((uint32_t)(aucResponse[2])) << 8U
							| ((uint32_t)(aucResponse[3])) << 16U
							| ((uint32_t)(aucResponse[4])) << 24U;
					fOk = true;
				}

				if (*sizAucResponse != 1 + sizChunk) {
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
							"%s(%p): answer to read_image has wrong packet size of %d!",
							m_pcName, this, ulPacketSize);
					fOk = false;
					break;
				} else {
					memcpy(pcBuffer, aucResponse + 1, sizChunk);
					pcBuffer += sizChunk;
					ulSize -= sizChunk;
					ulNetxAddress += sizChunk;
					lBytesProcessed += sizChunk;

					free(aucResponse);
					free(sizAucResponse);

				}
			} while (ulSize != 0);
		}
		next_sequence_number();
	}

	if (fOk == true) {
		*ppcBUFFER_OUT = pcBufferStart;
		*psizBUFFER_OUT = sizBuffer;
	} else {
		if (pcBufferStart != NULL)
			free(pcBufferStart);
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}

void romloader_dpm::write_data08(lua_State *ptClientData,
		uint32_t ulNetxAddress, uint8_t ucData) {
	uint8_t aucCommand[8];
	uint32_t ulValue;
	uint32_t ulPacketSize;

	int iResult;

	ulValue = 0;

#if tmp == 0
	uint8_t * aucResponse;
	uint32_t * sizAucResponse;
	aucResponse = NULL;

#endif

	if (m_fIsConnected == false) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!",
				m_pcName, this);
	} else if (m_ptTransfer == NULL) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): no transfer object!",
				m_pcName, this);
	} else {
		printf("\n####Writing 0x%x to adress 0x%x####", ucData, ulNetxAddress);
		aucCommand[0] = MONITOR_COMMAND_Write
				| (MONITOR_ACCESSSIZE_Byte << MONITOR_ACCESSSIZE_SRT)
				| (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 1;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)(ulNetxAddress & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress >> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress >> 16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress >> 24U) & 0xffU);
		aucCommand[7] = ucData;

//		tResult = execute_command(aucCommand, 11);
#if tmp == 1
		uint8_t aucResponse[16];
		unsigned char ucStatus;

		iResult = m_ptTransfer->send_command(aucCommand, 8);

		if (iResult != 0)
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
		}
		else
		{
//			if( m_sizMaxPacketSizeClient!=4+1 ) /* not sure with this */
//			{
//				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to write_data32 has wrong packet size of %d!", m_pcName, this, m_sizMaxPacketSizeClient);
//			}
//			else
//			{
			fOk = true;
//			}
		}
		printf("Receive_packet. \n");
		iResult = m_ptTransfer->receive_packet(aucResponse, sizeof(aucResponse), &ulPacketSize);

#else
		sizAucResponse = (uint32_t*) malloc(sizeof(uint32_t));
		*sizAucResponse = sizeof(ucData); // give a hint how many space is needed.
		iResult = execute_command(aucCommand, sizeof(aucCommand), &aucResponse,
				sizAucResponse);
#endif
	}
	next_sequence_number();

	if (iResult != 0) {
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}

void romloader_dpm::write_data16(lua_State *ptClientData,
		uint32_t ulNetxAddress, uint16_t usData) {
	uint8_t aucCommand[9];
	uint32_t ulValue;

	uint32_t ulPacketSize;

	int iResult;

#if tmp == 0
	uint8_t * aucResponse;
	uint32_t * sizAucResponse;
	aucResponse = NULL;
#endif

	ulValue = 0;

	if (m_fIsConnected == false) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!",
				m_pcName, this);
	} else if (m_ptTransfer == NULL) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): no transfer object!",
				m_pcName, this);
	} else {
		printf("\n####Writing 0x%x to adress 0x%x####", usData, ulNetxAddress);
		aucCommand[0] = MONITOR_COMMAND_Write
				| (MONITOR_ACCESSSIZE_Word << MONITOR_ACCESSSIZE_SRT)
				| (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 2;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)(ulNetxAddress & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress >> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress >> 16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress >> 24U) & 0xffU);
		aucCommand[7] = (uint8_t)(usData & 0xffU);
		aucCommand[8] = (uint8_t)((usData >> 8U) & 0xffU);

//		tResult = execute_command(aucCommand, 11);
#if tmp == 1

		uint8_t aucResponse[16];
		unsigned char ucStatus;

		iResult = m_ptTransfer->send_command(aucCommand, 9);

		if (iResult != 0)
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
		}
		else
		{
//			if( m_sizMaxPacketSizeClient!=4+1 ) /* not sure with this */
//			{
//				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to write_data32 has wrong packet size of %d!", m_pcName, this, m_sizMaxPacketSizeClient);
//			}
//			else
//			{
			fOk = true;
//			}
		}

		printf("Receive_packet. \n");
		iResult = m_ptTransfer->receive_packet(aucResponse, sizeof(aucResponse), &ulPacketSize);
#else
		sizAucResponse = (uint32_t*) malloc(sizeof(uint32_t));
		*sizAucResponse = sizeof(usData); // give a hint how many space is needed.
		iResult = execute_command(aucCommand, sizeof(aucCommand), &aucResponse,
				sizAucResponse);
		free(aucResponse);
		free(sizAucResponse);
		aucResponse = NULL;
#endif
	}
	next_sequence_number();

//	}

	if (iResult != 0) {
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}

void romloader_dpm::write_data32(lua_State *ptClientData,
		uint32_t ulNetxAddress, uint32_t ulData) {
	uint8_t aucCommand[11];
	uint32_t ulValue;
	uint32_t ulPacketSize;

	int iResult;
#if tmp == 0
	uint8_t * aucResponse;
	uint32_t * sizAucResponse;
	aucResponse = NULL;
#endif

	ulValue = 0;

	if (m_fIsConnected == false) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!",
				m_pcName, this);
	} else if (m_ptTransfer == NULL) {
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): no transfer object!",
				m_pcName, this);
	} else {
		printf("\n####Writing 0x%x to adress 0x%x####", ulData, ulNetxAddress);
		aucCommand[0] = MONITOR_COMMAND_Write
				| (MONITOR_ACCESSSIZE_Long << MONITOR_ACCESSSIZE_SRT)
				| (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 4;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)(ulNetxAddress & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress >> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress >> 16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress >> 24U) & 0xffU);
		aucCommand[7] = (uint8_t)(ulData & 0xffU);
		aucCommand[8] = (uint8_t)((ulData >> 8U) & 0xffU);
		aucCommand[9] = (uint8_t)((ulData >> 16U) & 0xffU);
		aucCommand[10] = (uint8_t)((ulData >> 24U) & 0xffU);

//		tResult = execute_command(aucCommand, 11);
#if tmp == 1
		uint8_t aucResponse[16];
		unsigned char ucStatus;

		iResult = m_ptTransfer->send_command(aucCommand, 11);

		if (iResult != 0)
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
		}
		else
		{
//			if( m_sizMaxPacketSizeClient!=4+1 ) /* not sure with this */
//			{
//				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to write_data32 has wrong packet size of %d!", m_pcName, this, m_sizMaxPacketSizeClient);
//			}
//			else
//			{
			fOk = true;
//			}
		}

		printf("Receive_packet. \n");
		iResult = m_ptTransfer->receive_packet(aucResponse, sizeof(aucResponse), &ulPacketSize);

#else
		sizAucResponse = (uint32_t*) malloc(sizeof(uint32_t));
		*sizAucResponse = sizeof(ulData); // give a hint how many space is needed.
		iResult = execute_command(aucCommand, sizeof(aucCommand), &aucResponse,
				sizAucResponse);
		free(aucResponse);
		free(sizAucResponse);
		aucResponse = NULL;
#endif
	}
	next_sequence_number();

	if (iResult != 0) {
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}

void romloader_dpm::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN,
		size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData) {
	bool fOk;
	size_t sizChunk;
	bool fIsRunning;
	long lBytesProcessed;

	uint32_t ulValue;

	uint32_t ulPacketSize;

	int iResult;
#if tmp == 1
	uint8_t aucCommand[m_sizMaxPacketSizeHost];
	uint8_t aucResponse[m_sizMaxPacketSizeHost];
#else

	uint8_t * aucResponse;
	uint32_t * sizAucResponse;
	aucResponse = NULL;

#endif

	/* Expect error. */
	fOk = false;

	ulValue = 0;

	if (m_fIsConnected == false) {
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName,
				this);
		fOk = false;
	} else if (m_ptTransfer == NULL) {
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): no transfer object!",
				m_pcName, this);
	} else if (sizBUFFER_IN != 0) {
		lBytesProcessed = 0;
		do {
			sizChunk = sizBUFFER_IN;
			if (sizChunk > m_sizMaxPacketSizeClient - 12) {
				sizChunk = m_sizMaxPacketSizeClient - 12;
			}
			uint8_t aucCommand[sizChunk + 7U]; // from m_sizMaxPacketSizeClient --> doesn't make any sense?

			aucCommand[0] = MONITOR_COMMAND_Write
					| (MONITOR_ACCESSSIZE_Byte << MONITOR_ACCESSSIZE_SRT)
					| (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
			aucCommand[1] = (uint8_t)(sizChunk & 0xffU);
			aucCommand[2] = (uint8_t)((sizChunk >> 8U) & 0xffU);
			aucCommand[3] = (uint8_t)(ulNetxAddress & 0xffU);
			aucCommand[4] = (uint8_t)((ulNetxAddress >> 8U) & 0xffU);
			aucCommand[5] = (uint8_t)((ulNetxAddress >> 16U) & 0xffU);
			aucCommand[6] = (uint8_t)((ulNetxAddress >> 24U) & 0xffU);
			memcpy(aucCommand + 7, pcBUFFER_IN, sizChunk);

#if tmp == 1
			printf("sending command...\n");
			printf("requested adr: 0x%02x%02x%02x%02x\n", aucCommand[6], aucCommand[5], aucCommand[4], aucCommand[3]);
			iResult = m_ptTransfer->send_command(aucCommand, 7 + sizChunk);

			printf("send done: %d\n", iResult);
			if (iResult != 0)
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L/*ptClientData*/,"%s(%p): failed to execute command!", m_pcName, this);

//							MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
			}
			else
			{
				printf("Receive_packet. \n");
				iResult = m_ptTransfer->receive_packet(aucResponse, sizeof(aucResponse), &ulPacketSize);

				printf("receive finished: %d\n", iResult);
				if (iResult != 0)
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): answer to read_data32 has wrong packet size!", m_pcName, this);
				}
				else
				{
#else
			sizAucResponse = (uint32_t*) malloc(sizeof(uint32_t));
			*sizAucResponse = sizBUFFER_IN; // give a hint how many space is needed.
			iResult = execute_command(aucCommand, sizeof(aucCommand),
					&aucResponse, sizAucResponse);
#endif
			ulValue = ((uint32_t)(aucResponse[2]))
					| ((uint32_t)(aucResponse[3])) << 8U
					| ((uint32_t)(aucResponse[4])) << 16U
					| ((uint32_t)(aucResponse[5])) << 24U;
			fOk = true;
			free(aucResponse);
			aucResponse = NULL;
//				}
//			}

			/* ---- */

//			tResult = execute_command(aucCommand, sizChunk+7);
//			if( tResult!=UARTSTATUS_OK )
//			{
//				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
//				fOk = false;
//				break;
//			}
//			else
//			{
			if (*sizAucResponse != 1) {
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
						"%s(%p): answer to write_data08 has wrong packet size of %d!",
						m_pcName, this, ulPacketSize);
				fOk = false;
				free(sizAucResponse);
				break;
			} else {
				pcBUFFER_IN += sizChunk;
				sizBUFFER_IN -= sizChunk;
				ulNetxAddress += sizChunk;
				lBytesProcessed += sizChunk;

				fIsRunning = callback_long(&tLuaFn, lBytesProcessed,
						lCallbackUserData);
				if (fIsRunning != true) {
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
							"%s(%p): write_image canceled!", m_pcName, this);
					fOk = false;
					free(sizAucResponse);
					break;
				}
			}
			free(sizAucResponse);
			next_sequence_number();
		} while (sizBUFFER_IN != 0);
	}

	if (iResult != 0) {
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}

void romloader_dpm::call(uint32_t ulNetxAddress, uint32_t ulParameterR0,
		SWIGLUA_REF tLuaFn, long lCallbackUserData) {

	bool fOk;
	uint8_t aucCommand[9];
	const uint8_t aucCancelBuf[1] = { 0x2b };
	uint8_t ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;

	int iResult = -1;

	uint8_t * aucResponse;
	uint32_t * sizAucResponse;
	aucResponse = NULL;

	if (m_fIsConnected == false) {
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName,
				this);
		fOk = false;
	} else if (m_ptTransfer == NULL) {
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): no transfer object!",
				m_pcName, this);
	} else {
		/* Construct the command packet. */
		aucCommand[0x00] = MONITOR_COMMAND_Execute
				| (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[0x01] = (uint8_t)(ulNetxAddress & 0xffU);
		aucCommand[0x02] = (uint8_t)((ulNetxAddress >> 8) & 0xffU);
		aucCommand[0x03] = (uint8_t)((ulNetxAddress >> 16) & 0xffU);
		aucCommand[0x04] = (uint8_t)((ulNetxAddress >> 24) & 0xffU);
		aucCommand[0x05] = (uint8_t)(ulParameterR0 & 0xffU);
		aucCommand[0x06] = (uint8_t)((ulParameterR0 >> 8) & 0xffU);
		aucCommand[0x07] = (uint8_t)((ulParameterR0 >> 16) & 0xffU);
		aucCommand[0x08] = (uint8_t)((ulParameterR0 >> 24) & 0xffU);

		sizAucResponse = (uint32_t*) malloc(sizeof(uint32_t));
		*sizAucResponse = sizeof(lCallbackUserData); // give a hint how many space is needed.

		//!!! WATCH SIZE AUCRESPONSE RETURN PARAM HERE
		iResult = execute_command(aucCommand, sizeof(aucCommand), &aucResponse,
				sizAucResponse);

		/* ??? */
		next_sequence_number();
		/* --- */

		if (iResult != 0) {
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
					"%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		} else {
			if (*sizAucResponse != /*4*/+1) {
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
						"%s(%p): answer to call command has wrong packet size of %d!",
						m_pcName, this, *sizAucResponse);
				fOk = false;
			} else {
				/* Get the next sequence number. */
				//m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);
				/* Receive message packets. */
				while (1) {
					pcProgressData = NULL;
					sizProgressData = 0;

					uint8_t aucLocalResponse[m_sizMaxPacketSizeClient]; // here we cant predict what netX is going to send us so we take m_sizMaxPacketSizeClient

					uint32_t ulPacketSize;

					/* HERE WE RECEIVE MORE DATA THAN EXPECTED --> WHAT DO WE EXPECT??? */
					iResult = m_ptTransfer->receive_packet(aucLocalResponse,
							sizeof(aucLocalResponse), &ulPacketSize);
//					if( iResult==UARTSTATUS_TIMEOUT )
//					{
//						/* Do nothing in case of timeout. The application is just running quietly. */
//					}
//					else

//					aucLocalResponse[ulPacketSize] = 0;

//					printf("=====> REceived: %s\n", (char*) aucLocalResponse);
					for (int i = 0; i < ulPacketSize; i++) {
						printf("0x%02x ", aucLocalResponse[i]);
						if (i % 16 == 15)
							printf("\n");
					}
					printf("\n");

					if (iResult != 0) {
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
								"%s(%p): Failed to receive a packet: %d",
								m_pcName, this, iResult);
						fOk = false;
					} else {
						if (ulPacketSize < 1) {
							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
									"%s(%p): Received a packet without any user data!",
									m_pcName, this);
							fOk = false;
						} else {
							ucStatus = aucLocalResponse[0] & MONITOR_STATUS_MSK;
							if (ucStatus == MONITOR_STATUS_CallMessage) {
								pcProgressData = (char *) &aucLocalResponse[1];
								sizProgressData = ulPacketSize - 1;

//								pcProgressData = (char*)ulPacketSize+3;
//								sizProgressData = ulPacketSize-5;
							} else if (ucStatus
									== MONITOR_STATUS_CallFinished) {
								fprintf(stderr, "%s(%p): Call has finished!",
										m_pcName, this);
								fOk = true;
								break;
							}
						}
					}

					if (pcProgressData != NULL) {
						fIsRunning = callback_string(&tLuaFn, pcProgressData,
								sizProgressData, lCallbackUserData);
						if (fIsRunning != true) {
							/* Send a cancel request to the device. */
							iResult = m_ptTransfer->send_command(aucCancelBuf,
									1);

							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,
									"%s(%p): the call was canceled!", m_pcName,
									this);
							fOk = false;
							break;
						}
					}

				}
			}
		}

	}

	if (fOk != true) {
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}

/*-------------------------------------*/

romloader_dpm_reference::romloader_dpm_reference(void) :
		muhkuh_plugin_reference() {
}

romloader_dpm_reference::romloader_dpm_reference(const char *pcName,
		const char *pcTyp, bool fIsUsed, romloader_dpm_provider *ptProvider) :
		muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider) {
}

romloader_dpm_reference::romloader_dpm_reference(
		const romloader_dpm_reference *ptCloneMe) :
		muhkuh_plugin_reference(ptCloneMe) {
}

/*-------------------------------------*/
