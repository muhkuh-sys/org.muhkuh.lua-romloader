#include "romloader_papa_schlumpf_device.h"
#include "papa_schlumpf_firmware_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#if CFG_DEBUGMSG!=0
	/* show all messages by default */
	static unsigned long s_ulCurSettings = 0xffffffff;

	#define DEBUGZONE(n)  (s_ulCurSettings&(0x00000001<<(n)))

	/* NOTE: These defines must match the ZONE_* defines. */
	#define DBG_ZONE_ERROR      0
	#define DBG_ZONE_WARNING    1
	#define DBG_ZONE_FUNCTION   2
	#define DBG_ZONE_INIT       3
	#define DBG_ZONE_VERBOSE    7

	#define ZONE_ERROR          DEBUGZONE(DBG_ZONE_ERROR)
	#define ZONE_WARNING        DEBUGZONE(DBG_ZONE_WARNING)
	#define ZONE_FUNCTION       DEBUGZONE(DBG_ZONE_FUNCTION)
	#define ZONE_INIT           DEBUGZONE(DBG_ZONE_INIT)
	#define ZONE_VERBOSE        DEBUGZONE(DBG_ZONE_VERBOSE)

	#define DEBUGMSG(cond,printf_exp) ((void)((cond)?(printf printf_exp),1:0))
#else  /* CFG_DEBUGMSG!=0 */
	#define DEBUGMSG(cond,printf_exp) ((void)0)
#endif /* CFG_DEBUGMSG!=0 */



romloader_papa_schlumpf_device::romloader_papa_schlumpf_device(const char *pcPluginId)
 : m_pcPluginId(NULL)
 , m_ptLibUsbDeviceHandle(NULL)
 , m_ptLibUsbContext(NULL)
{
	int iResult;


	m_pcPluginId = strdup(pcPluginId);

	/* Initialize the libusb context. */
	iResult = libusb_init(&m_ptLibUsbContext);
	if( iResult!=0 )
	{
		fprintf(stderr, "%s: Error initializing libusb: %d\n", m_pcPluginId, iResult);
		m_ptLibUsbContext = NULL;
	}
	else
	{
		/* Set the debug level to a bit more verbose. */
		libusb_set_debug(m_ptLibUsbContext, LIBUSB_LOG_LEVEL_INFO);
	}
}



romloader_papa_schlumpf_device::~romloader_papa_schlumpf_device(void)
{
	if( m_ptLibUsbContext!=NULL )
	{
		/* Close the libusb context. */
		libusb_exit(m_ptLibUsbContext);
		m_ptLibUsbContext = NULL;
	}


	if( m_pcPluginId!=NULL )
	{
		free(m_pcPluginId);
		m_pcPluginId = NULL;
	}
}



romloader_papa_schlumpf_device::PAPA_SCHLUMPF_ERROR_T romloader_papa_schlumpf_device::detect_interfaces(romloader_papa_schlumpf_device::INTERFACE_REFERENCE_T **pptInterfaces, size_t *psizInterfaces)
{
	PAPA_SCHLUMPF_ERROR_T tResult;
	int iResult;
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device **ptDevCnt, **ptDevEnd;
	libusb_device *ptDevice;
	libusb_device *ptDevPapaSchlumpf;
	libusb_device_handle *ptDevHandle;
	libusb_device_handle *ptDevHandlePapaSchlumpf;
	struct libusb_device_descriptor sDevDesc;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;
	size_t sizRefCnt;
	size_t sizRefMax;
	INTERFACE_REFERENCE_T *ptRef;
	INTERFACE_REFERENCE_T *ptRefNew;
	const int iPathMax = 32;
	unsigned char aucPath[iPathMax];
	int iPathStringPos;
	int iCnt;
	char *pcPathString;


	/* Be optimistic... */
	tResult = PAPA_SCHLUMPF_ERROR_Ok;

	/* Initialize the references array. */
	sizRefCnt = 0;
	sizRefMax = 16;
	ptRef = (INTERFACE_REFERENCE_T*)malloc(sizRefMax*sizeof(INTERFACE_REFERENCE_T));
	if( ptRef==NULL )
	{
		fprintf(stderr, "%s: out of memory!\n", m_pcPluginId);
		tResult = PAPA_SCHLUMPF_ERROR_MemoryError;
	}
	else
	{
		/* Get the list of all connected USB devices. */
		ptDeviceList = NULL;
		ssizDevList = libusb_get_device_list(m_ptLibUsbContext, &ptDeviceList);
		if( ssizDevList<0 )
		{
			/* Failed to get the list of all USB devices. */
			fprintf(stderr, "%s: failed to detect usb devices: %ld:%s\n", m_pcPluginId, ssizDevList, libusb_strerror(libusb_error(ssizDevList)));
			tResult = PAPA_SCHLUMPF_ERROR_FailedToListUSBDevices;
		}
		else
		{
			/* No Papa Schlumpf device found yet. */
			ptDevPapaSchlumpf = NULL;

			/* Loop over all devices. */
			ptDevCnt = ptDeviceList;
			ptDevEnd = ptDevCnt + ssizDevList;
			while( ptDevCnt<ptDevEnd )
			{
				ptDevice = *ptDevCnt;

				/* Try to open the device. */
				iResult = libusb_open(ptDevice, &ptDevHandle);
				if( iResult==LIBUSB_SUCCESS )
				{
					/* Get the device descriptor. */
					iResult = libusb_get_descriptor(ptDevHandle, LIBUSB_DT_DEVICE, 0, (unsigned char*)&sDevDesc, sizeof(struct libusb_device_descriptor));
					if( iResult==sizeof(struct libusb_device_descriptor) )
					{
						printf("Looking at USB device VID=0x%04x, PID=0x%04x\n", sDevDesc.idVendor, sDevDesc.idProduct);

						/* Is this a "Papa Schlumpf" device? */
						if( sDevDesc.idVendor==PAPA_SCHLUMPF_USB_VENDOR_ID && sDevDesc.idProduct==PAPA_SCHLUMPF_USB_PRODUCT_ID )
						{
							printf("Found a Papa Schlumpf device!\n");

							/* Get the bus and device number */
							uiBusNr = libusb_get_bus_number(ptDevice);
							uiDevAdr = libusb_get_device_address(ptDevice);

							/* Get the location. */
							iResult = libusb_get_port_numbers(ptDevice, aucPath, iPathMax);
							if( iResult<=0 )
							{
								fprintf(stderr, "Failed to get the port numbers: %d\n", iResult);
							}
							else
							{
								/* Is enough space in the array for one more entry? */
								if( sizRefCnt>=sizRefMax )
								{
									/* No -> expand the array. */
									sizRefMax *= 2;
									/* Detect overflow or limitation. */
									if( sizRefMax<=sizRefCnt )
									{
										fprintf(stderr, "%s: out of memory!\n", m_pcPluginId);
										tResult = PAPA_SCHLUMPF_ERROR_MemoryError;
										break;
									}
									/* Reallocate the array. */
									ptRefNew = (INTERFACE_REFERENCE_T*)realloc(ptRef, sizRefMax*sizeof(INTERFACE_REFERENCE_T));
									if( ptRefNew==NULL )
									{
										fprintf(stderr, "%s: out of memory!\n", m_pcPluginId);
										tResult = PAPA_SCHLUMPF_ERROR_MemoryError;
										break;
									}
									ptRef = ptRefNew;
								}
								ptRef[sizRefCnt].uiBusNr = uiBusNr;
								ptRef[sizRefCnt].uiDevAdr = uiDevAdr;

								/* Build the path string. */
								pcPathString = ptRef[sizRefCnt].acPathString;
								sprintf(pcPathString, "%02x", uiBusNr);
								pcPathString += 2;
								for(iCnt=0; iCnt<iResult; ++iCnt)
								{
									sprintf(pcPathString, "%02x", aucPath[iCnt]);
									pcPathString += 2;
								}
								*pcPathString = 0;
								fprintf(stderr, "Path: %s\n", ptRef[sizRefCnt].acPathString);

								++sizRefCnt;
							}
						}
					}

					/* Close the device. */
					libusb_close(ptDevHandle);
				}

				/* Next device in the list... */
				++ptDevCnt;
			}
		}
	}

	if( tResult!=PAPA_SCHLUMPF_ERROR_Ok )
	{
		if( ptRef!=NULL )
		{
			free(ptRef);
		}
		sizRefCnt = 0;
	}

	*pptInterfaces = ptRef;
	*psizInterfaces = sizRefCnt;

	return tResult;
}



romloader_papa_schlumpf_device::USB_STATUS_T romloader_papa_schlumpf_device::sendAndReceivePackets(uint8_t* aucOutBuf, uint32_t sizOutBuf, uint8_t* aucInBuf, uint32_t* sizInBuf)
{
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;


	tUsbStatus = sendPacket(aucOutBuf, sizOutBuf);
	if( tUsbStatus==USB_STATUS_OK)
	{
		tUsbStatus = receivePacket(aucInBuf, sizInBuf);
	}

	return tUsbStatus;
}



romloader_papa_schlumpf_device::USB_STATUS_T romloader_papa_schlumpf_device::sendPacket(uint8_t* aucOutBuf, uint32_t sizOutBuf)
{
	romloader_papa_schlumpf_device::USB_STATUS_T tResult;
	int iResult     = 0;
	int iTransfered = 0;
	int iTimeoutMs  = 60000;

	tResult = romloader_papa_schlumpf_device::USB_STATUS_OK;
	iResult = libusb_bulk_transfer(m_ptLibUsbDeviceHandle, 0x01, aucOutBuf, sizOutBuf, &iTransfered, iTimeoutMs);
	if(iResult != LIBUSB_SUCCESS)
	{
		//fprintf(stderr, "USB Error: Writing Data to Papa Schlumpf device failed!\n");
		tResult = romloader_papa_schlumpf_device::USB_STATUS_ERROR;
	}
	else
	{
		/*Check if all bytes were transfered*/
		if(iTransfered != sizOutBuf)
		{
			//fprintf(stderr, "USB Error: Not all bytes written to Papa Schlumpf device!\n");
			tResult = romloader_papa_schlumpf_device::USB_STATUS_NOT_ALL_BYTES_TRANSFERED;
		}
	}

	return tResult;
}



romloader_papa_schlumpf_device::USB_STATUS_T romloader_papa_schlumpf_device::receivePacket(uint8_t* aucInBuf, uint32_t* pSizInBuf)
{
	romloader_papa_schlumpf_device::USB_STATUS_T tResult;
	int iResult        = 0;
	int iTransfered    = 0;
	int iTimeoutMs     = 60000;

	tResult = USB_STATUS_OK;
	iResult = libusb_bulk_transfer(m_ptLibUsbDeviceHandle, 0x81, aucInBuf, 64, &iTransfered, iTimeoutMs);
	if(iResult != LIBUSB_SUCCESS)
	{
		tResult = USB_STATUS_ERROR;
	}
	else
	{
		if (iTransfered == 0)
		{
			tResult = USB_STATUS_RECEIVED_EMPTY_PACKET;
		}
		else
		{
			*pSizInBuf = iTransfered;
		}
	}

	return tResult;
}

