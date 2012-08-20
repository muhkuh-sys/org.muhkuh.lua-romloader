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
#include <stddef.h>
#include <stdio.h>

#include "romloader_usb_device_libusb.h"
#include "../machine_interface/netx/src/monitor_commands.h"

#include <stdlib.h>

#if defined(_MSC_VER)
#	define snprintf _snprintf
#	define SLEEP_MS(ms) Sleep(ms)
#else
#	include <sys/time.h>
#	include <unistd.h>
#	define SLEEP_MS(ms) usleep(ms*1000)
#endif

#include "romloader_usb_main.h"
#include "../uuencoder.h"

#include "netx/targets/usbmon_netx10.h"
#include "netx/targets/usbmon_netx56.h"
#include "netx/targets/usbmon_netx500.h"


#if defined(WIN32)
	const char *romloader_usb_device_libusb::m_pcLibUsb_BusPattern = "bus-%u";
	const char *romloader_usb_device_libusb::m_pcLibUsb_DevicePattern = "\\\\.\\libusb0-%u";
#else
	const char *romloader_usb_device_libusb::m_pcLibUsb_BusPattern = "%u";
	const char *romloader_usb_device_libusb::m_pcLibUsb_DevicePattern = "%u";
#endif



#if 0 /* defined(WIN32) */

/*-------------------------------------
  All calls to libusb functions (usb_*) are re-routed via pointers (pfn_usb_*) 
  which point to the libusb functions if it was loaded successfully, or 
  to dummy functions.

  romloader_usb_provider::romloader_usb_provider  calls libusb_load
  romloader_usb_provider::~romloader_usb_provider calls libusb_unload
  int romloader_usb_provider::DetectInterfaces    calls libusb_isloaded
  -------------------------------------*/

/* global pointers to the used libusb functions */

void            (* pfn_usb_init)(void);
usb_dev_handle *(* pfn_usb_open)(struct usb_device *dev);
int             (* pfn_usb_close)(usb_dev_handle *dev);
int             (* pfn_usb_reset)(usb_dev_handle *dev);
struct usb_bus *(* pfn_usb_get_busses)(void);
int             (* pfn_usb_find_busses)(void);
int             (* pfn_usb_find_devices)(void);
int             (* pfn_usb_get_descriptor)(usb_dev_handle *udev, unsigned char type, unsigned char index, void *buf, int size);
int             (* pfn_usb_claim_interface)(usb_dev_handle *dev, int _interface);
int             (* pfn_usb_release_interface)(usb_dev_handle *dev, int _interface);
int             (* pfn_usb_set_configuration)(usb_dev_handle *dev, int configuration);
int             (* pfn_usb_bulk_write)(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);
int             (* pfn_usb_bulk_read)(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);

/* non-NULL if the dll has been loaded and the function pointers have been set */ 
HINSTANCE hLibusb;

/* dummy functions */
void             no_usb_init(void)                                                                  {}
usb_dev_handle * no_usb_open(struct usb_device *dev)                                                {return NULL;}
int              no_usb_close(usb_dev_handle *dev)                                                  {return LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_reset(usb_dev_handle *dev)                                                  {return LIBUSB_ERROR_NOT_SUPPORTED;}
struct usb_bus * no_usb_get_busses(void)                                                            {return NULL;}
int              no_usb_find_busses(void)                                                           {return LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_find_devices(void)                                                          {return LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_get_descriptor(usb_dev_handle *udev, unsigned char type, unsigned char index, void *buf, int size) {return  LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_claim_interface(usb_dev_handle *dev, int _interface)                        {return LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_release_interface(usb_dev_handle *dev, int _interface)                      {return LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_set_configuration(usb_dev_handle *dev, int configuration)                   {return LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_bulk_write(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout) {return LIBUSB_ERROR_NOT_SUPPORTED;}
int              no_usb_bulk_read(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout)  {return LIBUSB_ERROR_NOT_SUPPORTED;}


/* auxiliary structures for reading the symbols from the dll */
typedef struct 
{
	const char *pstrFnName;
	void** ppFnPtr;
	void* pDefaultFnPtr;
} T_DLL_SYMBOL_ENTRY;

T_DLL_SYMBOL_ENTRY atLibusbSymbols[] = {
	{"usb_init"              , (void**) &pfn_usb_init,                 (void*) no_usb_init},             
	{"usb_open"              , (void**) &pfn_usb_open,                 (void*) no_usb_open},             
	{"usb_close"             , (void**) &pfn_usb_close,                (void*) no_usb_close},            
	{"usb_reset"             , (void**) &pfn_usb_reset,                (void*) no_usb_reset},            
	{"usb_get_busses"        , (void**) &pfn_usb_get_busses,           (void*) no_usb_get_busses},       
	{"usb_find_busses"       , (void**) &pfn_usb_find_busses,          (void*) no_usb_find_busses},      
	{"usb_find_devices"      , (void**) &pfn_usb_find_devices,         (void*) no_usb_find_devices},     
	{"usb_get_descriptor"    , (void**) &pfn_usb_get_descriptor,       (void*) no_usb_get_descriptor},   
	{"usb_claim_interface"   , (void**) &pfn_usb_claim_interface,      (void*) no_usb_claim_interface},  
	{"usb_release_interface" , (void**) &pfn_usb_release_interface,    (void*) no_usb_release_interface},
	{"usb_set_configuration" , (void**) &pfn_usb_set_configuration,    (void*) no_usb_set_configuration},
	{"usb_bulk_write"        , (void**) &pfn_usb_bulk_write,           (void*) no_usb_bulk_write},       
	{"usb_bulk_read"         , (void**) &pfn_usb_bulk_read,            (void*) no_usb_bulk_read},        
};

#define ARRAY_ENDADDR(arrayname) arrayname + (sizeof(arrayname)/sizeof(arrayname[0]))

void libusb_setDummyFunctions();

bool libusb_load()
{
	hLibusb = LoadLibrary("libusb0");
	if (hLibusb == NULL)
	{
		libusb_setDummyFunctions();
		printf("romloader_usb: libusb not found. netX500/100 USB devices will not be available\n");
	}
	else
	{
		printf("romloader_usb: libusb loaded.\n");

		T_DLL_SYMBOL_ENTRY *ptPfnEntry = atLibusbSymbols;
		T_DLL_SYMBOL_ENTRY *ptPfnTableEnd = ARRAY_ENDADDR(atLibusbSymbols);
		void* pvFn = NULL;

		while (ptPfnEntry < ptPfnTableEnd)
		{
			pvFn = (void*)GetProcAddress(hLibusb, ptPfnEntry->pstrFnName);
			*(ptPfnEntry->ppFnPtr) = pvFn;

			if (pvFn == NULL)
			{
				fprintf(stderr, "romloader_usb: Symbol %s not found in libusb.\n", ptPfnEntry->pstrFnName);
				break;
			}

			ptPfnEntry++;
		}

		/* 
			If pvFn is NULL, a symbol was not found. 
			In this case, unload the library and set dummy functions 
		*/
		if (pvFn == NULL)
		{
			FreeLibrary(hLibusb);
			hLibusb = NULL;
			libusb_setDummyFunctions();
			fprintf(stderr, "romloader_usb: netX500/100 USB devices will not be available.\n");
		}
	}

	return ( hLibusb != NULL );
}


void libusb_setDummyFunctions()
{
	T_DLL_SYMBOL_ENTRY *ptPfnEntry = atLibusbSymbols;
	T_DLL_SYMBOL_ENTRY *ptPfnTableEnd = ARRAY_ENDADDR(atLibusbSymbols);
	while (ptPfnEntry < ptPfnTableEnd)
	{
		*ptPfnEntry->ppFnPtr = ptPfnEntry->pDefaultFnPtr;
		ptPfnEntry++;
	}
}

void libusb_unload()
{
	if (hLibusb != NULL) 
	{
		FreeLibrary(hLibusb);
		hLibusb = NULL;
		printf("romloader_usb: libusb unloaded.\n");
	}

	libusb_setDummyFunctions();
}

bool libusb_isloaded()
{
	return (hLibusb != NULL);
}

#else

bool libusb_load() {return true;}
bool libusb_isloaded() {return true;}
void libusb_unload() {}

#define  pfn_usb_init               usb_init
#define  pfn_usb_open               usb_open
#define  pfn_usb_close              usb_close
#define  pfn_usb_reset              usb_reset
#define  pfn_usb_get_busses         usb_get_busses
#define  pfn_usb_find_busses        usb_find_busses
#define  pfn_usb_find_devices       usb_find_devices
#define  pfn_usb_get_descriptor     usb_get_descriptor
#define  pfn_usb_claim_interface    usb_claim_interface
#define  pfn_usb_release_interface  usb_release_interface
#define  pfn_usb_set_configuration  usb_set_configuration
#define  pfn_usb_bulk_write         usb_bulk_write
#define  pfn_usb_bulk_read          usb_bulk_read

#endif

/*-------------------------------------*/

#if ROMLOADER_USB_LIBUSB_VERSION==0
unsigned char libusb_get_bus_number(libusb_device *dev)
{
	return (unsigned char)(dev->bus->location);
}
unsigned char libusb_get_device_address(libusb_device *dev)
{
	return dev->devnum;
}

#define libusb_device_descriptor usb_device_descriptor
int libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc)
{
	memcpy(desc, &(dev->descriptor), sizeof(struct libusb_device_descriptor));
	return LIBUSB_SUCCESS;
}

#define libusb_get_descriptor pfn_usb_get_descriptor

int libusb_init(libusb_context **ctx)
{
	pfn_usb_init();
	*ctx = (libusb_context*)1;
	return LIBUSB_SUCCESS;
}

void libusb_exit(libusb_context *ctx)
{
}

void libusb_set_debug(libusb_context *ctx, int level)
{
}

#define libusb_reset_device pfn_usb_reset
#define libusb_close pfn_usb_close

int libusb_open(libusb_device *ptDevice, libusb_device_handle **pptDevHandle)
{
	libusb_device_handle *ptDevHandle;
	int iError;


	ptDevHandle = pfn_usb_open(ptDevice);
	if( ptDevHandle!=NULL )
	{
		*pptDevHandle = ptDevHandle;
		iError = LIBUSB_SUCCESS;
	}
	else
	{
		*pptDevHandle = NULL;
		iError = LIBUSB_ERROR_INVALID_PARAM;
	}

	return iError;
}


#define libusb_claim_interface pfn_usb_claim_interface
#define libusb_release_interface pfn_usb_release_interface

#define libusb_set_configuration pfn_usb_set_configuration

libusb_device *libusb_ref_device(libusb_device *dev)
{
	return dev;
}

void libusb_unref_device(libusb_device *dev)
{
}

int libusb_bulk_transfer(libusb_device_handle *ptDevHandle, unsigned char ucEndPoint, unsigned char *pucData, int iLength, int *piProcessed, unsigned int uiTimeoutMs)
{
	int iError;


	if( ucEndPoint>=0x80 )
	{
		iError = pfn_usb_bulk_read(ptDevHandle, ucEndPoint, (char*)pucData, iLength, uiTimeoutMs);
	}
	else
	{
		iError = pfn_usb_bulk_write(ptDevHandle, ucEndPoint, (char*)pucData, iLength, uiTimeoutMs);
	}
	if( iError>=0 )
	{
		/* transfer ok! */
		if( piProcessed!=NULL )
		{
			*piProcessed = iError;
		}
		iError = LIBUSB_SUCCESS;
	}
	else
	{
		/* transfer failed */
		if( piProcessed!=NULL )
		{
			*piProcessed = 0;
		}
	}

	return iError;
}

ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device ***list)
{
	ssize_t sizDevices;
	ssize_t sizDeviceCnt;
        struct usb_bus *ptBusses;
        struct usb_bus *ptBus;
        struct usb_device *ptDev;
	libusb_device **pptDeviceList;

	pfn_usb_find_busses();
	pfn_usb_find_devices();

	ptBusses = pfn_usb_get_busses();

	/* Count devices. */
	sizDevices = 0;

	/* Loop over all busses. */
	ptBus = ptBusses;
	while( ptBus!=NULL )
	{
		/* loop over all devices */
		ptDev = ptBus->devices;
		while( ptDev!=NULL )
		{
			++sizDevices;
			/* next device */
			ptDev = ptDev->next;
		}
		/* scan next bus */
		ptBus = ptBus->next;
	}

	/* Allocate a list for all devices. */
	pptDeviceList = (libusb_device**)malloc(sizDevices*sizeof(libusb_device**));

	/* Loop over all busses. */
	sizDeviceCnt = 0;
	ptBus = ptBusses;
	while( ptBus!=NULL )
	{
		/* loop over all devices */
		ptDev = ptBus->devices;
		while( ptDev!=NULL )
		{
			pptDeviceList[sizDeviceCnt] = ptDev;
			++sizDeviceCnt;
			/* next device */
			ptDev = ptDev->next;
		}
		/* scan next bus */
		ptBus = ptBus->next;
	}

	*list = pptDeviceList;
	return sizDevices;
}

void libusb_free_device_list(libusb_device **list, int unref_devices)
{
	free(list);
}

enum libusb_descriptor_type
{
	LIBUSB_DT_DEVICE = 0x01,
	LIBUSB_DT_CONFIG = 0x02,
	LIBUSB_DT_STRING = 0x03,
	LIBUSB_DT_INTERFACE = 0x04,
	LIBUSB_DT_ENDPOINT = 0x05,
	LIBUSB_DT_HID = 0x21,
	LIBUSB_DT_REPORT = 0x22,
	LIBUSB_DT_PHYSICAL = 0x23,
	LIBUSB_DT_HUB = 0x29
};

#endif


romloader_usb_device_libusb::romloader_usb_device_libusb(const char *pcPluginId)
 : romloader_usb_device(pcPluginId)
// , m_tDeviceId({0})
// , m_ucEndpoint_In(0)
// , m_ucEndpoint_Out(0)
 , m_ptLibUsbContext(NULL)
 , m_ptDevHandle(NULL)
{
	printf("romloader_usb_device_libusb\n");
	memset(&m_tDeviceId, 0, sizeof(NETX_USB_DEVICE_T));

	libusb_init(&m_ptLibUsbContext);
	libusb_set_debug(m_ptLibUsbContext, 3);
}


romloader_usb_device_libusb::~romloader_usb_device_libusb(void)
{
	printf("~romloader_usb_device_libusb\n");
	Disconnect();

	if( m_ptLibUsbContext!=NULL )
	{
		libusb_exit(m_ptLibUsbContext);
	}
}


const char *romloader_usb_device_libusb::m_pcPluginNamePattern = "romloader_usb_%02x_%02x";


int romloader_usb_device_libusb::libusb_reset_and_close_device(void)
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


int romloader_usb_device_libusb::detect_interfaces(romloader_usb_reference ***ppptReferences, size_t *psizReferences, romloader_usb_provider *ptProvider)
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
	const NETX_USB_DEVICE_T *ptId;


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
			fprintf(stderr, "%s(%p): failed to detect usb devices: %d:%s\n", m_pcPluginId, this, ssizDevList, libusb_strerror(ssizDevList));
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
				ptId = identifyDevice(ptDev);
				if( ptId!=NULL )
				{
					/* construct the name */
					uiBusNr = libusb_get_bus_number(ptDev);
					uiDevAdr = libusb_get_device_address(ptDev);
					snprintf(acName, sizMaxName-1, m_pcPluginNamePattern, uiBusNr, uiDevAdr);
/* TODO: replace this with setup_device?
 * It is the same open/set_config/claim, except that here no error is printed if BUSY.
 */
					iResult = setup_netx_device(ptDev, ptId);
					if( iResult!=LIBUSB_SUCCESS && iResult!=LIBUSB_ERROR_BUSY /* && iResult!=LIBUSB_ERROR_SYS_BUSY */ )
					{
						/* Failed to open the interface, do not add it to the list. */
						fprintf(stderr, "%s(%p): failed to setup the device %s: %d:%s\n", m_pcPluginId, this, acName, iResult, libusb_strerror(iResult));
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
							iResult = libusb_release_interface(m_ptDevHandle, ptId->ucInterface);
							if( iResult!=LIBUSB_SUCCESS )
							{
								/* failed to release the interface */
								fprintf(stderr, "%s(%p): failed to release interface %d of device %s after a successful claim: %d:%s\n", m_pcPluginId, this, ptId->ucInterface, acName, iResult, libusb_strerror(iResult));
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


libusb_device *romloader_usb_device_libusb::find_netx_device(libusb_device **ptDeviceList, ssize_t ssizDevList, unsigned int uiBusNr, unsigned int uiDeviceAdr)
{
	libusb_device **ptDevCnt;
	libusb_device **ptDevEnd;
	libusb_device *ptDev;
	libusb_device *ptNetxDevice;


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
			ptNetxDevice = ptDev;
			break;
		}

		++ptDevCnt;
	}

	return ptNetxDevice;
}


int romloader_usb_device_libusb::setup_netx_device(libusb_device *ptNetxDevice, const NETX_USB_DEVICE_T *ptId)
{
	int iResult;


	printf("romloader_usb_device_libusb::setup_netx_device(): ptNetxDevice=%p, ptId=%p", ptNetxDevice, ptId);

	iResult = libusb_open(ptNetxDevice, &m_ptDevHandle);
	if( iResult!=LIBUSB_SUCCESS )
	{
		fprintf(stderr, "%s(%p): failed to open the device: %d:%s", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}
	else
	{
		/* Set the configuration. */
		/* Do not set the configuration for a combo device. The composite interface is owned by the composite device driver.
		 * The good thing is: the configuration is already set.
		 */
		if( ptId->ucConfiguration!=0 )
		{
//			printf("%s(%p): set device configuration to %d.\n", m_pcPluginId, this, ptId->ucConfiguration);
			iResult = libusb_set_configuration(m_ptDevHandle, ptId->ucConfiguration);
		}
		if( iResult!=LIBUSB_SUCCESS )
		{
			/* failed to set the configuration */
			fprintf(stderr, "%s(%p): failed to set the configuration %d of device: %d:%s\n", m_pcPluginId, this, ptId->ucConfiguration, iResult, libusb_strerror(iResult));
		}
		else
		{
			/* Claim the interface. */
//			printf("%s(%p): claim interface %d.\n", m_pcPluginId, this, ptId->ucInterface);
			iResult = libusb_claim_interface(m_ptDevHandle, ptId->ucInterface);
			/* Do not print an error message for busy devices. This will be done above. */
			if( iResult!=LIBUSB_SUCCESS && iResult!=LIBUSB_ERROR_BUSY )
			{
				/* Failed to claim the interface. */
				fprintf(stderr, "%s(%p): failed to claim interface %d: %d:%s\n", m_pcPluginId, this, ptId->ucInterface, iResult, libusb_strerror(iResult));
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


int romloader_usb_device_libusb::Connect(unsigned int uiBusNr, unsigned int uiDeviceAdr)
{
	int iResult;
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device *ptUsbDevice;
	libusb_device *ptUpdatedNetxDevice;
	const NETX_USB_DEVICE_T *ptId;


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
			ptId = identifyDevice(ptUsbDevice);
			if( ptId==NULL )
			{
				fprintf(stderr, "%s(%p): The device could not be identified as a netX.\n", m_pcPluginId, this);
				iResult = LIBUSB_ERROR_NOT_FOUND;
			}
			else
			{
				/* Copy all data of the chip id. */
				memcpy(&m_tDeviceId, ptId, sizeof(NETX_USB_DEVICE_T));

				/* Reference the device before freeing the list. */
				libusb_ref_device(ptUsbDevice);
				iResult = LIBUSB_SUCCESS;
			}
		}

		/* free the device list */
		libusb_free_device_list(ptDeviceList, 1);

		if( iResult==LIBUSB_SUCCESS )
		{
			/* NOTE: hier sollte eine generelle Entscheidung rein, ob das Gerät geupdated werden soll. */
			
			
			
			/* Does this device need an update? */
			iResult = LIBUSB_ERROR_OTHER;
			switch(m_tDeviceId.tCommandSet)
			{
			case ROMLOADER_COMMANDSET_UNKNOWN:
				/* No update plan for an unknown device. */
				iResult = LIBUSB_ERROR_OTHER;
				break;

			case ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1:
			case ROMLOADER_COMMANDSET_MI1:
				iResult = update_old_netx_device(ptUsbDevice, &ptUpdatedNetxDevice);
				if( iResult==LIBUSB_SUCCESS )
				{
					/* Use the updated device. */
					ptUsbDevice = ptUpdatedNetxDevice;
				}
				break;

			case ROMLOADER_COMMANDSET_MI2:
				/* The device uses the hboot v3 protocol. */
				iResult = LIBUSB_SUCCESS;
				break;
			}

			if( iResult==LIBUSB_SUCCESS )
			{
				iResult = setup_netx_device(ptUsbDevice, ptId);
				if( iResult==LIBUSB_ERROR_BUSY /* || iResult==LIBUSB_ERROR_SYS_BUSY */ )
				{
					fprintf(stderr, "%s(%p): the device is busy. Maybe some other program is accessing it right now.\n", m_pcPluginId, this);
					libusb_unref_device(ptUsbDevice);
				}
				else if( iResult!=LIBUSB_SUCCESS )
				{
					fprintf(stderr, "%s(%p): failed to setup the device, trying to reset it.\n", m_pcPluginId, this);

					/* Reset the device and try again. */
					/* FIXME: setup_netx_device already closed the device. */
					iResult = libusb_reset_and_close_device();
					if( iResult!=LIBUSB_SUCCESS )
					{
						fprintf(stderr, "%s(%p): failed to reset the netx, giving up: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
						libusb_release_interface(m_ptDevHandle, ptId->ucInterface);
						libusb_close(m_ptDevHandle);
						m_ptDevHandle = NULL;
					}
					else
					{
						fprintf(stderr, "%s(%p): reset ok!\n", m_pcPluginId, this);

						iResult = setup_netx_device(ptUsbDevice, ptId);
						if( iResult==LIBUSB_ERROR_BUSY /* || iResult==LIBUSB_ERROR_SYS_BUSY */ )
						{
							fprintf(stderr, "%s(%p): the device is busy after the reset. Maybe some other program is accessing it right now.\n", m_pcPluginId, this);
						}
						else if( iResult!=LIBUSB_SUCCESS )
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
	}

	printf("-Connect(): iResult=%d\n", iResult);

	return iResult;
}


void romloader_usb_device_libusb::Disconnect(void)
{
	if( m_ptDevHandle!=NULL )
	{
		libusb_release_interface(m_ptDevHandle, m_tDeviceId.ucInterface);
		libusb_close(m_ptDevHandle);
		m_ptDevHandle = NULL;
	}
	m_ptDevHandle = NULL;
}


const romloader_usb_device::NETX_USB_DEVICE_T *romloader_usb_device_libusb::identifyDevice(libusb_device *ptDevice) const
{
	const NETX_USB_DEVICE_T *ptDevHit;
	int iResult;
	struct libusb_device_descriptor sDevDesc;
	const NETX_USB_DEVICE_T *ptDevCnt;
	const NETX_USB_DEVICE_T *ptDevEnd;
	libusb_device_handle *ptDevHandle;


	ptDevHit = NULL;
	if( ptDevice!=NULL )
	{
		/* Try to open the device. */
		iResult = libusb_open(ptDevice, &ptDevHandle);
		if( iResult==LIBUSB_SUCCESS )
		{
			iResult = libusb_get_descriptor(ptDevHandle, LIBUSB_DT_DEVICE, 0, (unsigned char*)&sDevDesc, sizeof(struct libusb_device_descriptor));
			if( iResult==sizeof(struct libusb_device_descriptor) )
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
						printf("identifyDevice: Found device %04x:%04x:%04x\n", sDevDesc.idVendor, sDevDesc.idProduct, sDevDesc.bcdDevice);
						ptDevHit = ptDevCnt;
						break;
					}
					else
					{
						++ptDevCnt;
					}
				}
			}
			libusb_close(ptDevHandle);
		}
	}

	return ptDevHit;
}


const romloader_usb_device_libusb::LIBUSB_STRERROR_T romloader_usb_device_libusb::atStrError[14] =
{
	{ LIBUSB_SUCCESS,               "success" },
	{ LIBUSB_ERROR_IO,              "input/output error" },
	{ LIBUSB_ERROR_INVALID_PARAM,   "invalid parameter" },
	{ LIBUSB_ERROR_ACCESS,          "access denied (insufficient permissions)" },
	{ LIBUSB_ERROR_NO_DEVICE,       "no such device (it may have been disconnected)" },
	{ LIBUSB_ERROR_NOT_FOUND,       "entity not found" },
	{ LIBUSB_ERROR_BUSY,            "resource busy" },
	{ LIBUSB_ERROR_TIMEOUT,         "operation timed out" },
	{ LIBUSB_ERROR_OVERFLOW,        "overflow" },
	{ LIBUSB_ERROR_PIPE,            "pipe error" },
	{ LIBUSB_ERROR_INTERRUPTED,     "system call interrupted (perhaps due to signal)" },
	{ LIBUSB_ERROR_NO_MEM,          "insufficient memory" },
	{ LIBUSB_ERROR_NOT_SUPPORTED,   "operation not supported or unimplemented on this platform" },
	{ LIBUSB_ERROR_OTHER,           "other error" }
};


const char *romloader_usb_device_libusb::libusb_strerror(int iError)
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


int romloader_usb_device_libusb::netx10_discard_until_timeout(libusb_device_handle *ptDevHandle)
{
	unsigned char aucBuffer[64];
	int iResult;
	int iProcessed;
	unsigned int uiTimeoutMs;


	uiTimeoutMs = 100;

	do
	{
		iProcessed=0;
		iResult = libusb_bulk_transfer(ptDevHandle, m_tDeviceId.ucEndpoint_In, aucBuffer, sizeof(aucBuffer), &iProcessed, uiTimeoutMs);
		if( iResult==LIBUSB_ERROR_TIMEOUT )
		{
			/* Timeouts are great! */
			iResult = 0;
			break;
		}
		else if( iResult!=0 )
		{
			fprintf(stderr, "%s(%p): Failed to receive data: (%d)%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
			break;
		}
	} while( iResult==0 );

	return iResult;
}


int romloader_usb_device_libusb::netx500_exchange_data(libusb_device_handle *ptDevHandle, const unsigned char *pucOutBuffer, unsigned char *pucInBuffer)
{
	int iResult;
	int iProcessed;
	const unsigned int uiTimeoutMs = 100;
	/* The packet size is fixed to 64 bytes in this protocol. */
	const int iPacketSize = 64;


	iResult = libusb_bulk_transfer(ptDevHandle, m_tDeviceId.ucEndpoint_Out, (unsigned char*)pucOutBuffer, iPacketSize, &iProcessed, uiTimeoutMs);
	if( iResult==0 )
	{
		iResult = libusb_bulk_transfer(ptDevHandle, m_tDeviceId.ucEndpoint_In, pucInBuffer, iPacketSize, &iProcessed, uiTimeoutMs);
	}

	return iResult;
}


int romloader_usb_device_libusb::netx500_discard_until_timeout(libusb_device_handle *ptDevHandle)
{
	unsigned char aucOutBuffer[64] = {0};
	unsigned char aucInBuffer[64] = {0};
	int iResult;


	/* Throw away all data until the end. */
	do
	{
		iResult = netx500_exchange_data(ptDevHandle, aucOutBuffer, aucInBuffer);
		if( iResult!=LIBUSB_SUCCESS )
		{
			/* Error! */
			break;
		}
//		printf("discard packet:\n");
//		hexdump(aucInBuffer, 64);
	} while( aucInBuffer[0]!=0 );

	return iResult;
}


int romloader_usb_device_libusb::netx500_load_code(libusb_device_handle *ptDevHandle, const unsigned char* pucNetxCode, size_t sizNetxCode)
{
	unsigned short usCrc;
	size_t sizLine;
	unsigned long ulLoadAddress;
	int iResult;
	const unsigned char *pucDataCnt;
	const unsigned char *pucDataEnd;
	unsigned char aucOutBuffer[64] = {0};
	unsigned char aucInBuffer[64] = {0};
	size_t sizChunkSize;


	/* Be optimistic. */
	iResult = 0;

	if( pucNetxCode[0x00]!='m' || pucNetxCode[0x01]!='o' || pucNetxCode[0x02]!='o' || pucNetxCode[0x03]!='h' )
	{
		fprintf(stderr, "%s(%p): Invalid netx code, header missing.\n", m_pcPluginId, this);
		iResult = -1;
	}
	else
	{
		/* Get the load address. */
		ulLoadAddress  =  (unsigned long)(pucNetxCode[0x04]);
		ulLoadAddress |= ((unsigned long)(pucNetxCode[0x05])) <<  8U;
		ulLoadAddress |= ((unsigned long)(pucNetxCode[0x06])) << 16U;
		ulLoadAddress |= ((unsigned long)(pucNetxCode[0x07])) << 24U;

//		printf("Load 0x%08x bytes to 0x%08x.\n", sizNetxCode, ulLoadAddress);

		/* Generate crc16 checksum. */
		usCrc = crc16(pucNetxCode, sizNetxCode);

		/* Generate load command. */
		sizLine = snprintf((char*)(aucOutBuffer+1), sizeof(aucOutBuffer), "load %lx %x %04X\n", ulLoadAddress, sizNetxCode, usCrc);
		/* Set the length. */
		aucOutBuffer[0] = (unsigned char)(sizLine + 1);

		/* Send the command. */
		iResult = netx500_exchange_data(ptDevHandle, aucOutBuffer, aucInBuffer);
		if( iResult==LIBUSB_SUCCESS )
		{
			/* Terminate the command. */
			aucOutBuffer[0] = 0;
			iResult = netx500_exchange_data(ptDevHandle, aucOutBuffer, aucInBuffer);
			if( iResult==LIBUSB_SUCCESS )
			{
				/* Now send the data part. */
				pucDataCnt = pucNetxCode;
				pucDataEnd = pucNetxCode + sizNetxCode;
				while( pucDataCnt<pucDataEnd )
				{
					/* Get the size of the next data chunk. */
					sizChunkSize = pucDataEnd - pucDataCnt;
					if( sizChunkSize>63 )
					{
						sizChunkSize = 63;
					}
					/* Copy data to the packet. */
					memcpy(aucOutBuffer+1, pucDataCnt, sizChunkSize);
					aucOutBuffer[0] = (unsigned char)(sizChunkSize+1);

					iResult = netx500_exchange_data(ptDevHandle, aucOutBuffer, aucInBuffer);
					if( iResult!=LIBUSB_SUCCESS )
					{
						break;
					}
					pucDataCnt += sizChunkSize;
				}
			}
		}
	}

	return iResult;
}


int romloader_usb_device_libusb::netx10_load_code(libusb_device_handle* ptDevHandle, const unsigned char* pucNetxCode, size_t sizNetxCode)
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
		fprintf(stderr, "%s(%p): Invalid netx code, header missing.\n", m_pcPluginId, this);
		iResult = -1;
	}
	else
	{
		/* Construct the command. */
		ulLoadAddress  =  (unsigned long)(pucNetxCode[0x04]);
		ulLoadAddress |= ((unsigned long)(pucNetxCode[0x05])) <<  8U;
		ulLoadAddress |= ((unsigned long)(pucNetxCode[0x06])) << 16U;
		ulLoadAddress |= ((unsigned long)(pucNetxCode[0x07])) << 24U;
		sizLine = snprintf((char*)aucBuffer, sizeof(aucBuffer), "l %lx\n", ulLoadAddress);
		iResult = libusb_bulk_transfer(ptDevHandle, m_tDeviceId.ucEndpoint_Out, aucBuffer, sizLine, &iProcessed, uiTimeoutMs);
		if( iResult!=0 )
		{
			fprintf(stderr, "%s(%p): Failed to send packet!\n", m_pcPluginId, this);
			iResult = -1;
		}
		else if( sizLine!=iProcessed )
		{
			fprintf(stderr, "%s(%p): Requested to send %d bytes, but only %d were processed!\n", m_pcPluginId, this, sizLine, iProcessed);
			iResult = -1;
		}
		else
		{
			netx10_discard_until_timeout(ptDevHandle);

			tUuencoder.set_data(pucNetxCode, sizNetxCode);

			/* Send the data line by line with a delay of 10ms. */
			do
			{
				sizLine = tUuencoder.process((char*)aucBuffer, sizeof(aucBuffer));
				if( sizLine!=0 )
				{
					uiTimeoutMs = 100;

					iResult = libusb_bulk_transfer(ptDevHandle, m_tDeviceId.ucEndpoint_Out, aucBuffer, sizLine, &iProcessed, uiTimeoutMs);
					if( iResult!=0 )
					{
						fprintf(stderr, "%s(%p): Failed to send packet!\n", m_pcPluginId, this);
						iResult = -1;
						break;
					}
					else if( sizLine!=iProcessed )
					{
						fprintf(stderr, "%s(%p): Requested to send %d bytes, but only %d were processed!\n", m_pcPluginId, this, sizLine, iProcessed);
						iResult = -1;
						break;
					}

					SLEEP_MS(10);
				}
			} while( tUuencoder.isFinished()==false );

			if( iResult==0 )
			{
				netx10_discard_until_timeout(ptDevHandle);
			}
		}
	}

	return iResult;
}


int romloader_usb_device_libusb::netx56_execute_command(libusb_device_handle *ptDevHandle, const unsigned char *aucOutBuf, size_t sizOutBuf, unsigned char *aucInBuf, size_t *psizInBuf)
{
	int iResult;
	size_t sizProcessed;
	int iProcessed;
	unsigned int uiTimeoutMs;
	const unsigned char ucEndpoint_In = 0x85;
	const unsigned char ucEndpoint_Out = 0x04;


	uiTimeoutMs = 500; // 100

	iResult = libusb_bulk_transfer(ptDevHandle, ucEndpoint_Out, (unsigned char*)aucOutBuf, sizOutBuf, &iProcessed, uiTimeoutMs);
	if( iResult!=0 )
	{
		fprintf(stderr, "%s(%p): Failed to send data: %s\n", m_pcPluginId, this, libusb_strerror(iResult));
	}
	else if( sizOutBuf!=iProcessed )
	{
		fprintf(stderr, "%s(%p): Requested to send %d bytes, but only %d were processed!\n", m_pcPluginId, this, sizOutBuf, iProcessed);
		iResult = 1;
	}
	else
	{
		iResult = libusb_bulk_transfer(ptDevHandle, ucEndpoint_In, aucInBuf, 64, &iProcessed, uiTimeoutMs);
		if( iResult==0 )
		{
			if( iProcessed==0 )
			{
				fprintf(stderr, "%s(%p): Received empty packet!\n", m_pcPluginId, this);
				iResult = 1;
			}
			else
			{
				*psizInBuf = iProcessed;
			}
		}
	}

	return iResult;
}


int romloader_usb_device_libusb::netx56_load_code(libusb_device_handle* ptDevHandle, const unsigned char* pucNetxCode, size_t sizNetxCode)
{
	size_t sizChunk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	long lBytesProcessed;
	unsigned char aucTxBuf[64];
	unsigned char aucRxBuf[64];
	unsigned long ulNetxAddress;


//	fprintf(stderr, "+netx56_load_code(): ptDevHandle=%p, pucNetxCode=%p, sizNetxCode=%d\n", ptDevHandle, pucNetxCode, sizNetxCode);

	/* Be optimistic. */
	iResult = 0;

	if( pucNetxCode[0x00]!='m' || pucNetxCode[0x01]!='o' || pucNetxCode[0x02]!='o' || pucNetxCode[0x03]!='h' )
	{
		fprintf(stderr, "%s(%p): Invalid netx code, header missing.\n", m_pcPluginId, this);
		iResult = -1;
	}
	else
	{
		/* Construct the command. */
		ulNetxAddress  =  (unsigned long)(pucNetxCode[0x04]);
		ulNetxAddress |= ((unsigned long)(pucNetxCode[0x05])) <<  8U;
		ulNetxAddress |= ((unsigned long)(pucNetxCode[0x06])) << 16U;
		ulNetxAddress |= ((unsigned long)(pucNetxCode[0x07])) << 24U;

		do
		{
			sizChunk = sizNetxCode;
			if( sizChunk>58 )
			{
				sizChunk = 58;
			}

			/* Construct the command packet. */
			aucTxBuf[0x00] = MONITOR_COMMAND_Write |
			                 (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT);
			aucTxBuf[0x01] = (unsigned char)sizChunk;
			aucTxBuf[0x02] = (unsigned char)( ulNetxAddress       & 0xffU);
			aucTxBuf[0x03] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
			aucTxBuf[0x04] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
			aucTxBuf[0x05] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
			memcpy(aucTxBuf+6, pucNetxCode, sizChunk);

			iResult = netx56_execute_command(ptDevHandle, aucTxBuf, sizChunk+6, aucRxBuf, &sizInBuf);
			if( iResult!=0 )
			{
				break;
			}
			else if( sizInBuf!=1 )
			{
				hexdump(aucRxBuf, sizInBuf);
				iResult = -1;
				break;
			}
			else
			{
				pucNetxCode += sizChunk;
				sizNetxCode -= sizChunk;
				ulNetxAddress += sizChunk;
			}
		} while( sizNetxCode!=0 );
	}

	return iResult;
}




int romloader_usb_device_libusb::netx500_start_code(libusb_device_handle *ptDevHandle, const unsigned char *pucNetxCode)
{
	int iResult;
	unsigned long ulExecAddress;
	size_t sizLine;
	unsigned char aucOutBuffer[64] = {0};
	unsigned char aucInBuffer[64] = {0};


	ulExecAddress  =  (unsigned long)(pucNetxCode[0x08]);
	ulExecAddress |= ((unsigned long)(pucNetxCode[0x09])) <<  8U;
	ulExecAddress |= ((unsigned long)(pucNetxCode[0x0a])) << 16U;
	ulExecAddress |= ((unsigned long)(pucNetxCode[0x0b])) << 24U;

	/* Generate call command. */
	sizLine = snprintf((char*)(aucOutBuffer+1), sizeof(aucOutBuffer), "call %lx 0\n", ulExecAddress);
	/* Set the length. */
	aucOutBuffer[0] = (unsigned char)(sizLine + 1);

//	printf("Start code at 0x%08x.\n", ulExecAddress);

	/* Send the command. */
	iResult = netx500_exchange_data(ptDevHandle, aucOutBuffer, aucInBuffer);
	if( iResult==LIBUSB_SUCCESS )
	{
		/* Terminate the command. */
		aucOutBuffer[0] = 0;
		iResult = netx500_exchange_data(ptDevHandle, aucOutBuffer, aucInBuffer);
	}

	return iResult;
}


int romloader_usb_device_libusb::netx10_start_code(libusb_device_handle *ptDevHandle, const unsigned char *pucNetxCode)
{
	unsigned char aucBuffer[64];
	size_t sizBlock;
	int iResult;
	int iProcessed;
	unsigned int uiTimeoutMs;
	unsigned long ulExecAddress;


	/* Construct the command. */
	ulExecAddress  =  (unsigned long)(pucNetxCode[0x08]);
	ulExecAddress |= ((unsigned long)(pucNetxCode[0x09])) <<  8U;
	ulExecAddress |= ((unsigned long)(pucNetxCode[0x0a])) << 16U;
	ulExecAddress |= ((unsigned long)(pucNetxCode[0x0b])) << 24U;

	/* Construct the command. */
	sizBlock = sprintf((char*)aucBuffer, "g %lx 0\n", ulExecAddress);

	uiTimeoutMs = 1000;
	iResult = libusb_bulk_transfer(ptDevHandle, m_tDeviceId.ucEndpoint_Out, aucBuffer, sizBlock, &iProcessed, uiTimeoutMs);
	if( iResult!=0 )
	{
		fprintf(stderr, "%s(%p): Failed to send packet!\n", m_pcPluginId, this);
		iResult = -1;
	}
	else if( sizBlock!=iProcessed )
	{
		fprintf(stderr, "%s(%p): Requested to send %d bytes, but only %d were processed!\n", m_pcPluginId, this, sizBlock, iProcessed);
		iResult = -1;
	}
	else
	{
		netx10_discard_until_timeout(ptDevHandle);

		iResult = 0;
	}

	return iResult;
}



int romloader_usb_device_libusb::netx56_start_code(libusb_device_handle *ptDevHandle, const unsigned char *pucNetxCode)
{
	int iResult;
	int iProcessed;
	unsigned int uiTimeoutMs;
	unsigned long ulExecAddress;
	unsigned char aucTxBuf[64];
	unsigned char aucRxBuf[64];
	size_t sizRxBuf;


	/* Construct the command. */
	ulExecAddress  =  (unsigned long)(pucNetxCode[0x08]);
	ulExecAddress |= ((unsigned long)(pucNetxCode[0x09])) <<  8U;
	ulExecAddress |= ((unsigned long)(pucNetxCode[0x0a])) << 16U;
	ulExecAddress |= ((unsigned long)(pucNetxCode[0x0b])) << 24U;

	/* Construct the command packet. */
	aucTxBuf[0x00] = MONITOR_COMMAND_Execute;
	aucTxBuf[0x01] = (unsigned char)( ulExecAddress       & 0xffU);
	aucTxBuf[0x02] = (unsigned char)((ulExecAddress>> 8U) & 0xffU);
	aucTxBuf[0x03] = (unsigned char)((ulExecAddress>>16U) & 0xffU);
	aucTxBuf[0x04] = (unsigned char)((ulExecAddress>>24U) & 0xffU);
	aucTxBuf[0x05] = 0x00U;
	aucTxBuf[0x06] = 0x00U;
	aucTxBuf[0x07] = 0x00U;
	aucTxBuf[0x08] = 0x00U;

	iResult = netx56_execute_command(ptDevHandle, aucTxBuf, 9, aucRxBuf, &sizRxBuf);
	if( iResult!=0 )
	{
		iResult = -1;
	}
	else if( sizRxBuf!=1 )
	{
		fprintf(stderr, "call answer has invalid size: %d\n", sizRxBuf);
		hexdump(aucRxBuf, sizRxBuf);
		iResult = -1;
	}
	else if( aucRxBuf[0]!=0x00 )
	{
		fprintf(stderr, "call answer status is not OK: 0x%02x\n", aucRxBuf[0]);
		iResult = -1;
	}

	return iResult;
}



int romloader_usb_device_libusb::netx10_upgrade_romcode(libusb_device *ptDevice, libusb_device **pptUpdatedNetxDevice)
{
	int iResult;
	libusb_device_handle *ptDevHandle;


//	printf(". Found old netX10 romcode, starting download.\n");

	iResult = libusb_open(ptDevice, &ptDevHandle);
	if( iResult!=0 )
	{
		fprintf(stderr, "%s(%p): Failed to open the device: %s\n", m_pcPluginId, this, libusb_strerror(iResult));
	}
	else
	{
		/* set the configuration */
		iResult = libusb_set_configuration(ptDevHandle, 1);
		if( iResult!=0 )
		{
			fprintf(stderr, "%s(%p): Failed to set config 1: %s\n", m_pcPluginId, this, libusb_strerror(iResult));
		}
		else
		{
			iResult = libusb_claim_interface(ptDevHandle, 0);
			if( iResult!=0 )
			{
				fprintf(stderr, "%s(%p): Failed to claim interface 0: %s\n", m_pcPluginId, this, libusb_strerror(iResult));

				libusb_close(ptDevHandle);
			}
			else
			{
				/* Read data until 'newline''>' or timeout. */
				netx10_discard_until_timeout(ptDevHandle);

				/* Load data. */
				netx10_load_code(ptDevHandle, auc_usbmon_netx10, sizeof(auc_usbmon_netx10));

				/* Start the code. */
				netx10_start_code(ptDevHandle, auc_usbmon_netx10);

				libusb_release_interface(ptDevHandle, m_tDeviceId.ucInterface);
				libusb_close(ptDevHandle);

				SLEEP_MS(100);

				*pptUpdatedNetxDevice = ptDevice;

				iResult = 0;
			}
		}
	}

	return iResult;
}

#if 0 
libusb_device *romloader_usb_device_libusb::find_usb_device_by_location(unsigned char ucLocation_Bus, unsigned char ucLocation_Port)
{
	libusb_device *ptFound;
	ssize_t ssizDevices;
	libusb_device **pptUsbDevCnt;
	libusb_device **pptUsbDevMax;
	unsigned char ucDevice_Bus;
	unsigned char ucDevice_Port;


	/* Nothing found yet. */
	ptFound = NULL;
	
	/* Get a list with all connected devices. */
	ssizDevices = libusb_get_device_list(m_ptLibUsbContext, &pptUsbDevCnt);
	pptUsbDevMax = pptUsbDevCnt + ssizDevices;
	while(pptUsbDevCnt<pptUsbDevMax)
	{
		/* Get the bus and the port of the device. */
		ucDevice_Bus = libusb_get_bus_number(*pptUsbDevCnt);
		ucDevice_Port = libusb_get_port_number(*pptUsbDevCnt);
		
		/* Is this the requested location? */
		if( ucLocation_Bus==ucDevice_Bus && ucLocation_Port==ucDevice_Port )
		{
			/* Yes -> this is the device. */
			ptFound = *pptUsbDevCnt;
			
			/* Reference the device. */
			libusb_ref_device(ptFound);
			
			break;
		}
		++pptUsbDevCnt;
	}

	return ptFound;
}
#endif


int romloader_usb_device_libusb::netx500_upgrade_romcode(libusb_device *ptDevice, libusb_device **pptUpdatedNetxDevice)
{
	int iResult;
	libusb_device_handle *ptDevHandle;
	unsigned char ucDevAddr_Bus;
	unsigned char ucDevAddr_Port;
	int iDelay;

	
	printf(". Found old netX500 romcode, starting download.\n");

	iResult = libusb_open(ptDevice, &ptDevHandle);
	if( iResult!=0 )
	{
		fprintf(stderr, "%s(%p): Failed to open the device: %s\n", m_pcPluginId, this, libusb_strerror(iResult));
	}
	else
	{
		/* set the configuration */
		iResult = libusb_set_configuration(ptDevHandle, 1);
		if( iResult!=0 )
		{
			fprintf(stderr, "%s(%p): Failed to set config 1: %s\n", m_pcPluginId, this, libusb_strerror(iResult));
		}
		else
		{
			iResult = libusb_claim_interface(ptDevHandle, 0);
			if( iResult!=0 )
			{
				fprintf(stderr, "%s(%p): Failed to claim interface 0: %s\n", m_pcPluginId, this, libusb_strerror(iResult));

				libusb_close(ptDevHandle);
			}
			else
			{
				/* Read data until 'newline''>' or timeout. */
				netx500_discard_until_timeout(ptDevHandle);

				/* Load data. */
				netx500_load_code(ptDevHandle, auc_usbmon_netx500, sizeof(auc_usbmon_netx500));
				/* Discard load response. */
				netx500_discard_until_timeout(ptDevHandle);

				/* Start the code parameter. */
				netx500_start_code(ptDevHandle, auc_usbmon_netx500);

				/* Release the interface. */
				libusb_release_interface(ptDevHandle, m_tDeviceId.ucInterface);

				libusb_close(ptDevHandle);

				SLEEP_MS(100);
				
				*pptUpdatedNetxDevice = ptDevice;
				iResult = 0;
			}
		}
	}

	return iResult;
}


int romloader_usb_device_libusb::netx56_upgrade_romcode(libusb_device *ptDevice, libusb_device **pptUpdatedNetxDevice)
{
	int iResult;
	libusb_device_handle *ptDevHandle;


	iResult = libusb_open(ptDevice, &ptDevHandle);
	if( iResult!=0 )
	{
		fprintf(stderr, "%s(%p): Failed to open the device: %s\n", m_pcPluginId, this, libusb_strerror(iResult));
	}
	else
	{
		iResult = libusb_claim_interface(ptDevHandle, 1);
		if( iResult!=0 )
		{
			fprintf(stderr, "%s(%p): Failed to claim interface 1: %s\n", m_pcPluginId, this, libusb_strerror(iResult));

			libusb_close(ptDevHandle);
		}
		else
		{
			/* Load data. */
			netx56_load_code(ptDevHandle, auc_usbmon_netx56, sizeof(auc_usbmon_netx56));

			/* Start the code. */
			netx56_start_code(ptDevHandle, auc_usbmon_netx56);

			libusb_release_interface(ptDevHandle, m_tDeviceId.ucInterface);
			libusb_close(ptDevHandle);

			SLEEP_MS(100);

			*pptUpdatedNetxDevice = ptDevice;

			iResult = 0;
		}
	}

	return iResult;
}


int romloader_usb_device_libusb::send_packet(const unsigned char *aucOutBuf, size_t sizOutBuf, unsigned int uiTimeoutMs)
{
	int iResult;
	int iProcessed;


	iResult = libusb_bulk_transfer(m_ptDevHandle, m_tDeviceId.ucEndpoint_Out, (unsigned char*)aucOutBuf, sizOutBuf, &iProcessed, uiTimeoutMs);
	if( iResult!=0 )
	{
		fprintf(stderr, "%s(%p): Failed to send data: %s\n", m_pcPluginId, this, libusb_strerror(iResult));
	}
	else if( sizOutBuf!=iProcessed )
	{
		fprintf(stderr, "%s(%p): Requested to send %d bytes, but only %d were processed!\n", m_pcPluginId, this, sizOutBuf, iProcessed);
		iResult = 1;
	}

	return iResult;
}


int romloader_usb_device_libusb::receive_packet(unsigned char *aucInBuf, size_t *psizInBuf, unsigned int uiTimeoutMs)
{
	int iResult;
	int iProcessed;


	iResult = libusb_bulk_transfer(m_ptDevHandle, m_tDeviceId.ucEndpoint_In, aucInBuf, 64, &iProcessed, uiTimeoutMs);
	if( iResult==0 )
	{
		if( iProcessed==0 )
		{
			fprintf(stderr, "%s(%p): Received empty packet!\n", m_pcPluginId, this);
			iResult = 1;
		}
		else
		{
			*psizInBuf = iProcessed;
		}
	}

	return iResult;
}


int romloader_usb_device_libusb::execute_command(const unsigned char *aucOutBuf, size_t sizOutBuf, unsigned char *aucInBuf, size_t *psizInBuf)
{
	int iResult;
	size_t sizProcessed;
	unsigned int uiTimeoutMs;


	uiTimeoutMs = 500; // 100

	iResult = send_packet(aucOutBuf, sizOutBuf, uiTimeoutMs);
	if( iResult!=0 )
	{
		fprintf(stderr, "%s(%p): Failed to send data: %s\n", m_pcPluginId, this, libusb_strerror(iResult));
	}
	else
	{

		iResult = receive_packet(aucInBuf, &sizProcessed, uiTimeoutMs);
		if( iResult!=0 )
		{
			fprintf(stderr, "%s(%p): Failed to receive data: %s\n", m_pcPluginId, this, libusb_strerror(iResult));
		}
		else
		{
//			printf("received:\n");
//			hexdump(aucInBuf, iProcessed);
			*psizInBuf = sizProcessed;
		}
	}

	return iResult;
}


int romloader_usb_device_libusb::update_old_netx_device(libusb_device *ptNetxDevice, libusb_device **pptUpdatedNetxDevice)
{
	int iResult;
	const NETX_USB_DEVICE_T *ptId;
	libusb_device *ptUpdatedDevice;


	switch(m_tDeviceId.tChiptyp)
	{
	case ROMLOADER_CHIPTYP_UNKNOWN:
		iResult = LIBUSB_ERROR_OTHER;
		break;

	case ROMLOADER_CHIPTYP_NETX500:
		iResult = netx500_upgrade_romcode(ptNetxDevice, &ptUpdatedDevice);
		break;

	case ROMLOADER_CHIPTYP_NETX100:
		iResult = netx500_upgrade_romcode(ptNetxDevice, &ptUpdatedDevice);
		break;

	case ROMLOADER_CHIPTYP_NETX50:
		/* The netX50 romcode provides a CDC device over USB. It is not handled by this plugin. */
		iResult = LIBUSB_ERROR_OTHER;
		break;

	case ROMLOADER_CHIPTYP_NETX5:
		/* TODO: insert update code for the netX5. */
		iResult = LIBUSB_ERROR_OTHER;
		break;

	case ROMLOADER_CHIPTYP_NETX10:
		iResult = netx10_upgrade_romcode(ptNetxDevice, &ptUpdatedDevice);
		break;

	case ROMLOADER_CHIPTYP_NETX56:
		iResult = netx56_upgrade_romcode(ptNetxDevice, &ptUpdatedDevice);
		break;
	}

	/* Did the update succeed? */
	if( iResult==LIBUSB_SUCCESS )
	{
		/* Yes -> try to identify the device again. */
		ptId = identifyDevice(ptUpdatedDevice);
		if( ptId==NULL )
		{
			fprintf(stderr, "%s(%p): Failed to identify the updated device!\n", m_pcPluginId, this);
			iResult = LIBUSB_ERROR_OTHER;
		}
		else
		{
//			printf("After update: %s\n", ptId->pcName);
			/* Update the settings. */
			memcpy(&m_tDeviceId, ptId, sizeof(NETX_USB_DEVICE_T));

			*pptUpdatedNetxDevice = ptUpdatedDevice;
		}
	}

	return iResult;
}


unsigned short romloader_usb_device_libusb::crc16(const unsigned char *pucData, size_t sizData)
{
	const unsigned char *pucDataCnt;
	const unsigned char *pucDataEnd;
	unsigned int uiCrc;


	pucDataCnt = pucData;
	pucDataEnd = pucData + sizData;

	uiCrc = 0xffffU;
	while( pucDataCnt<pucDataEnd )
	{
		uiCrc  = (uiCrc >> 8U) | ((uiCrc & 0xff) << 8U);
		uiCrc ^= *(pucDataCnt++);
		uiCrc ^= (uiCrc & 0xffU) >> 4U;
		uiCrc ^= (uiCrc & 0x0fU) << 12U;
		uiCrc ^= ((uiCrc & 0xffU) << 4U) << 1U;
	}

	return (unsigned short)uiCrc;
}

