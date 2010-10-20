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
#include <stdlib.h>

#if defined(WIN32)
#	define snprintf _snprintf
#	define ETIMEDOUT 116
#	define SLEEP_MS(ms) Sleep(ms)
#else
#	include <sys/time.h>
#	include <unistd.h>
#	define SLEEP_MS(ms) usleep(ms*1000)
#endif

#include "romloader_usb_main.h"
#include "uuencoder.h"

#include "netx/targets/usbmon_netx10_intram.h"


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
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device *ptUsbDevice;
	libusb_device *ptUpdatedNetxDevice;


	ptDeviceList = NULL;

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
			/* Reference the device before freeing the list. */
			libusb_ref_device(ptUsbDevice);
			iResult = LIBUSB_SUCCESS;
		}

		/* free the device list */
		libusb_free_device_list(ptDeviceList, 1);

		if( iResult==LIBUSB_SUCCESS )
		{
			/* Does this device need an update? */
			iResult = LIBUSB_ERROR_OTHER;
			switch(m_tRomcode)
			{
			case ROMLOADER_ROMCODE_UNKNOWN:
				iResult = LIBUSB_ERROR_OTHER;
				break;

			case ROMLOADER_ROMCODE_ABOOT:
			case ROMLOADER_ROMCODE_HBOOT:
				iResult = update_old_netx_device(ptUsbDevice, &ptUpdatedNetxDevice);
				if( iResult==LIBUSB_SUCCESS )
				{
					/* Free the old device. */
					libusb_unref_device(ptUsbDevice);
					/* Use the updated device. */
					ptUsbDevice = ptUpdatedNetxDevice;
				}
				break;

			case ROMLOADER_ROMCODE_HBOOT2_SOFT:
				/* The device uses the hboot v2 protocol. */
				iResult = 0;
				break;
			}

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
						/* Free the old device. */
						fprintf(stderr, "%s(%p): lost device after reset!\n", m_pcPluginId, this);
						iResult = LIBUSB_ERROR_OTHER;
					}
				}

				libusb_unref_device(ptUsbDevice);
			}
		}
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


int romloader_usb_device_libusb0::load_code(libusb_device_handle* ptDevHandle, const unsigned char* pucNetxCode, size_t sizNetxCode)
{
	size_t sizLine;
	unsigned long ulLoadAddress;
	unsigned char aucBuffer[64];
	unsigned int uiTimeoutMs;
	int iProcessed;
	int iResult;
	uuencoder tUuencoder;


	/* Be optimistic. */
	iResult = 0;

	uiTimeoutMs = 100;

	if( pucNetxCode[0x00]!='m' || pucNetxCode[0x01]!='o' || pucNetxCode[0x02]!='o' || pucNetxCode[0x03]!='h' )
	{
		printf("! Invalid netx code, header missing.\n");
		iResult = -1;
	}
	else
	{
		/* Construct the command. */
		ulLoadAddress  = pucNetxCode[0x04];
		ulLoadAddress |= pucNetxCode[0x05]<<8;
		ulLoadAddress |= pucNetxCode[0x06]<<16;
		ulLoadAddress |= pucNetxCode[0x07]<<24;
		sizLine = snprintf((char*)aucBuffer, sizeof(aucBuffer), "l %lx\n", ulLoadAddress);
		iResult = libusb_bulk_transfer(ptDevHandle, m_ucEndpoint_Out, aucBuffer, sizLine, &iProcessed, uiTimeoutMs);
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
			discard_until_timeout(ptDevHandle);

			tUuencoder.set_data(pucNetxCode, sizNetxCode);

			/* Send the data line by line with a delay of 10ms. */
			do
			{
				sizLine = tUuencoder.process((char*)aucBuffer, sizeof(aucBuffer));
				if( sizLine!=0 )
				{
					uiTimeoutMs = 100;

					iResult = libusb_bulk_transfer(ptDevHandle, m_ucEndpoint_Out, aucBuffer, sizLine, &iProcessed, uiTimeoutMs);
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
					/* Sleep for 10ms. */
					SLEEP_MS(10);
				}
			} while( tUuencoder.isFinished()==false );

			if( iResult==0 )
			{
				discard_until_timeout(ptDevHandle);
			}
		}
	}

	return iResult;
}


int romloader_usb_device_libusb0::start_code(libusb_device_handle *ptDevHandle)
{
	unsigned char aucBuffer[64];
	size_t sizBlock;
	int iResult;
	int iProcessed;
	unsigned int uiTimeoutMs;
	const unsigned char *pucNetxCode;
	unsigned long ulExecAddress;


	pucNetxCode = auc_usbmon_netx10_intram;

	/* Construct the command. */
	ulExecAddress  = pucNetxCode[0x08];
	ulExecAddress |= pucNetxCode[0x09]<<8;
	ulExecAddress |= pucNetxCode[0x0a]<<16;
	ulExecAddress |= pucNetxCode[0x0b]<<24;

	/* Construct the command. */
	sizBlock = sprintf((char*)aucBuffer, "g %lx 0\n", ulExecAddress);

	uiTimeoutMs = 1000;
	iResult = libusb_bulk_transfer(ptDevHandle, m_ucEndpoint_Out, aucBuffer, sizBlock, &iProcessed, uiTimeoutMs);
	if( iResult!=0 )
	{
		printf("! Failed to send packet!\n");
		iResult = -1;
	}
	else if( sizBlock!=iProcessed )
	{
		printf("! Requested to send %d bytes, but only %d were processed!\n", sizBlock, iProcessed);
		iResult = -1;
	}
	else
	{
		iResult = 0;
	}

	return iResult;
}


int romloader_usb_device_libusb0::compare_uuid_from_string_descriptors(libusb_device *ptDevice, const char *pcUuid)
{
	int iResult;
	struct libusb_device_descriptor sDevDesc;
	libusb_device_handle *ptDeviceHandle;
	const int iStringDescSize = 16;
	/* Each descriptor must be 16 bytes long and one trailing 0. */
	unsigned char aucUuid[2*iStringDescSize+1];


	/* Get the device descriptor. */
	iResult = libusb_get_device_descriptor(ptDevice, &sDevDesc);
	if( iResult==LIBUSB_SUCCESS )
	{
		/* Open the device. */
		iResult = libusb_open(ptDevice, &ptDeviceHandle);
		if( iResult==LIBUSB_SUCCESS )
		{
			/* Get the string descriptor. */
			iResult = libusb_get_string_descriptor_ascii(ptDeviceHandle, sDevDesc.iProduct, aucUuid, iStringDescSize+1);
			if( iResult==iStringDescSize )
			{
				/* Convert the serial number string. */
				iResult = libusb_get_string_descriptor_ascii(ptDeviceHandle, sDevDesc.iSerialNumber, aucUuid+16, iStringDescSize+1);
				if( iResult==iStringDescSize )
				{
					iResult = memcmp(aucUuid, pcUuid, 2*iStringDescSize);
					if( iResult!=0 )
					{
						iResult = LIBUSB_ERROR_OTHER;
					}
					else
					{
						iResult = LIBUSB_SUCCESS;
					}
				}
			}

			libusb_close(ptDeviceHandle);
		}
	}

	return iResult;
}


libusb_device *romloader_usb_device_libusb0::find_device_by_uuid(const char *pcUuid)
{
	int iResult;
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device **ptDevCnt, **ptDevEnd;
	libusb_device *ptDev;
	libusb_device *ptUpdatedDevice;
	bool fDeviceIsNetx;


	/* Expect error. */
	ptUpdatedDevice = NULL;

	/* Detect devices. */
	ptDeviceList = NULL;
	ssizDevList = libusb_get_device_list(m_ptLibUsbContext, &ptDeviceList);
	if( ssizDevList<0 )
	{
		/* Failed to detect devices. */
		printf("%s(%p): failed to detect usb devices: %d:%s\n", m_pcPluginId, this, ssizDevList, libusb_strerror(ssizDevList));
		iResult = -1;
	}
	else
	{
		/* Loop over all devices. */
		ptDevCnt = ptDeviceList;
		ptDevEnd = ptDevCnt + ssizDevList;
		while( ptDevCnt<ptDevEnd )
		{
			/* Is this device a netx? */
			ptDev = *ptDevCnt;
			fDeviceIsNetx = fIsDeviceNetx(ptDev);
			if( fDeviceIsNetx==true )
			{
				/* Yes, this device is a netx. Get the device descriptor. */
				iResult = compare_uuid_from_string_descriptors(ptDev, pcUuid);
				if( iResult==LIBUSB_SUCCESS )
				{
					ptUpdatedDevice = ptDev;
					libusb_ref_device(ptUpdatedDevice);
					break;
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

	return ptUpdatedDevice;
}


char romloader_usb_device_libusb0::nibble_to_asciihex(unsigned char ucNibble)
{
	int iHex;


	iHex = ucNibble + '0';
	if( iHex>'9' )
	{
		iHex += 'a' - '9' - 1;
	}
	return (char)iHex;
}


void romloader_usb_device_libusb0::convert_buffer_to_asciihex(const unsigned char *pucData, size_t sizData, char *pcOutput)
{
	const unsigned char *pucSrcCnt;
	const unsigned char *pucSrcEnd;
	char *pcDstCnt;
	unsigned char ucByte;


	/* Convert the uuid to a string. */
	pucSrcCnt = pucData;
	pucSrcEnd = pucData + sizData;
	pcDstCnt = pcOutput;
	do
	{
		ucByte = *(pucSrcCnt++);
		*(pcDstCnt++) = nibble_to_asciihex((unsigned char)(ucByte>>4U));
		*(pcDstCnt++) = nibble_to_asciihex(ucByte&0x0fU);
	} while( pucSrcCnt<pucSrcEnd );
}


int romloader_usb_device_libusb0::upgrade_netx10_romcode(libusb_device *ptDevice, libusb_device **pptUpdatedNetxDevice)
{
	int iResult;
	libusb_device_handle *ptDevHandle;
	libusb_device *ptUpdatedDevice;
	UUID_T tUuid;
	char acUuid[32];
	int iCnt;
	unsigned char *pucCode;


	printf(". Found old netX10 romcode, starting download.\n");

	iResult = libusb_open(ptDevice, &ptDevHandle);
	if( iResult!=0 )
	{
		printf("Failed to open the device: %s\n", libusb_strerror(iResult));
	}
	else
	{
		/* set the configuration */
		iResult = libusb_set_configuration(ptDevHandle, 1);
		if( iResult!=0 )
		{
			printf("Failed to set config 1: %s\n", libusb_strerror(iResult));
		}
		else
		{
			iResult = libusb_claim_interface(ptDevHandle, 0);
			if( iResult!=0 )
			{
				printf("Failed to claim interface 0: %s\n", libusb_strerror(iResult));

				libusb_close(ptDevHandle);
			}
			else
			{
				/* Read data until 'newline''>' or timeout. */
				discard_until_timeout(ptDevHandle);

				/* Generate a UUID to identify the device when it reappears. */
				uuid_generate(&tUuid);
				convert_buffer_to_asciihex(tUuid.auc, sizeof(tUuid), acUuid);

				pucCode = (unsigned char*)malloc(sizeof(auc_usbmon_netx10_intram));
				if( pucCode==NULL )
				{
					printf("Failed to allocate memory!\n");
					iResult = -1;
				}
				else
				{
					memcpy(pucCode, auc_usbmon_netx10_intram, sizeof(auc_usbmon_netx10_intram));
					/* Copy the UUID to the image. */
					memcpy(pucCode+0x0c, acUuid, sizeof(acUuid));

					/* Load data. */
					load_code(ptDevHandle, pucCode, sizeof(auc_usbmon_netx10_intram));
					free(pucCode);

					/* Start the code with the uuid as parameter. */
					start_code(ptDevHandle);

					/* Release the interface. */
					libusb_release_interface(ptDevHandle, 0);

					/* Reset and close the device. */
					libusb_reset_device(ptDevHandle);
					libusb_close(ptDevHandle);

					/* Poll all 200ms for the new device. */
					iCnt = 0;
					do
					{
						SLEEP_MS(200);
						/* Search for device with uuid. */
						ptUpdatedDevice = find_device_by_uuid(acUuid);
						++iCnt;
					} while( ptUpdatedDevice==NULL && iCnt<10 );

					if( ptUpdatedDevice==NULL )
					{
						iResult = -1;
					}
					else
					{
						*pptUpdatedNetxDevice = ptUpdatedDevice;
						iResult = 0;
					}
				}
			}
		}
	}

	return iResult;
}


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


int romloader_usb_device_libusb0::receive_packet(unsigned char *aucInBuf, size_t *psizInBuf, unsigned int uiTimeoutMs)
{
	int iResult;
	int iProcessed;


//	printf("Send Command:\n", iProcessed);
//	hexdump(aucOutBuf, sizOutBuf);

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
//		printf("Received %d bytes:\n", iProcessed);
//		hexdump(aucInBuf, iProcessed);
	}

	return iResult;
}


int romloader_usb_device_libusb0::update_old_netx_device(libusb_device *ptNetxDevice, libusb_device **pptUpdatedNetxDevice)
{
	int iResult;


	switch(m_tChiptyp)
	{
	case ROMLOADER_CHIPTYP_UNKNOWN:
		iResult = LIBUSB_ERROR_OTHER;
		break;

	case ROMLOADER_CHIPTYP_NETX500:
		/* TODO: insert update code for the netX500. */
		iResult = LIBUSB_ERROR_OTHER;
		break;

	case ROMLOADER_CHIPTYP_NETX100:
		/* TODO: insert update code for the netX100. */
		iResult = LIBUSB_ERROR_OTHER;
		break;

	case ROMLOADER_CHIPTYP_NETX50:
		/* TODO: insert update code for the netX10. */
		iResult = LIBUSB_ERROR_OTHER;
		break;

	case ROMLOADER_CHIPTYP_NETX5:
		/* TODO: insert update code for the netX5. */
		iResult = LIBUSB_ERROR_OTHER;
		break;

	case ROMLOADER_CHIPTYP_NETX10:
		iResult = upgrade_netx10_romcode(ptNetxDevice, pptUpdatedNetxDevice);
		break;
	}

	return iResult;
}

