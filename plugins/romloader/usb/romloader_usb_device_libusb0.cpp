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
#if defined(WIN32)
 , m_hRxThread(NULL)
 , m_hRxDataAvail(NULL)
#else
 , m_ptRxThread(NULL)
 , m_ptRxDataAvail_Condition(NULL)
 , m_ptRxDataAvail_Mutex(NULL)
#endif
{
	int iResult;


	libusb_init(&m_ptLibUsbContext);
	libusb_set_debug(m_ptLibUsbContext, 3);

#if defined(WIN32)
	m_hRxDataAvail = CreateEvent(NULL, FALSE, FALSE, NULL);
	if( m_hRxDataAvail==NULL )
	{
		fprintf(stderr, "%s(%p): Failed to create card mutex: %ul\n", m_pcPluginId, this, GetLastError());
		iResult = -1;
	}
#else
	m_ptRxDataAvail_Condition = new pthread_cond_t;
	iResult = pthread_cond_init(m_ptRxDataAvail_Condition, NULL);
	if( iResult!=0 )
	{
		fprintf(stderr, "failed to init m_ptRxDataAvail_Condition: %d\n", iResult);
		delete m_ptRxDataAvail_Condition;
		m_ptRxDataAvail_Condition = NULL;
	}
	else
	{
		m_ptRxDataAvail_Mutex = new pthread_mutex_t;
		iResult = pthread_mutex_init(m_ptRxDataAvail_Mutex, NULL);
		if( iResult!=0 )
		{
			fprintf(stderr, "failed to init m_ptRxDataAvail_Condition: %d\n", iResult);
			pthread_cond_destroy(m_ptRxDataAvail_Condition);
			delete m_ptRxDataAvail_Condition;
			m_ptRxDataAvail_Condition = NULL;

			delete m_ptRxDataAvail_Mutex;
			m_ptRxDataAvail_Mutex = NULL;
		}
	}
#endif
}


romloader_usb_device_libusb0::~romloader_usb_device_libusb0(void)
{
	Disconnect();

 	printf("Destructor 1\n");
#if defined(WIN32)
	if( m_hRxDataAvail!=NULL )
	{
		CloseHandle(m_hRxDataAvail);
		m_hRxDataAvail = NULL;
	}
#else
	if( m_ptRxDataAvail_Condition!=NULL )
	{
		pthread_cond_destroy(m_ptRxDataAvail_Condition);
		delete m_ptRxDataAvail_Condition;
		m_ptRxDataAvail_Condition = NULL;
	}

	if( m_ptRxDataAvail_Mutex!=NULL )
	{
		pthread_mutex_destroy(m_ptRxDataAvail_Mutex);
		delete m_ptRxDataAvail_Mutex;
		m_ptRxDataAvail_Mutex = NULL;
	}
#endif

	printf("Destructor 2\n");
	if( m_ptLibUsbContext!=NULL )
	{
		libusb_exit(m_ptLibUsbContext);
	}
	printf("Destructor 3\n");
}


#if defined(WIN32)
DWORD WINAPI romloader_usb_device_libusb0::rxThread(LPVOID lpParam)
{
	romloader_usb_device_libusb0 *ptParent;


	/* Get the parent class. */
	ptParent = (romloader_usb_device_libusb0*)lpParam;
	return ptParent->localRxThread();
}


DWORD romloader_usb_device_libusb0::localRxThread(void)
{
	int iError;
	DWORD dwResult;
	const size_t sizBufferSize = 4096;
	union
	{
		unsigned char auc[sizBufferSize];
		char ac[sizBufferSize];
	} uBuffer;


	/* Expect error. */
	dwResult = FALSE;

	do
	{
		iError = usb_bulk_read(m_ptDevHandle, m_ucEndpoint_In, uBuffer.ac, sizBufferSize, 200);
		if( iError>0 )
		{
			/* transfer ok! */
			writeCards(uBuffer.auc, iError);

			/* Send a signal to any waiting threads. */
			SetEvent(m_hRxDataAvail);
			iError = 0;
		}
		else if( iError==-ETIMEDOUT )
		{
			/* Timeout. */
			iError = 0;
		}

		/* Check for thread cancelation. */
		if( m_fRxThread_RequestTermination==true )
		{
			dwResult = TRUE;
			break;
		}
	} while( iError>=0 );

	return dwResult;
}
#else
void *romloader_usb_device_libusb0::rxThread(void *pvParameter)
{
	romloader_usb_device_libusb0 *ptParent;


	/* Get the parent class. */
	ptParent = (romloader_usb_device_libusb0*)pvParameter;
	return ptParent->localRxThread();
}


void *romloader_usb_device_libusb0::localRxThread(void)
{
	int iError;
	int iOldValue;
	const int iBufferSize = 4096;
	unsigned char aucBuffer[iBufferSize];
	int iTransfered;
	const unsigned int uiRxTimeout = 200;


	/* Allow exit at cancellation points. */
	iError = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &iOldValue);
	if( iError==0 )
	{
		iError = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldValue);
		if( iError==0 )
		{
			do
			{
				iError = libusb_bulk_transfer(m_ptDevHandle, m_ucEndpoint_In, aucBuffer, iBufferSize, &iTransfered, uiRxTimeout);
				if( iError==LIBUSB_SUCCESS || iError==LIBUSB_ERROR_TIMEOUT )
				{
					/* Transfer ok! */
					writeCards(aucBuffer, iTransfered);
					/* Send a signal to any waiting threads. */
					pthread_cond_signal(m_ptRxDataAvail_Condition);
					/* Ignore Timeout errors. */
					iError = LIBUSB_SUCCESS;
				}

				pthread_testcancel();
			} while( iError==LIBUSB_SUCCESS );
		}
	}

	fprintf(stderr, "rxThread exit with %d\n", iError);
	pthread_exit((void*)iError);
}
#endif


const char *romloader_usb_device_libusb0::m_pcPluginNamePattern = "romloader_usb_%02x_%02x";


int romloader_usb_device_libusb0::start_rx_thread(void)
{
	int iResult;


	/* Create the receive thread. */
#if defined(WIN32)
	m_fRxThread_RequestTermination = false;
	m_hRxThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) romloader_usb_device_libusb0::rxThread, (void*)this, 0, NULL);
	if( m_hRxThread==NULL )
	{
		fprintf(stderr, "%s(%p): Failed to create card mutex: %d:%s\n", m_pcPluginId, this, errno, strerror(errno));
		iResult = -1;
	}
	else
	{
		iResult = 0;
	}
#else
	pthread_attr_t tAttr;
	sched_param tParam;
	int iPolicy;


	/* initialized with default attributes */
	iResult = pthread_attr_init(&tAttr);
	if( iResult!=0 )
	{
		fprintf(stderr, "Failed to init the thread attributes: %d\n", iResult);
	}
	else
	{
		/* safe to get existing scheduling param */
		iResult = pthread_attr_getschedparam (&tAttr, &tParam);
		if( iResult!=0 )
		{
			fprintf(stderr, "Failed to get the scheduling parameter: %d\n", iResult);
		}
		else
		{
			iResult = pthread_attr_getschedpolicy(&tAttr, &iPolicy);
			if( iResult!=0 )
			{
				fprintf(stderr, "Failed to get the scheduling policy: %d\n", iResult);
			}
			else
			{
				printf("scheduling policy is: %d\n", iPolicy);
#if 0
				if( iPolicy==SCHED_OTHER )
				{
					/* Change policy to RR. */
					printf("Changing policy from OTHER to RR.\n");
					iResult = pthread_attr_setschedpolicy(&tAttr, SCHED_RR);
					if(  iResult!=0 )
					{
						fprintf(stderr, "Failed to set the scheduling policy: %d\n", iResult);
					}
				}

				if( iResult==0 )
				{
					printf("Old rxThread priority is: %d\n", tParam.sched_priority);
					/* set the priority; others are unchanged */
					++tParam.sched_priority;
					printf("New rxThread priority is: %d\n", tParam.sched_priority);
	
					/* setting the new scheduling param */
					iResult = pthread_attr_setschedparam (&tAttr, &tParam);
					if( iResult!=0 )
					{
						fprintf(stderr, "Failed to set the scheduling parameter: %d\n", iResult);
					}
					else
					{
#endif
						m_ptRxThread = new pthread_t;
						iResult = pthread_create(m_ptRxThread, &tAttr, romloader_usb_device_libusb0::rxThread, (void*)this);
						if( iResult!=0 )
						{
							fprintf(stderr, "%s(%p): Failed to create card mutex: %d:%s\n", m_pcPluginId, this, errno, strerror(errno));
							delete m_ptRxThread;
							m_ptRxThread = NULL;
						}
//					}
//				}
			}
		}
	}
#endif
	return iResult;
}


int romloader_usb_device_libusb0::stop_rx_thread(void)
{
#if defined(WIN32)
	int iResult;


	if( m_hRxThread!=NULL )
	{
		m_fRxThread_RequestTermination = true;
		WaitForSingleObject(m_hRxThread, INFINITE);
		CloseHandle(m_hRxThread);
		m_hRxThread = NULL;
		iResult = 0;
	}
#else
	int iResult;
	void *pvStatus;
	int iStatus;


	printf("m_ptRxThread=%p\n", m_ptRxThread);

	if( m_ptRxThread!=NULL )
	{
		iResult = pthread_cancel(*m_ptRxThread);
		if( iResult!=0 )
		{
			fprintf(stderr, "pthread_cancel failed: %d\n", iResult);
		}
		else
		{
			iResult = pthread_join(*m_ptRxThread, &pvStatus);
			if( iResult!=0 )
			{
				fprintf(stderr, "pthread_join failed: %d\n", iResult);
			}
			else
			{
				iStatus = (int)pvStatus;
				printf("rxthread finished with status %d\n", iStatus);
			}
		}

		delete m_ptRxThread;
		m_ptRxThread = NULL;
	}
	else
	{
		printf("No rxthread running!\n");

		iResult = 0;
	}
#endif
	return iResult;
}


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
	const NETX_USB_DEVICE_T *ptIdCnt;
	const NETX_USB_DEVICE_T *ptIdEnd;
	struct libusb_device_descriptor sDevDesc;


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
			/* Get the vendor and product id. */
			iResult = libusb_get_device_descriptor(ptDev, &sDevDesc);
			if( iResult==LIBUSB_SUCCESS )
			{
				printf("Hello device (VID=0x%04x, PID=0x%04x)\n", sDevDesc.idVendor, sDevDesc.idProduct);
				/* Loop over all known devices. */
				ptIdCnt = atNetxUsbDevices;
				ptIdEnd = atNetxUsbDevices + (sizeof(atNetxUsbDevices)/sizeof(atNetxUsbDevices[0]));
				printf("probing %d devices\n", sizeof(atNetxUsbDevices)/sizeof(atNetxUsbDevices[0]));
				while(ptIdCnt<ptIdEnd)
				{
					printf("Are you a %s?\n", ptIdCnt->pcName);
					if( sDevDesc.idVendor==ptIdCnt->usVendorId && sDevDesc.idProduct==ptIdCnt->usDeviceId )
					{
						printf("Found VID=0x%04x, PID=0x%04x -> %s\n", sDevDesc.idVendor, sDevDesc.idProduct, ptIdCnt->pcName);
						m_tChiptyp = ptIdCnt->tChiptyp;
						m_tRomcode = ptIdCnt->tRomcode;
						m_ucEndpoint_In = ptIdCnt->ucEndpoint_In;
						m_ucEndpoint_Out = ptIdCnt->ucEndpoint_Out;
						ptNetxDevice = ptDev;
						break;
					}
					++ptIdCnt;
				}
				printf("End of search\n");
			}
		}

		++ptDevCnt;
	}

	return ptNetxDevice;
}


int romloader_usb_device_libusb0::setup_netx_device(libusb_device *ptNetxDevice)
{
	int iResult;
	size_t sizReceivedTotal;
	size_t sizChunk;
	const size_t sizMaxChunk = 65536;
	const size_t sizMaxData = 524288;
	unsigned char aucChunkBuffer[sizMaxChunk];
	const unsigned int uiChunkReceiveTimeoutMs = 100;
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
			else
			{
				printf("%s(%p): start rx thread.\n", m_pcPluginId, this);
				iResult = start_rx_thread();
				if( iResult!=0 )
				{
					/* Failed to start the receive thread. */
					fprintf(stderr, "%s(%p): failed to start the receive thread: %d\n", m_pcPluginId, this, iResult);
				}
				else
				{
					/* Get any waiting data. This can be garbage from the last connection or the netx welcome message. */
					printf("%s(%p): receive welcome message.\n", m_pcPluginId, this);

					sizReceivedTotal = 0;
					do
					{
						/* Receive a chunk of data. */
						sizChunk = usb_receive(aucChunkBuffer, sizMaxChunk, uiChunkReceiveTimeoutMs);
						printf("received %d bytes of data:\n", sizChunk);
						hexdump(aucChunkBuffer, sizChunk);
						/* Is the maximum already reached? */
						sizReceivedTotal += sizChunk;
						if( sizReceivedTotal>=sizMaxData )
						{
							/* Guess the device will never shut up! */
							fprintf(stderr, "%s(%p): received too much initial data from the device, guess it will never shut up!\n", m_pcPluginId, this);
							iResult = -1;
							stop_rx_thread();
							break;
						}
					} while( sizChunk!=0 );
				}

				/* Cleanup in error case. */
				if( iResult!=LIBUSB_SUCCESS )
				{
					/* Release the interface. */
					libusb_release_interface(m_ptDevHandle, m_iInterface);
				}
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
	printf("Disconnect 1\n");
	stop_rx_thread();
	printf("Disconnect 2\n");
	if( m_ptDevHandle!=NULL )
	{
		printf("m_ptDevHandle=%p, m_iInterface=%d\n", m_ptDevHandle, m_iInterface);
		libusb_release_interface(m_ptDevHandle, m_iInterface);
		printf("a\n");
		libusb_close(m_ptDevHandle);
		printf("b\n");
		m_ptDevHandle = NULL;
	}
	printf("Disconnect 3\n");
	m_ptDevHandle = NULL;
}


bool romloader_usb_device_libusb0::fIsDeviceNetx(libusb_device *ptDevice)
{
	bool fDeviceIsNetx;
	int iResult;
	struct libusb_device_descriptor sDevDesc;


	fDeviceIsNetx = false;
	if( ptDevice!=NULL )
	{
		iResult = libusb_get_device_descriptor(ptDevice, &sDevDesc);
		if( iResult==LIBUSB_SUCCESS )
		{
			if(
				sDevDesc.bDeviceClass==0x00 &&
				sDevDesc.bDeviceSubClass==0x00 &&
				sDevDesc.bDeviceProtocol==0x00 &&
				sDevDesc.idVendor==0x0cc4 &&
				sDevDesc.idProduct==0x0815 &&
				sDevDesc.bcdDevice==0x0100
			  )
			{
				/* This seems to be a netX500 ABoot device. */
				fDeviceIsNetx  = true;
			}
			else if(
				sDevDesc.bDeviceClass==0xff &&
				sDevDesc.bDeviceSubClass==0x00 &&
				sDevDesc.bDeviceProtocol==0xff &&
				sDevDesc.idVendor==0x1939 &&
				sDevDesc.idProduct==0x000c &&
				sDevDesc.bcdDevice==0x0001
			       )
			{
				/* This seems to be a netX10 HBoot device. */
				fDeviceIsNetx  = true;
			}
		}
	}

	return fDeviceIsNetx;
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



#if defined(WIN32)
size_t romloader_usb_device_libusb0::usb_receive(unsigned char *pucBuffer, size_t sizBuffer, unsigned int uiTimeoutMs)
{
	size_t sizReceived;
	size_t sizChunk;
	DWORD dwStartTime;
	DWORD dwTimeElapsed;
	DWORD dwTimeLeft;
	DWORD dwWaitResult;
	DWORD dwTimeoutMs;


	/* No data received. */
	sizReceived = 0;

	dwTimeoutMs = uiTimeoutMs;
	dwStartTime = GetTickCount();

	while( sizBuffer>0 )
	{
		/* Receive as much of the requested data as possible. */
		sizChunk = readCards(pucBuffer, sizBuffer);
		if( sizChunk>0 )
		{
			/* Received some data. */
			pucBuffer += sizChunk;
			sizBuffer -= sizChunk;
			sizReceived += sizChunk;
		}
		else
		{
			/* No data left, wait for input. */
			dwTimeElapsed = GetTickCount() - dwStartTime;
			if( dwTimeElapsed>=uiTimeoutMs )
			{
				/* Timeout, no more data arrived. */
				break;
			}
			else
			{
				dwTimeLeft = dwTimeoutMs - dwTimeElapsed;

				/* Wait for new data to arrive. */
				dwWaitResult = WaitForSingleObject(m_hRxDataAvail, dwTimeLeft);
				if( dwWaitResult!=WAIT_OBJECT_0 )
				{
					/* Timeout or error -> no new data arrived. */
					break;
				}
			}
		}
	}

	return sizReceived;
}


int romloader_usb_device_libusb0::usb_receive_line(char *pcBuffer, size_t sizBuffer, unsigned int uiTimeoutMs, size_t *psizReceived)
{
	int iResult;
	unsigned char *pucCnt;
	size_t sizProcessed;
	const unsigned char aucEol[3] = { '\r', '\n', 0 };
	int iState;
	tBufferCard *ptCard;
	size_t sizReceived;
	union
	{
		unsigned char *puc;
		char *pc;
	} uBuffer;
	DWORD dwStartTime;
	DWORD dwTimeElapsed;
	DWORD dwTimeLeft;
	DWORD dwWaitResult;
	DWORD dwTimeoutMs;


	dwTimeoutMs = uiTimeoutMs;
	dwStartTime = GetTickCount();

	/* Start to check for EOL at the first card. */
	pucCnt = m_ptFirstCard->pucRead;
	sizProcessed = 0;
	iState = 0;
	ptCard = m_ptFirstCard;
	sizReceived = 0;

	/* Expect success. */
	iResult = 0;

	while( sizProcessed<sizBuffer )
	{
		/* Is the write pointer is in first card? */
		if( ptCard->pucWrite!=NULL )
		{
			/* Are more chars available? */
			if( pucCnt<ptCard->pucWrite )
			{
				if( *pucCnt==aucEol[iState] )
				{
					++sizProcessed;
					++pucCnt;
					++iState;
					if( aucEol[iState]==0 )
					{
						/* Ok, EOL found! */
						break;
					}
				}
				else
				{
					if( iState==0 )
					{
						++sizProcessed;
						++pucCnt;
					}
					/* Reset EOL detect counter. */
					iState = 0;
				}
			}
			else
			{
				/* No data left, wait for input. */
				dwTimeElapsed = GetTickCount() - dwStartTime;
				if( dwTimeElapsed>=uiTimeoutMs )
				{
					/* Timeout, no more data arrived. */
					break;
				}
				else
				{
					dwTimeLeft = dwTimeoutMs - dwTimeElapsed;

					/* Wait for new data to arrive. */
					dwWaitResult = WaitForSingleObject(m_hRxDataAvail, dwTimeLeft);
					if( dwWaitResult==WAIT_TIMEOUT )
					{
						/* Timeout or error -> no new data arrived. */
						break;
					}
					else if( dwWaitResult!=WAIT_OBJECT_0 )
					{
						/* Error. */
						iResult = -1;
						break;
					}
				}
			}
		}
		else
		{
			/* Are more chars available? */
			if( pucCnt<ptCard->pucEnd )
			{
				if( *pucCnt==aucEol[iState] )
				{
					++sizProcessed;
					++pucCnt;
					++iState;
					if( aucEol[iState]==0 )
					{
						/* Ok, EOL found! */
						break;
					}
				}
				else
				{
					if( iState==0 )
					{
						++sizProcessed;
						++pucCnt;
					}
					/* Reset EOL detect counter. */
					iState = 0;
				}
			}
			else
			{
				ptCard = ptCard->ptNext;
				pucCnt = ptCard->aucData;
			}
		}
	}

	if( iResult==0 && sizProcessed>0 )
	{
		uBuffer.pc = pcBuffer;
		sizReceived = usb_receive(uBuffer.puc, sizProcessed, 1);
		if( sizReceived==sizProcessed )
		{
			/* Remove linefeed from buffer. */
			sizReceived -= iState;
			uBuffer.puc[sizReceived] = 0;
		}
		*psizReceived = sizReceived;
	}

	return iResult;
}
#else
#define MILLISEC_PER_SEC 1000UL
#define NANOSEC_PER_MILLISEC 1000000UL
#define NANOSEC_PER_MICROSEC 1000UL
#define NANOSEC_PER_SEC 1000000000UL

int romloader_usb_device_libusb0::get_end_time(unsigned int uiTimeoutMs, struct timespec *ptEndTime)
{
	int iResult;
	struct timeval tTimeVal;
	struct timespec tCurTime;
	struct timespec tAddTime;
	struct timespec tSumTime;


	iResult = gettimeofday(&tTimeVal, NULL);
	if( iResult!=0 )
	{
		fprintf(stderr, "clock_gettime failed with result %d, errno: %d (%s)", iResult, errno, strerror(errno));
	}
	else
	{
		/* Convert the current time in a timeval structure to timespec. */
		tCurTime.tv_sec  = tTimeVal.tv_sec;
		tCurTime.tv_nsec = tTimeVal.tv_usec * NANOSEC_PER_MICROSEC;

		/* Convert the timeout value in milliseconds to a timespec. */
		tAddTime.tv_sec  = uiTimeoutMs/MILLISEC_PER_SEC;
		tAddTime.tv_nsec = (uiTimeoutMs%MILLISEC_PER_SEC) * NANOSEC_PER_MILLISEC;
	
		/* Add the timeout to the current time. */
		tSumTime.tv_sec = tCurTime.tv_sec   + tAddTime.tv_sec;
		tSumTime.tv_nsec = tCurTime.tv_nsec + tAddTime.tv_nsec;
		while( tSumTime.tv_nsec>NANOSEC_PER_SEC )
		{
			tSumTime.tv_nsec -= NANOSEC_PER_SEC;
			++tSumTime.tv_sec;
		}

		/* Set result. */
		ptEndTime->tv_sec  = tSumTime.tv_sec;
		ptEndTime->tv_nsec = tSumTime.tv_nsec;
	}

	return iResult;
}


size_t romloader_usb_device_libusb0::usb_receive(unsigned char *pucBuffer, size_t sizBuffer, unsigned int uiTimeoutMs)
{
	int iResult;
	int iWaitResult;
	size_t sizReceived;
	size_t sizChunk;
	struct timespec tEndTime;


	/* No data received. */
	sizReceived = 0;
	iResult = get_end_time(uiTimeoutMs, &tEndTime);
	if( iResult!=0 )
	{
		fprintf(stderr, "get_end_time failed with result %d, errno: %d (%s)", iResult, errno, strerror(errno));
	}
	else
	{
		while( sizBuffer>0 )
		{
			/* Receive as much of the requested data as possible. */
			sizChunk = readCards(pucBuffer, sizBuffer);
			if( sizChunk>0 )
			{
				/* Received some data. */
				pucBuffer += sizChunk;
				sizBuffer -= sizChunk;
				sizReceived += sizChunk;
			}
			else
			{
				/* No data left, wait for input. */
#if !defined(WIN32)
				sched_yield();
#endif
				/* Wait for data. */
				pthread_mutex_lock(m_ptRxDataAvail_Mutex);
				iWaitResult = pthread_cond_timedwait(m_ptRxDataAvail_Condition, m_ptRxDataAvail_Mutex, &tEndTime);
				pthread_mutex_unlock(m_ptRxDataAvail_Mutex);

				if( iWaitResult==ETIMEDOUT )
				{
					/* Timeout, no more data arrived. */
					break;
				}
				else if( iWaitResult!=0 )
				{
					fprintf(stderr, "failed to wait for data available condition: %d\n", iWaitResult);
					break;
				}
			}
		}
	}

	return sizReceived;
}


int romloader_usb_device_libusb0::usb_receive_line(char *pcBuffer, size_t sizBuffer, unsigned int uiTimeoutMs, size_t *psizReceived)
{
	int iResult;
	struct timespec tEndTime;
	unsigned char *pucCnt;
	size_t sizProcessed;
	const unsigned char aucEol[3] = { '\r', '\n', 0 };
	int iState;
	tBufferCard *ptCard;
	size_t sizReceived;
	union
	{
		unsigned char *puc;
		char *pc;
	} uBuffer;


	/* Start to check for EOL at the first card. */
	pucCnt = m_ptFirstCard->pucRead;
	sizProcessed = 0;
	iState = 0;
	ptCard = m_ptFirstCard;
	sizReceived = 0;

	iResult = get_end_time(uiTimeoutMs, &tEndTime);
	if( iResult!=0 )
	{
		fprintf(stderr, "get_end_time failed with result %d, errno: %d (%s)", iResult, errno, strerror(errno));
	}
	else
	{
		while( sizProcessed<sizBuffer )
		{
			/* Is the write pointer is in first card? */
			if( ptCard->pucWrite!=NULL )
			{
				/* Are more chars available? */
				if( pucCnt<ptCard->pucWrite )
				{
					if( *pucCnt==aucEol[iState] )
					{
						++sizProcessed;
						++pucCnt;
						++iState;
						if( aucEol[iState]==0 )
						{
							/* Ok, EOL found! */
							break;
						}
					}
					else
					{
						if( iState==0 )
						{
							++sizProcessed;
							++pucCnt;
						}
						/* Reset EOL detect counter. */
						iState = 0;
					}
				}
				else
				{
					/* Wait for data. */
					pthread_mutex_lock(m_ptRxDataAvail_Mutex);
					iResult = pthread_cond_timedwait(m_ptRxDataAvail_Condition, m_ptRxDataAvail_Mutex, &tEndTime);
					pthread_mutex_unlock(m_ptRxDataAvail_Mutex);

					if( iResult==ETIMEDOUT )
					{
						/* Timeout, no more data arrived. */
						iResult = 0;
						break;
					}
					else if( iResult!=0 )
					{
						fprintf(stderr, "failed to wait for data available condition: %d\n", iResult);
						break;
					}
				}
			}
			else
			{
				/* Are more chars available? */
				if( pucCnt<ptCard->pucEnd )
				{
					if( *pucCnt==aucEol[iState] )
					{
						++sizProcessed;
						++pucCnt;
						++iState;
						if( aucEol[iState]==0 )
						{
							/* Ok, EOL found! */
							break;
						}
					}
					else
					{
						if( iState==0 )
						{
							++sizProcessed;
							++pucCnt;
						}
						/* Reset EOL detect counter. */
						iState = 0;
					}
				}
				else
				{
					ptCard = ptCard->ptNext;
					pucCnt = ptCard->aucData;
				}
			}
		}

		printf("iResult=%d, sizProcessed=%d\n", iResult, sizProcessed);
		if( iResult==0 && sizProcessed>0 )
		{
			uBuffer.pc = pcBuffer;
			sizReceived = usb_receive(uBuffer.puc, sizProcessed, 1);
			if( sizReceived==sizProcessed )
			{
				/* Remove linefeed from buffer. */
				sizReceived -= iState;
				uBuffer.puc[sizReceived] = 0;
			}
			*psizReceived = sizReceived;

			printf("Line: '%s'\n", uBuffer.pc);
		}
	}

	return iResult;
}
#endif


int romloader_usb_device_libusb0::usb_send(const char *pcBuffer, size_t sizBuffer)
{
	unsigned char *pucCnt;
	unsigned char *pucEnd;
	const unsigned int uiChunkTimeoutMs = 1000;
	int iChunk;
	int iTransfered;
	int iResult;


	/* Send the complete data block. */
	pucCnt = (unsigned char*)pcBuffer;
	pucEnd = (unsigned char*)pcBuffer + sizBuffer;
	while( pucCnt<pucEnd )
	{
		/* Limit the next data chunk to the maximum USB packet size of 64 bytes. */
		iChunk = pucEnd - pucCnt;
		if( iChunk>64 )
		{
			iChunk = 64;
		}

		/* Send the next data chunk. */
#if defined(WIN32)
		iResult = usb_bulk_write(m_ptDevHandle, m_ucEndpoint_Out, pcCnt, sizChunk, uiChunkTimeoutMs);
#else
		iResult = libusb_bulk_transfer(m_ptDevHandle, m_ucEndpoint_Out, pucCnt, iChunk, &iTransfered, uiChunkTimeoutMs);
#endif
		if( iResult==LIBUSB_SUCCESS || iResult==LIBUSB_ERROR_TIMEOUT )
		{
			if( iResult==LIBUSB_ERROR_TIMEOUT )
			{
				printf("Timeout, transfered %d bytes.\n", iTransfered);
				if( iTransfered==0 )
				{
					printf("Clearing halt\n");
					libusb_clear_halt(m_ptDevHandle, m_ucEndpoint_Out);
					printf("In Chars: 0x%08x\n", getCardSize());
					dump_all_cards();
					libusb_clear_halt(m_ptDevHandle, m_ucEndpoint_In);
				}
			}

			/* Transfer ok! */
			pucCnt += iTransfered;
			iResult = LIBUSB_SUCCESS;
#if !defined(WIN32)
			usleep(1000);
			sched_yield();
#endif
		}
		else
		{
			/* Transfer failed. */
			break;
		}
	}

	return iResult;
}


int romloader_usb_device_libusb0::read_data08(unsigned long ulNetxAddress, unsigned char *pucValue)
{
	int iResult;
	char acCommand[19];
	size_t sizCommand;
	unsigned long ulResponseAddress;
	unsigned long ulResponseValue;
	size_t sizOldData;
	size_t sizReceived;
	int iScanCnt;
	char acBuffer[82];
	const unsigned int uiCommandTimeoutMs = 100;


	ulResponseValue = 0;

	/* Construct the command. */
	sizCommand = snprintf(acCommand, sizeof(acCommand), "db %08lX ++1\r\n", ulNetxAddress);

	/* Flush any old data. */
	sizOldData = getCardSize();
	if( sizOldData!=0 )
	{
		fprintf(stderr, "Old data in card buffer left!\n");
		flushCards();
	}

	/* send the command */
	iResult = usb_send(acCommand, sizCommand);
	if( iResult!=LIBUSB_SUCCESS )
	{
		fprintf(stderr, "%s(%p): failed to send command: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}
	else
	{
		if( expect_string(acCommand)!=true )
		{
			fprintf(stderr, "%s(%p): strange response 1 from device\n", m_pcPluginId, this);
			iResult = -1;
		}
		else
		{
			iResult = usb_receive_line(acBuffer, sizeof(acBuffer), uiCommandTimeoutMs, &sizReceived);
			if( iResult!=LIBUSB_SUCCESS )
			{
				fprintf(stderr, "%s(%p): failed to receive response: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
			}
			else
			{
/*
				hexdump((const unsigned char*)acBuffer, sizReceived);
*/
				iScanCnt = sscanf(acBuffer, "%lx: %lx ", &ulResponseAddress, &ulResponseValue);
				if( iScanCnt!=2 )
				{
					fprintf(stderr, "%s(%p): strange response 2 from device\n", m_pcPluginId, this);
					iResult = -1;
				}
				else if( ulResponseAddress!=ulNetxAddress )
				{
					fprintf(stderr, "%s(%p): address does not match request\n", m_pcPluginId, this);
					iResult = -1;
				}
				else if( expect_string("Result: 0\r\n\r\n>")!=true )
				{
					fprintf(stderr, "%s(%p): Failed to get result line!\n", m_pcPluginId, this);
					iResult = -1;
				}
				else
				{
/*
					printf("%s(%p): read_data08: 0x%08lx = 0x%02lx\n", m_pcPluginId, this, ulNetxAddress, ulResponseValue);
*/
					*pucValue = (unsigned char)(ulResponseValue & 0xffU);
				}
			}
		}
	}

	return iResult;
}


int romloader_usb_device_libusb0::read_data32(unsigned long ulNetxAddress, unsigned long *pulValue)
{
	int iResult;
	char acCommand[19];
	size_t sizCommand;
	unsigned long ulResponseAddress;
	unsigned long ulResponseValue;
	size_t sizOldData;
	size_t sizReceived;
	int iScanCnt;
	union
	{
		char ac[52];
		unsigned char auc[52];
	} uResult;


	ulResponseValue = 0;

	/* Construct the command. */
	if( m_tChiptyp==ROMLOADER_CHIPTYP_NETX10 )
	{
		sizCommand = snprintf(acCommand, sizeof(acCommand), "d %08lX ++4\r\n", ulNetxAddress);
	}
	else
	{
		sizCommand = snprintf(acCommand, sizeof(acCommand), "DUMP %08lX LONG", ulNetxAddress);
	}

	/* Flush any old data. */
	sizOldData = getCardSize();
	if( sizOldData!=0 )
	{
		fprintf(stderr, "Old data in card buffer left!\n");
		flushCards();
	}

	/* send the command */
	iResult = usb_send(acCommand, sizCommand);
	if( iResult!=LIBUSB_SUCCESS )
	{
		fprintf(stderr, "%s(%p): failed to send command: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}
	else
	{
		if( m_tChiptyp==ROMLOADER_CHIPTYP_NETX10 && expect_string(acCommand)!=true )
		{
			fprintf(stderr, "%s(%p): strange response 1 from device\n", m_pcPluginId, this);
			iResult = -1;
		}
		else
		{
			/* FIXME: this should be something like a linebased receive routine. */
			sizReceived = usb_receive(uResult.auc, sizeof(uResult.auc), 200);
			if( sizReceived!=sizeof(uResult.auc) )
			{
				fprintf(stderr, "%s(%p): failed to receive %d bytes, got only %d\n", m_pcPluginId, this, sizeof(uResult.auc), sizReceived);
				hexdump(uResult.auc, sizReceived);
				iResult = -1;
			}
			else
			{
/*
				hexdump(uResult.auc, sizeof(uResult.auc));
*/
				iScanCnt = sscanf(uResult.ac, "%lx: %lx ", &ulResponseAddress, &ulResponseValue);
				if( iScanCnt!=2 )
				{
					fprintf(stderr, "%s(%p): strange response 2 from device\n", m_pcPluginId, this);
					iResult = -1;
				}
				else if( ulResponseAddress!=ulNetxAddress )
				{
					fprintf(stderr, "%s(%p): address does not match request\n", m_pcPluginId, this);
					iResult = -1;
				}
				else if( expect_string("Result: 0\r\n\r\n>")!=true )
				{
					fprintf(stderr, "%s(%p): Failed to get result line!\n", m_pcPluginId, this);
					iResult = -1;
				}
				else
				{
/*
					printf("%s(%p): read_data32: 0x%08lx = 0x%08lx\n", m_pcPluginId, this, ulNetxAddress, ulResponseValue);
*/
					*pulValue = ulResponseValue;
				}
			}
		}
	}

	return iResult;
}


int romloader_usb_device_libusb0::read_image(unsigned long ulNetxAddress, size_t sizData, unsigned char *pucData)
{
	size_t sizCommand;
	int iResult;
	char acCommand[28];


	sizCommand = snprintf(acCommand, sizeof(acCommand), "s %08lX ++%08lX\r\n", ulNetxAddress, sizData);

	/* send the command */
	iResult = usb_send(acCommand, sizCommand);
	if( iResult!=LIBUSB_SUCCESS )
	{
		fprintf(stderr, "%s(%p): failed to send command: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}
	else
	{
		if( expect_string(acCommand)!=true )
		{
			fprintf(stderr, "%s(%p): strange response 1 from device\n", m_pcPluginId, this);
			iResult = -1;
		}
		else
		{
			iResult = parse_uue(sizData, pucData, ulNetxAddress, ulNetxAddress+sizData);
			if( iResult!=0 )
			{
				fprintf(stderr, "%s(%p): failed to parse uue data.\n", m_pcPluginId, this);
			}
			else if( expect_string("Result: 0\r\n\r\n>")!=true )
			{
				fprintf(stderr, "%s(%p): Failed to get result line!\n", m_pcPluginId, this);
				iResult = -1;
			}
/*
			else
			{
				printf("%s(%p): read_image: 0x%08lx =\n", m_pcPluginId, this, ulNetxAddress);
				hexdump(pucData, sizData);
			}
*/
		}
	}

	return iResult;
}


int romloader_usb_device_libusb0::write_data32(unsigned long ulNetxAddress, unsigned long ulData)
{
	int iResult;
	char acCommand[28];
	size_t sizCommand;
	size_t sizOldData;


	/* Construct the command. */
	if( m_tChiptyp==ROMLOADER_CHIPTYP_NETX10 )
	{
		sizCommand = snprintf(acCommand, sizeof(acCommand), "m %08lX %08X\r\n", ulNetxAddress, ulData);
	}
	else
	{
		sizCommand = snprintf(acCommand, sizeof(acCommand), "FILL %08lX %08X LONG", ulNetxAddress, ulData);
	}

	/* Flush any old data. */
	sizOldData = getCardSize();
	if( sizOldData!=0 )
	{
		fprintf(stderr, "Old data in card buffer left!\n");
		flushCards();
	}

	/* send the command */
	iResult = usb_send(acCommand, sizCommand);
	if( iResult!=LIBUSB_SUCCESS )
	{
		fprintf(stderr, "%s(%p): failed to send command: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}
	else
	{
		if( m_tChiptyp==ROMLOADER_CHIPTYP_NETX10 && expect_string(acCommand)!=true )
		{
			fprintf(stderr, "%s(%p): strange response 1 from device\n", m_pcPluginId, this);
			iResult = -1;
		}
		else if( expect_string("Result: 0\r\n\r\n>")!=true )
		{
			fprintf(stderr, "%s(%p): Failed to get result line!\n", m_pcPluginId, this);
			iResult = -1;
		}
/*
		else
		{
			printf("%s(%p): write_data32: 0x%08lx = 0x%08x\n", m_pcPluginId, this, ulNetxAddress, ulData);
		}
*/
	}

	return iResult;
}


int romloader_usb_device_libusb0::write_image(unsigned long ulNetxAddress, const unsigned char *pucData, size_t sizData)
{
	int iResult;
	char *pcUueData;
	size_t sizUueData;
	char acCommand[16];
	size_t sizCommand;
	size_t sizOldData;


	/* Generate the command. */
	sizCommand = snprintf(acCommand, sizeof(acCommand), "l %08lx\r\n", ulNetxAddress);

	/* Flush any old data. */
	sizOldData = getCardSize();
	if( sizOldData!=0 )
	{
		fprintf(stderr, "Old data in card buffer left!\n");
		flushCards();
	}

	/* Send the command. */
	iResult = usb_send(acCommand, sizCommand);
	if( iResult!=LIBUSB_SUCCESS )
	{
		fprintf(stderr, "%s(%p): failed to send command: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}
	else if( expect_string(acCommand)!=true )
	{
		fprintf(stderr, "%s(%p): strange response 1 from device\n", m_pcPluginId, this);
		iResult = -1;
	}
	else
	{
		pcUueData = NULL;
		sizUueData = 0;
		iResult = uue_generate(pucData, sizData, &pcUueData, &sizUueData);
		if( iResult==0 )
		{
			/* Send the complete data block. */
			iResult = usb_send(pcUueData, sizUueData);
			printf("done: %d\n", iResult);
			free(pcUueData);

			if( iResult!=0 )
			{
				fprintf(stderr, "%s(%p): Failed to send uue data: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
			}
			else
			{
				if( expect_string("Result: 0\r\n\r\n>")!=true )
				{
					fprintf(stderr, "%s(%p): Failed to get result line!\n", m_pcPluginId, this);
					iResult = -1;
				}
/*
				else
				{
					printf("%s(%p): write_image: 0x%08lx =\n", m_pcPluginId, this, ulNetxAddress);
					hexdump(pucData, sizData);
				}
*/
			}
		}
	}

	return iResult;
}


int romloader_usb_device_libusb0::call(unsigned long ulNetxAddress, unsigned long ulR0)
{
	size_t sizCommand;
	int iResult;
	char acCommand[28];
	unsigned char aucOutput[64];
	size_t sizReceived;
	bool fEndFound;
	const char *pcEndPattern = "\r\nResult: 0x********\r\nResult: 0\r\n\r\n>";
	int iState;
	const size_t sizWarpBackFrom = 31;
	const size_t sizWarpBackTo = 11;
	size_t sizCnt;
	char cPattern;
	char cReceived;
	unsigned long ulResultValue;
	int iMatch;


	sizCommand = snprintf(acCommand, sizeof(acCommand), "g %08lX %08lX\r\n", ulNetxAddress, ulR0);

	/* send the command */
	iResult = usb_send(acCommand, sizCommand);
	if( iResult!=LIBUSB_SUCCESS )
	{
		fprintf(stderr, "%s(%p): failed to send command: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}
	else
	{
		if( expect_string(acCommand)!=true )
		{
			fprintf(stderr, "%s(%p): strange response 1 from device\n", m_pcPluginId, this);
			iResult = -1;
		}
		else
		{
			/* Capture output. */
			fEndFound = false;
			iState = 0;
			ulResultValue = 0;
			do
			{
				sizReceived = usb_receive(aucOutput, sizeof(aucOutput), 200);
				if( sizReceived!=0 )
				{
//					hexdump(aucOutput, sizReceived);
					fwrite(aucOutput, sizReceived, 1, stdout);

					/* Loop over the complete input and look for the end pattern. */
					sizCnt = 0;
					do
					{
						iMatch = 0;
						cPattern = pcEndPattern[iState];
						cReceived = (char)aucOutput[sizCnt];
						if( cPattern=='*' )
						{
							cReceived = tolower(cReceived);
							ulResultValue <<= 4;
							/* The received digit must be a hex digit. */
							if( cReceived>='0' && cReceived<='9' )
							{
//								printf("%c\n", cReceived);
								cReceived &= 0x0fU;
								ulResultValue |= cReceived;
								iMatch = 1;
							}
							else if( cReceived>='a' && cReceived<='f' )
							{
//								printf("%c\n", cReceived);
								cReceived -= 'a';
								cReceived += 10;
								ulResultValue |= cReceived;
								iMatch = 1;
							}
						}
						else if( cPattern==cReceived )
						{
//							printf("%c\n", cReceived);
							iMatch = 1;
						}

						if( iMatch==0 )
						{
//							printf("\nNo match %02x!=%02x\n", cReceived, cPattern);
							/* Go back. */
							if( iState==sizWarpBackFrom )
							{
								iState = sizWarpBackTo;
							}
							else if( iState!=0 )
							{
								iState = 0;
							}
							else
							{
								++sizCnt;
							}
						}
						else
						{
							++sizCnt;
							++iState;
						}

						if( pcEndPattern[iState]=='\0' )
						{
//							printf("\nEOP!\n");
							fEndFound = true;
							break;
						}
					} while( sizCnt<sizReceived );
//					printf("\n");
				}
			} while( fEndFound==false );
		}
	}

	return iResult;
}

