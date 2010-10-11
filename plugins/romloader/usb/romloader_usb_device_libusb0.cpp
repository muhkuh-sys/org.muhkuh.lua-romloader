/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
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

#include "romloader_usb_device_libusb0.h"

#include <errno.h>
#include <unistd.h>

#if defined(WIN32)
#	define snprintf _snprintf
#	define ETIMEDOUT 116
#else
#	include <sys/time.h>
#endif

#include "romloader_usb_main.h"
#include "uuencoder.h"


#if defined(WIN32)
	const char *romloader_usb_device_libusb0::m_pcLibUsb_BusPattern = "bus-%u";
	const char *romloader_usb_device_libusb0::m_pcLibUsb_DevicePattern = "\\\\.\\libusb0-%u";
#else
	const char *romloader_usb_device_libusb0::m_pcLibUsb_BusPattern = "%u";
	const char *romloader_usb_device_libusb0::m_pcLibUsb_DevicePattern = "%u";

	#include <pthread.h>
	#include <sched.h>
#endif



romloader_usb_device_libusb0::romloader_usb_device_libusb0(const char *pcPluginId)
 : romloader_usb_device(pcPluginId)
 , m_ucEndpoint_In(0)
 , m_ucEndpoint_Out(0)
 , m_iConfiguration(1)
 , m_iInterface(0)
 , m_ptLibUsbContext(NULL)
 , m_ptDevHandle(NULL)
{
	int iResult;


	libusb_init(&m_ptLibUsbContext);
	libusb_set_debug(m_ptLibUsbContext, 3);

}


romloader_usb_device_libusb0::~romloader_usb_device_libusb0(void)
{
	Disconnect();

	if( m_ptLibUsbContext!=NULL )
	{
		libusb_exit(m_ptLibUsbContext);
	}
}


const char *romloader_usb_device_libusb0::m_pcPluginNamePattern = "romloader_usb_%02x_%02x";


int romloader_usb_device_libusb0::libusb_reset_and_close_device(void)
{
	int iResult;


	if( m_ptDevHandle!=NULL )
	{
		iResult = libusb_reset_device(m_ptDevHandle);
		if( iResult==LIBUSB_SUCCESS || iResult==LIBUSB_ERROR_NOT_FOUND )
		{
			libusb_close(m_ptDevHandle);
			m_ptDevHandle = NULL;
			iResult = LIBUSB_SUCCESS;
		}
	}
	else
	{
		/* No open device found. */
		iResult = LIBUSB_ERROR_NOT_FOUND;
	}

	return iResult;
}


int romloader_usb_device_libusb0::detect_interfaces(romloader_usb_reference ***ppptReferences, size_t *psizReferences, romloader_usb_provider *ptProvider)
{
	int iResult;
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device **ptDevCnt, **ptDevEnd;
	libusb_device *ptDev;
//	libusb_device_handle *ptDevHandle;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;
	bool fDeviceIsBusy;
	bool fDeviceIsNetx;
	romloader_usb_reference *ptRef;
	const size_t sizMaxName = 32;
	char acName[sizMaxName];

	size_t sizRefCnt;
	size_t sizRefMax;
	romloader_usb_reference **pptRef;
	romloader_usb_reference **pptRefNew;
	int iCurrentConfiguration;


	/* Expect success. */
	iResult = 0;

	/* Init References array. */
	sizRefCnt = 0;
	sizRefMax = 16;
	pptRef = (romloader_usb_reference**)malloc(sizRefMax*sizeof(romloader_usb_reference*));
	if( pptRef==NULL )
	{
		fprintf(stderr, "out of memory!\n");
		iResult = -1;
	}
	else
	{
		/* detect devices */
		ptDeviceList = NULL;
		ssizDevList = libusb_get_device_list(m_ptLibUsbContext, &ptDeviceList);
		if( ssizDevList<0 )
		{
			/* failed to detect devices */
			printf("%s(%p): failed to detect usb devices: %d:%s\n", m_pcPluginId, this, ssizDevList, libusb_strerror(ssizDevList));
			iResult = -1;
		}
		else
		{
			/* loop over all devices */
			ptDevCnt = ptDeviceList;
			ptDevEnd = ptDevCnt + ssizDevList;
			while( ptDevCnt<ptDevEnd )
			{
				ptDev = *ptDevCnt;
				fDeviceIsNetx = fIsDeviceNetx(ptDev);
				if( fDeviceIsNetx==true )
				{
					/* construct the name */
					uiBusNr = libusb_get_bus_number(ptDev);
					uiDevAdr = libusb_get_device_address(ptDev);
					snprintf(acName, sizMaxName-1, m_pcPluginNamePattern, uiBusNr, uiDevAdr);

					/* open the device */
					iResult = libusb_open(ptDev, &m_ptDevHandle);
					if( iResult!=LIBUSB_SUCCESS )
					{
						/* failed to open the interface, do not add it to the list */
						printf("%s(%p): failed to open device %s: %d:%s\n", m_pcPluginId, this, acName, iResult, libusb_strerror(iResult));
					}
					else
					{
						iResult = libusb_get_configuration(m_ptDevHandle, &iCurrentConfiguration);
						if( iResult!=LIBUSB_SUCCESS )
						{
							/* failed to get the configuration */
							printf("%s(%p): failed to get the configuration of device %s: %d:%s\n", m_pcPluginId, this, acName, iResult, libusb_strerror(iResult));
						}
						else if( iCurrentConfiguration!=m_iConfiguration )
						{
							/* set the configuration */
							iResult = libusb_set_configuration(m_ptDevHandle, m_iConfiguration);
							if( iResult!=LIBUSB_SUCCESS )
							{
								/* failed to set the configuration */
								printf("%s(%p): failed to set the configuration of device %s: %d:%s\n", m_pcPluginId, this, acName, iResult, libusb_strerror(iResult));
							}
						}

						if( iResult==LIBUSB_SUCCESS )
						{
							/* claim the interface, 0 is the interface number */
							iResult = libusb_claim_interface(m_ptDevHandle, m_iInterface);
							if( iResult!=LIBUSB_SUCCESS && iResult!=LIBUSB_ERROR_BUSY )
							{
								/* failed to claim the interface */
								printf("%s(%p): failed to claim interface %d of device %s: %d:%s\n", m_pcPluginId, this, m_iInterface, acName, iResult, libusb_strerror(iResult));
							}
							else
							{
								if( iResult!=LIBUSB_SUCCESS )
								{
									/* the interface is busy! */
									fDeviceIsBusy = true;
								}
								else
								{
									/* ok, claimed the interface -> the device is not busy */
									fDeviceIsBusy = false;
									/* release the interface */
									/* NOTE: The 'busy' information only represents the device state at detection time.
									 * This function _must_not_ keep the claim on the device or other applications will
									 * not be able to use it.
									 */
									iResult = libusb_release_interface(m_ptDevHandle, m_iInterface);
									if( iResult!=LIBUSB_SUCCESS )
									{
										/* failed to release the interface */
										printf("%s(%p): failed to release interface %d of device %s after a successful claim: %d:%s\n", m_pcPluginId, this, m_iInterface, acName, iResult, libusb_strerror(iResult));
									}
								}

								/* create the new instance */
								ptRef = new romloader_usb_reference(acName, m_pcPluginId, fDeviceIsBusy, ptProvider);
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
									pptRefNew = (romloader_usb_reference**)realloc(pptRef, sizRefMax*sizeof(romloader_usb_reference*));
									if( pptRefNew==NULL )
									{
										iResult = -1;
										break;
									}
									pptRef = pptRefNew;
								}
								pptRef[sizRefCnt++] = ptRef;
							}
						}

						/* close the device */
						libusb_close(m_ptDevHandle);
						m_ptDevHandle = NULL;
					}
				}
				/* next list item */
				++ptDevCnt;
			}
			/* free the device list */
			if( ptDeviceList!=NULL )
			{
				libusb_free_device_list(ptDeviceList, 1);
			}
		}
	}

	if( iResult!=0 && pptRef!=NULL )
	{
		/* Free all references in the array. */
		while( sizRefCnt>0 )
		{
			--sizRefCnt;
			ptRef = pptRef[sizRefCnt];
			if( ptRef!=NULL )
			{
				delete ptRef;
			}
		}
		/* Free the array. */
		free(pptRef);
		/* No more array. */
		pptRef = NULL;
		sizRefCnt = 0;
	}

	*ppptReferences = pptRef;
	*psizReferences = sizRefCnt;

	return iResult;
}


libusb_device *romloader_usb_device_libusb0::find_netx_device(libusb_device **ptDeviceList, ssize_t ssizDevList, unsigned int uiBusNr, unsigned int uiDeviceAdr)
{
	libusb_device **ptDevCnt;
	libusb_device **ptDevEnd;
	libusb_device *ptDev;
	libusb_device *ptNetxDevice;
	int iResult;
	const NETX_USB_DEVICE_T *ptId;


	/* No netx found. */
	ptNetxDevice = NULL;

	/* loop over all devices */
	ptDevCnt = ptDeviceList;
	ptDevEnd = ptDevCnt + ssizDevList;
	while( ptNetxDevice==NULL && ptDevCnt<ptDevEnd )
	{
		ptDev = *ptDevCnt;
		if( libusb_get_bus_number(ptDev)==uiBusNr && libusb_get_device_address(ptDev)==uiDeviceAdr )
		{
			ptId = fIsDeviceNetx(ptDev);
			if( ptId!=NULL )
			{
				m_tChiptyp = ptId->tChiptyp;
				m_tRomcode = ptId->tRomcode;
				m_ucEndpoint_In = ptId->ucEndpoint_In;
				m_ucEndpoint_Out = ptId->ucEndpoint_Out;
				ptNetxDevice = ptDev;
			}
			break;
		}

		++ptDevCnt;
	}

	return ptNetxDevice;
}


int romloader_usb_device_libusb0::setup_netx_device(libusb_device *ptNetxDevice)
{
	int iResult;
	int iCurrentConfiguration;


	printf("%s(%p): open device.\n", m_pcPluginId, this);
	iResult = libusb_open(ptNetxDevice, &m_ptDevHandle);
	if( iResult!=LIBUSB_SUCCESS )
	{
		fprintf(stderr, "%s(%p): failed to open the device: %d:%s", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}
	else
	{
		/* Set the configuration. */
		printf("%s(%p): set device configuration to %d.\n", m_pcPluginId, this, m_iConfiguration);

		iResult = libusb_get_configuration(m_ptDevHandle, &iCurrentConfiguration);
		if( iResult!=LIBUSB_SUCCESS )
		{
			/* failed to get the configuration */
			printf("%s(%p): failed to get the configuration of device: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
		}
		else if( iCurrentConfiguration!=m_iConfiguration )
		{
			/* set the configuration */
			iResult = libusb_set_configuration(m_ptDevHandle, m_iConfiguration);
			if( iResult!=LIBUSB_SUCCESS )
			{
				/* failed to set the configuration */
				printf("%s(%p): failed to set the configuration of device: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
			}
		}

		if( iResult==LIBUSB_SUCCESS )
		{
			/* Claim interface 0. */
			printf("%s(%p): claim interface %d.\n", m_pcPluginId, this, m_iInterface);
			iResult = libusb_claim_interface(m_ptDevHandle, m_iInterface);
			if( iResult!=LIBUSB_SUCCESS )
			{
				/* Failed to claim the interface. */
				fprintf(stderr, "%s(%p): failed to claim interface %d: %d:%s\n", m_pcPluginId, this, m_iInterface, iResult, libusb_strerror(iResult));
			}
#if 0
			else
			{

				/* Cleanup in error case. */
				if( iResult!=LIBUSB_SUCCESS )
				{
					/* Release the interface. */
					libusb_release_interface(m_ptDevHandle, m_iInterface);
				}
			}
#endif
		}

		/* Cleanup in error case. */
		if( iResult!=LIBUSB_SUCCESS )
		{
			/* Close the device. */
			libusb_close(m_ptDevHandle);
			m_ptDevHandle = NULL;
		}
	}

	return iResult;
}


int romloader_usb_device_libusb0::Connect(unsigned int uiBusNr, unsigned int uiDeviceAdr)
{
	int iResult;
	SWIGLUA_REF tRef;
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device *ptUsbDevice;


	tRef.L = NULL;
	tRef.ref = 0;

	ptDeviceList = NULL;

	printf("Connect\n");

	/* Search device with bus and address. */
	ssizDevList = libusb_get_device_list(m_ptLibUsbContext, &ptDeviceList);
	if( ssizDevList<0 )
	{
		/* Failed to detect devices. */
		fprintf(stderr, "%s(%p): failed to detect usb devices: %d:%s\n", m_pcPluginId, this, ssizDevList, libusb_strerror(ssizDevList));
		iResult = (int)ssizDevList;
	}
	else
	{
		ptUsbDevice = find_netx_device(ptDeviceList, ssizDevList, uiBusNr, uiDeviceAdr);
		if( ptUsbDevice==NULL )
		{
			fprintf(stderr, "%s(%p): interface not found. Maybe it was plugged out.\n", m_pcPluginId, this);
			iResult = LIBUSB_ERROR_NOT_FOUND;
		}
		else
		{
			iResult = setup_netx_device(ptUsbDevice);
			if( iResult!=LIBUSB_SUCCESS )
			{
				printf("%s(%p): failed to setup the device, trying to reset it.\n", m_pcPluginId, this);

				/* Reset the device and try again. */
				iResult = libusb_reset_and_close_device();
				if( iResult!=LIBUSB_SUCCESS )
				{
					fprintf(stderr, "%s(%p): failed to reset the netx, giving up: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
					libusb_release_interface(m_ptDevHandle, m_iInterface);
					libusb_close(m_ptDevHandle);
					m_ptDevHandle = NULL;
				}
				else
				{
					printf("%s(%p): reset ok!\n", m_pcPluginId, this);

					iResult = setup_netx_device(ptUsbDevice);
					if( iResult!=LIBUSB_SUCCESS )
					{
						fprintf(stderr, "%s(%p): lost device after reset!\n", m_pcPluginId, this);
						iResult = LIBUSB_ERROR_OTHER;
					}
				}
			}
		}

		/* free the device list */
		libusb_free_device_list(ptDeviceList, 1);
	}

	return iResult;
}


void romloader_usb_device_libusb0::Disconnect(void)
{
	if( m_ptDevHandle!=NULL )
	{
		libusb_release_interface(m_ptDevHandle, m_iInterface);
		libusb_close(m_ptDevHandle);
		m_ptDevHandle = NULL;
	}
	m_ptDevHandle = NULL;
}


const NETX_USB_DEVICE_T *romloader_usb_device_libusb0::fIsDeviceNetx(libusb_device *ptDevice) const
{
	const NETX_USB_DEVICE_T *ptDevHit;
	int iResult;
	struct libusb_device_descriptor sDevDesc;
	const NETX_USB_DEVICE_T *ptDevCnt;
	const NETX_USB_DEVICE_T *ptDevEnd;


	ptDevHit = NULL;
	if( ptDevice!=NULL )
	{
		iResult = libusb_get_device_descriptor(ptDevice, &sDevDesc);
		if( iResult==LIBUSB_SUCCESS )
		{
			ptDevCnt = atNetxUsbDevices;
			ptDevEnd = atNetxUsbDevices + (sizeof(atNetxUsbDevices)/sizeof(atNetxUsbDevices[0]));
			while( ptDevCnt<ptDevEnd )
			{
				if(
					sDevDesc.idVendor==ptDevCnt->usVendorId &&
					sDevDesc.idProduct==ptDevCnt->usDeviceId &&
					sDevDesc.bcdDevice==ptDevCnt->usBcdDevice
				)
				{
					/* Found a matching device. */
					ptDevHit = ptDevCnt;
					break;
				}
				else
				{
					++ptDevCnt;
				}
			}
		}
	}

	return ptDevHit;
}



const romloader_usb_device_libusb0::LIBUSB_STRERROR_T romloader_usb_device_libusb0::atStrError[14] =
{
	{ LIBUSB_SUCCESS,		"success" },
	{ LIBUSB_ERROR_IO,		"input/output error" },
	{ LIBUSB_ERROR_INVALID_PARAM,	"invalid parameter" },
	{ LIBUSB_ERROR_ACCESS,		"access denied (insufficient permissions)" },
	{ LIBUSB_ERROR_NO_DEVICE,	"no such device (it may have been disconnected)" },
	{ LIBUSB_ERROR_NOT_FOUND,	"entity not found" },
	{ LIBUSB_ERROR_BUSY,		"resource busy" },
	{ LIBUSB_ERROR_TIMEOUT,		"operation timed out" },
	{ LIBUSB_ERROR_OVERFLOW,	"overflow" },
	{ LIBUSB_ERROR_PIPE,		"pipe error" },
	{ LIBUSB_ERROR_INTERRUPTED,	"system call interrupted (perhaps due to signal)" },
	{ LIBUSB_ERROR_NO_MEM,		"insufficient memory" },
	{ LIBUSB_ERROR_NOT_SUPPORTED,	"operation not supported or unimplemented on this platform" },
	{ LIBUSB_ERROR_OTHER,		"other error" }
};


const char *romloader_usb_device_libusb0::libusb_strerror(int iError)
{
	const char *pcMessage = "unknown error";
	const LIBUSB_STRERROR_T *ptCnt;
	const LIBUSB_STRERROR_T *ptEnd;


	ptCnt = atStrError;
	ptEnd = atStrError + (sizeof(atStrError)/sizeof(atStrError[0]));
	while( ptCnt<ptEnd )
	{
		if(ptCnt->iError==iError)
		{
			pcMessage = ptCnt->pcMessage;
			break;
		}
		++ptCnt;
	}

	return pcMessage;
}


int romloader_usb_device_libusb0::discard_until_timeout(libusb_device_handle *ptDevHandle)
{
	unsigned char aucBuffer[64];
	int iResult;
	int iProcessed;
	unsigned int uiTimeoutMs;


	uiTimeoutMs = 100;

	do
	{
		iResult = libusb_bulk_transfer(ptDevHandle, m_ucEndpoint_In, aucBuffer, sizeof(aucBuffer), &iProcessed, uiTimeoutMs);
		if( iResult==LIBUSB_ERROR_TIMEOUT )
		{
			/* Timeouts are great! */
			iResult = 0;
			break;
		}
		else if( iResult!=0 )
		{
			printf("Failed to receive data: %s\n", libusb_strerror(iResult));
			break;
		}
	} while( iResult==0 );

	return iResult;
}

#if 0
static int romloader_usb_device_libusb0::netx10_load_code(libusb_device_handle *ptDevHandle)
{
	size_t sizLine;
	unsigned char ucData;
	unsigned long ulLoadAddress;
	unsigned char aucBuffer[64];
	unsigned int uiTimeoutMs;
	int iProcessed;
	int iResult;
	const unsigned char *pucNetxCode;
	size_t sizNetxCode;
	uuencoder tUuencoder;


	/* Be optimistic. */
	iResult = 0;

	pucNetxCode = auc_usbmon_netx10_intram;
	sizNetxCode = sizeof(auc_usbmon_netx10_intram);

	/* Construct the command. */
	ulLoadAddress  = pucNetxCode[0];
	ulLoadAddress |= pucNetxCode[1]<<8;
	ulLoadAddress |= pucNetxCode[2]<<16;
	ulLoadAddress |= pucNetxCode[3]<<24;
	sizLine = snprintf(aucBuffer, sizeof(aucBuffer), "l %x\n", ulLoadAddress);
	iResult = usb_bulk_pc_to_netx(ptDevHandle, EP_OUT, aucBuffer, sizLine, &iProcessed, uiTimeoutMs);
	if( iResult!=0 )
	{
		printf("! Failed to send packet!\n");
		iResult = -1;
	}
	else if( sizLine!=iProcessed )
	{
		printf("! Requested to send %d bytes, but only %d were processed!\n", sizLine, iProcessed);
		iResult = -1;
	}
	else
	{
		skip_until_prompt(ptDevHandle);

		uuencoder_init(pucNetxCode, sizNetxCode, &tUuencoder);

		/* Send the data line by line with a delay of 10ms. */
		do
		{
			sizLine = uuencoder_process(&tUuencoder, aucBuffer, sizeof(aucBuffer));
			if( sizLine!=0 )
			{
				uiTimeoutMs = 100;

				{
					size_t sizCnt;
					sizCnt = 0;
					while( sizCnt<sizLine )
					{
						printf("%c", aucBuffer[sizCnt]);
						++sizCnt;
					}
				}

				iResult = usb_bulk_pc_to_netx(ptDevHandle, EP_OUT, aucBuffer, sizLine, &iProcessed, uiTimeoutMs);
				if( iResult!=0 )
				{
					printf("! Failed to send packet!\n");
					iResult = -1;
					break;
				}
				else if( sizLine!=iProcessed )
				{
					printf("! Requested to send %d bytes, but only %d were processed!\n", sizLine, iProcessed);
					iResult = -1;
					break;
				}
				/* usleep delays in us, i.e. a parameter of 10000 means 10ms. */
				usleep(10000);
			}
		} while( uuencoder_isFinished(&tUuencoder)==0 );

		if( iResult==0 )
		{
			skip_until_prompt(ptDevHandle);
		}
	}

	return iResult;
}
#endif


int romloader_usb_device_libusb0::execute_command(const unsigned char *aucOutBuf, size_t sizOutBuf, unsigned char *aucInBuf, size_t *psizInBuf)
{
	int iResult;
	int iProcessed;
	unsigned int uiTimeoutMs;


	uiTimeoutMs = 100;

//	printf("Send Command:\n", iProcessed);
//	hexdump(aucOutBuf, sizOutBuf);

	iResult = libusb_bulk_transfer(m_ptDevHandle, m_ucEndpoint_Out, (unsigned char*)aucOutBuf, sizOutBuf, &iProcessed, uiTimeoutMs);
	if( iResult!=0 )
	{
		printf("Failed to send data: %s\n", libusb_strerror(iResult));
	}
	else if( sizOutBuf!=iProcessed )
	{
		printf("! Requested to send %d bytes, but only %d were processed!\n", sizOutBuf, iProcessed);
		iResult = 1;
	}
	else
	{
		iResult = libusb_bulk_transfer(m_ptDevHandle, m_ucEndpoint_In, aucInBuf, 64, &iProcessed, uiTimeoutMs);
		if( iResult!=0 )
		{
			printf("Failed to receive data: %s\n", libusb_strerror(iResult));
		}
		else if( iProcessed==0 )
		{
			printf("Received empty packet!\n");
			iResult = 1;
		}
		else
		{
			*psizInBuf = iProcessed;
//			printf("Received %d bytes:\n", iProcessed);
//			hexdump(aucInBuf, iProcessed);
		}
	}

	return iResult;
}


int romloader_usb_device_libusb0::receive_packet(unsigned char *aucInBuf, size_t *psizInBuf)
{
	int iResult;
	int iProcessed;
	unsigned int uiTimeoutMs;


	uiTimeoutMs = 100;

//	printf("Send Command:\n", iProcessed);
//	hexdump(aucOutBuf, sizOutBuf);

	iResult = libusb_bulk_transfer(m_ptDevHandle, m_ucEndpoint_In, aucInBuf, 64, &iProcessed, uiTimeoutMs);
	if( iResult==LIBUSB_ERROR_TIMEOUT )
	{
		printf("Timeout: iProcessed=%d\n", iProcessed);
		/* Nothing received. */
		*psizInBuf = 0;
		iResult = 0;
	}
	else if( iResult!=0 )
	{
		printf("Failed to receive data: %s\n", libusb_strerror(iResult));
	}
	else if( iProcessed==0 )
	{
		printf("Received empty packet!\n");
		iResult = 1;
	}
	else
	{
		*psizInBuf = iProcessed;
//		printf("Received %d bytes:\n", iProcessed);
//		hexdump(aucInBuf, iProcessed);
	}

	return iResult;
}
