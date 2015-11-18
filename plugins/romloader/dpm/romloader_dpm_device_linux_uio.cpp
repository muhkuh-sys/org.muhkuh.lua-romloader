/***************************************************************************
 *   Copyright (C) 2015 by Christoph Thelen and M. Trensch                 *
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


#include "romloader_dpm_device_linux_uio.h"

#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>


#if defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
#       include <unistd.h>
#       define SLEEP_MS(ms) usleep(ms*1000)
#endif


romloader_dpm_device_linux_uio::romloader_dpm_device_linux_uio(unsigned int uiUioDevice)
 : m_uiUioDevice(uiUioDevice)
 , m_uiUioBar(0)
 , m_pcUioName(NULL)
 , m_ulUioAddress(0)
 , m_sizUioMemory(0)
{
	m_pvUioMappedMemory.pv = NULL;
}



romloader_dpm_device_linux_uio::romloader_dpm_device_linux_uio(const char *pcDeviceName)
: m_uiUioDevice(0xFFFFFFFF)
, m_uiUioBar(0)
, m_pcUioName(NULL)
, m_ulUioAddress(0)
, m_sizUioMemory(0)
{
	int iScanResult;
	unsigned int uiUioDevice;


	/* Try to parse the device name. */
	iScanResult = sscanf(pcDeviceName, romloader_dpm_device_linux_uio::s_pcDeviceNamePattern, &uiUioDevice);
	if( iScanResult==1 )
	{
		/* Yes, this matches. */
		m_uiUioDevice = uiUioDevice;
	}
}



romloader_dpm_device_linux_uio::~romloader_dpm_device_linux_uio(void)
{
	/* Close any open connection. */
	this->Close();

	/* Free the remaining memory. */
	if( m_pcUioName!=NULL )
	{
		free(m_pcUioName);
	}
}



int romloader_dpm_device_linux_uio::Open(void)
{
	int iResult;


	iResult = this->uio_get_name();
	if( iResult==0 )
	{
		iResult = this->uio_probe();
		if( iResult==0 )
		{
			iResult = uio_get_addr();
			if( iResult==0 )
			{
				iResult = uio_get_size();
				if( iResult==0 )
				{
					iResult = uio_map_dpm();
					if( iResult==0 )
					{
						/* Try to identify the netX DPM. */
						iResult = detect();
					}
				}
			}
		}
	}

	return iResult;
}



void romloader_dpm_device_linux_uio::Close(void)
{
	if( m_pvUioMappedMemory.pv!=NULL )
	{
		munmap(m_pvUioMappedMemory.pv, m_sizUioMemory);
		m_pvUioMappedMemory.pv = NULL;
	}
}



int romloader_dpm_device_linux_uio::dpm_read08(uint32_t ulDpmAddress, uint8_t *pucValue)
{
	int iResult;


	if( (ulDpmAddress+sizeof(uint8_t))>m_sizUioMemory )
	{
		printf("The access at offset 0x%08x exceeds the DPM size of 0x%08zx bytes.\n", ulDpmAddress, m_sizUioMemory);
		iResult = -1;
	}
	else
	{
		*pucValue = *(m_pvUioMappedMemory.puc + ulDpmAddress);
		iResult = 0;
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::dpm_read16(uint32_t ulDpmAddress, uint16_t *pusValue)
{
	int iResult;


	if( (ulDpmAddress+sizeof(uint16_t))>m_sizUioMemory )
	{
		printf("The access at offset 0x%08x exceeds the DPM size of 0x%08zx bytes.\n", ulDpmAddress, m_sizUioMemory);
		iResult = -1;
	}
	else if( (ulDpmAddress&1)!=0 )
	{
		/* The access is not aligned! */
		printf("The read16 access at offset 0x%08x is not aligned.\n", ulDpmAddress);
		iResult = -1;
	}
	else
	{
		*pusValue = *(m_pvUioMappedMemory.pus + (ulDpmAddress/sizeof(uint16_t)));
		iResult = 0;
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::dpm_read32(uint32_t ulDpmAddress, uint32_t *pulValue)
{
	int iResult;


	if( (ulDpmAddress+sizeof(uint32_t))>m_sizUioMemory )
	{
		printf("The access at offset 0x%08x exceeds the DPM size of 0x%08zx bytes.\n", ulDpmAddress, m_sizUioMemory);
		iResult = -1;
	}
	else if( (ulDpmAddress&3)!=0 )
	{
		/* The access is not aligned! */
		printf("The read32 access at offset 0x%08x is not aligned.\n", ulDpmAddress);
		iResult = -1;
	}
	else
	{
		*pulValue = *(m_pvUioMappedMemory.pul + (ulDpmAddress/sizeof(uint32_t)));
		iResult = 0;
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::dpm_read_area(uint32_t ulDpmAddress, uint8_t *pucData, size_t sizData)
{
	int iResult;


	if( (ulDpmAddress+sizData)>m_sizUioMemory )
	{
		printf("The access at offset 0x%08x exceeds the DPM size of 0x%08zx bytes.\n", ulDpmAddress, m_sizUioMemory);
		iResult = -1;
	}
	else
	{
		memcpy(pucData, m_pvUioMappedMemory.puc+ulDpmAddress, sizData);
		iResult = 0;
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::dpm_write08(uint32_t ulDpmAddress, uint8_t ucValue)
{
	int iResult;


	if( (ulDpmAddress+sizeof(uint8_t))>m_sizUioMemory )
	{
		printf("The access at offset 0x%08x exceeds the DPM size of 0x%08zx bytes.\n", ulDpmAddress, m_sizUioMemory);
		iResult = -1;
	}
	else
	{
		*(m_pvUioMappedMemory.puc + ulDpmAddress) = ucValue;
		iResult = 0;
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::dpm_write16(uint32_t ulDpmAddress, uint16_t usValue)
{
	int iResult;


	if( (ulDpmAddress+sizeof(uint16_t))>m_sizUioMemory )
	{
		printf("The access at offset 0x%08x exceeds the DPM size of 0x%08zx bytes.\n", ulDpmAddress, m_sizUioMemory);
		iResult = -1;
	}
	else if( (ulDpmAddress&1)!=0 )
	{
		/* The access is not aligned! */
		printf("The write16 access at offset 0x%08x is not aligned.\n", ulDpmAddress);
		iResult = -1;
	}
	else
	{
		*(m_pvUioMappedMemory.pus + (ulDpmAddress/sizeof(uint16_t))) = usValue;
		iResult = 0;
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::dpm_write32(uint32_t ulDpmAddress, uint32_t ulValue)
{
	int iResult;


	if( (ulDpmAddress+sizeof(uint32_t))>m_sizUioMemory )
	{
		printf("The access at offset 0x%08x exceeds the DPM size of 0x%08zx bytes.\n", ulDpmAddress, m_sizUioMemory);
		iResult = -1;
	}
	else if( (ulDpmAddress&3)!=0 )
	{
		/* The access is not aligned! */
		printf("The write32 access at offset 0x%08x is not aligned.\n", ulDpmAddress);
		iResult = -1;
	}
	else
	{
		*(m_pvUioMappedMemory.pul + (ulDpmAddress/sizeof(uint32_t))) = ulValue;
		iResult = 0;
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::dpm_write_area(uint32_t ulDpmAddress, const uint8_t *pucData, size_t sizData)
{
	int iResult;


	if( (ulDpmAddress+sizData)>m_sizUioMemory )
	{
		printf("The access at offset 0x%08x exceeds the DPM size of 0x%08zx bytes.\n", ulDpmAddress, m_sizUioMemory);
		iResult = -1;
	}
	else
	{
		memcpy(m_pvUioMappedMemory.puc+ulDpmAddress, pucData, sizData);
		iResult = 0;
	}

	return iResult;
}



/* NOTE: This will not work with the mainline driver "uio-netx.ko".
 *       Get the driver from https://kb.hilscher.com/pages/viewpage.action?pageId=22808122
 *
 * TODO: Is there a way to distinguish the mainline and Hilscher version and print a warning?
 * /sys/class/uio/uio0/version is always 0.0.1 . :(
 *
 */
const romloader_dpm_device_linux_uio::NETX_UIO_TYP_T romloader_dpm_device_linux_uio::acKnownNetxUioTypes[1] =
{
	{
		.pcName     = "netx_plx",
		.uiBar      = 0,
		.ulBarSize  = 16384
	}
};


const char *romloader_dpm_device_linux_uio::s_pcDeviceNamePattern = "romloader_dpm_uio%d";


/* Replace newlines at end of string with 0 bytes. */
void romloader_dpm_device_linux_uio::cut_off_newlines(char *pcBuffer)
{
	size_t sizString;
	char cChar;


	/* Remove newline from end of string. */
	sizString = strlen(pcBuffer);
	while( sizString>0 )
	{
		--sizString;
		cChar = pcBuffer[sizString];
		if( cChar!='\n' && cChar!='\r' )
		{
			break;
		}
		else
		{
			pcBuffer[sizString] = '\0';
		}
	}
}



int romloader_dpm_device_linux_uio::uio_get_name(void)
{
	int iResult;
	char acFilename[256];
	char acUioName[256];
	FILE *ptFile;


	snprintf(acFilename, sizeof(acFilename), "/sys/class/uio/uio%d/name", m_uiUioDevice);
	ptFile = fopen(acFilename, "r");
	if( ptFile==NULL )
	{
		iResult = -1;
	}
	else
	{
		if( fgets(acUioName, sizeof(acUioName), ptFile)==NULL )
		{
			iResult = -1;
		}
		else
		{
			cut_off_newlines(acUioName);

			if( m_pcUioName!=NULL )
			{
				free(m_pcUioName);
			}

			m_pcUioName = strdup(acUioName);
			iResult = 0;
		}

		fclose(ptFile);
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::uio_get_size(void)
{
	int iResult;
	char acFilename[256];
	FILE *ptFile;


	snprintf(acFilename, sizeof(acFilename), "/sys/class/uio/uio%d/maps/map%d/size", m_uiUioDevice, m_uiUioBar);
	ptFile = fopen(acFilename, "r");
	if( ptFile==NULL )
	{
		iResult = -1;
	}
	else
	{
		if( fscanf(ptFile, "0x%lx", &m_sizUioMemory)!=1 )
		{
			iResult = -1;
		}
		else
		{
			iResult = 0;
		}

		fclose(ptFile);
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::uio_get_addr(void)
{
	int iResult;
	char acFilename[256];
	FILE *ptFile;


	snprintf(acFilename, sizeof(acFilename), "/sys/class/uio/uio%d/maps/map%d/addr", m_uiUioDevice, m_uiUioBar);
	ptFile = fopen(acFilename, "r");
	if( ptFile==NULL )
	{
		iResult = -1;
	}
	else
	{
		if( fscanf(ptFile, "0x%lx", &m_ulUioAddress)!=1 )
		{
			iResult = -1;
		}
		else
		{
			iResult = 0;
		}

		fclose(ptFile);
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::uio_probe(void)
{
	int iResult;
	const NETX_UIO_TYP_T *ptCnt;
	const NETX_UIO_TYP_T *ptEnd;
	const NETX_UIO_TYP_T *ptMatch;


	/* Compare the name with all entries in the list of known names. */
	ptCnt = acKnownNetxUioTypes;
	ptEnd = acKnownNetxUioTypes + sizeof(acKnownNetxUioTypes)/sizeof(acKnownNetxUioTypes[0]);
	ptMatch = NULL;
	while( ptCnt<ptEnd )
	{
		if( strcmp(ptCnt->pcName, m_pcUioName)==0 )
		{
			ptMatch = ptCnt;

			/* Get the bar number from the list. */
			m_uiUioBar = ptMatch->uiBar;

			break;
		}
		++ptCnt;
	}

	if( ptMatch==NULL )
	{
		fprintf(stderr, "unknown name '%s'\n", m_pcUioName);
		iResult = -1;
	}
	else
	{
		iResult = 0;
	}

	return iResult;
}



int romloader_dpm_device_linux_uio::uio_map_dpm(void)
{
	char acUioDevice[256];
	int iDeviceHandle;
	int iResult;
	void *pvMem;
	int iProtection;
	int iFlags;
	off_t tOffset;


	snprintf(acUioDevice, sizeof(acUioDevice), "/dev/uio%d", m_uiUioDevice);
	iDeviceHandle = open(acUioDevice, O_RDWR);
	if( iDeviceHandle==-1 )
	{
		iResult = -1;
	}
	else
	{
		/* The mapped memory is data only. */
		iProtection = PROT_READ | PROT_WRITE;

		iFlags = MAP_SHARED | MAP_LOCKED | MAP_POPULATE;
		// iFlags = MAP_PRIVATE | MAP_LOCKED | MAP_POPULATE;

		tOffset = m_uiUioBar * sysconf(_SC_PAGE_SIZE);

		pvMem = mmap(NULL, m_sizUioMemory, iProtection, iFlags, iDeviceHandle, tOffset);
		if( pvMem==MAP_FAILED )
		{
			iResult = -1;
		}
		else
		{
			m_pvUioMappedMemory.pv = pvMem;
			iResult = 0;
		}

		close(iDeviceHandle);
	}

	return iResult;
}



size_t romloader_dpm_device_linux_uio::ScanForDevices(char ***pppcDeviceNames)
{
	DIR *ptDirectory;
	struct dirent *ptDirectoryEntry;
	int iResult;
	unsigned int uiUioDevice;
	romloader_dpm_device_linux_uio *ptDpm;
	char strDevicePath[PATH_MAX];
	char *pcDeviceName;
	size_t sizRefCnt;
	size_t sizRefMax;
	char **ppcRef;
	char **ppcRefNew;
	int iStringLength;


	sizRefCnt = 0;
	sizRefMax = 16;
	ppcRef = (char**)malloc(sizRefMax*sizeof(char*));
	if( ppcRef==NULL )
	{
		fprintf(stderr, "out of memory!\n");
		iResult = -1;
	}
	else
	{
		/* Clear the complete array. */
		memset(ppcRef, 0, sizRefMax*sizeof(char*));

		/* Loop over all files in the folder "/sys/class/uio/" and look for
		 * names like "uio%d".
		 */

		/* Open the directory. */
		ptDirectory = opendir("/sys/class/uio/");
		if( ptDirectory!=NULL )
		{
			/* Loop over all entries. */
			do
			{
				ptDirectoryEntry = readdir(ptDirectory);
				if( ptDirectoryEntry!=NULL )
				{
					/* Compare the filename. */
					iResult = sscanf(ptDirectoryEntry->d_name, "uio%u", &uiUioDevice);
					if( iResult==1 )
					{
						printf("Trying UIO device %d...\n", uiUioDevice);

						/* Try to identify the UIO device and map it into the memory. */
						ptDpm = new romloader_dpm_device_linux_uio(uiUioDevice);
						iResult = ptDpm->Open();
						if( iResult==0 )
						{
							/* Is enough space in the list? */
							printf("Detected a netX DPM on UIO%d. Typ=%d\n", uiUioDevice, ptDpm->get_chiptyp());

							/* Create a new entry. */
							iStringLength = snprintf(strDevicePath, PATH_MAX-1, "romloader_dpm_uio%d", uiUioDevice);
							if( iStringLength<=0 )
							{
								iResult = -1;
								break;
							}

							/* Is enough space in the array for one more entry? */
							if( sizRefCnt>=sizRefMax )
							{
								/* No -> expand the array. */
								sizRefMax *= 2;
								/* Detect overflow or limitation. */
								if( sizRefMax<=sizRefCnt )
								{
									iResult = -1;
									break;
								}
								/* Reallocate the array. */
								ppcRefNew = (char**)realloc(ppcRef, sizRefMax*sizeof(char*));
								if( ppcRefNew==NULL )
								{
									iResult = -1;
									break;
								}
								ppcRef = ppcRefNew;
							}

							/* Allocate memory for the new reference. */
							pcDeviceName = (char*)malloc(iStringLength+1);
							if( pcDeviceName==NULL )
							{
								iResult = -1;
								break;
							}

							/* Copy the reference to the string. */
							memcpy(pcDeviceName, strDevicePath, iStringLength+1);

							/* Append the string to the list. */
							ppcRef[sizRefCnt++] = pcDeviceName;
						}

						/* Close the UIO device again. */
						ptDpm->Close();
						delete ptDpm;
						ptDpm = NULL;
					}
				}
			} while( ptDirectoryEntry!=NULL );
		}

		if( iResult!=0 )
		{
			/* Free all strings in the list. */
			while( sizRefCnt!=0 )
			{
				--sizRefCnt;
				free(ppcRef[sizRefCnt]);
			}
			/* Free the list itself. */
			free(ppcRef);
			ppcRef = NULL;
		}
	}

	if( iResult!=0 )
	{
		sizRefCnt = 0;
	}

	*pppcDeviceNames = ppcRef;

	return sizRefCnt;
}



int romloader_dpm_device_linux_uio::IsMyDeviceName(const char *pcDeviceName)
{
	int iResult;
	int iScanResult;
	unsigned int uiDevice;


	/* Be pessimistic. */
	iResult = -1;

	/* Try to parse the device name. */
	iScanResult = sscanf(pcDeviceName, romloader_dpm_device_linux_uio::s_pcDeviceNamePattern, &uiDevice);
	if( iScanResult==1 )
	{
		/* Yes, this matches. */
		iResult = 0;
	}

	return iResult;
}

