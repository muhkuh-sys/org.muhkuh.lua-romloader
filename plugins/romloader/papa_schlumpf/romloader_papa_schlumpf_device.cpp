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


typedef union PAPA_SCHLUMPF_PARAMETER_DATA_UNION
{
	PAPA_SCHLUMPF_PARAMETER_T tPapaSchlumpfParameter;
	uint8_t auc[sizeof(PAPA_SCHLUMPF_PARAMETER_T)];
} PAPA_SCHLUMPF_PARAMETER_DATA_T;

typedef union PAPA_SCHLUMPF_DATA_BUFFER_UNION
{
	uint8_t  aucBuf[64];
	char     acBuf[64];
	uint16_t ausBuf[64 / sizeof(uint16_t)];
	uint32_t aulBuf[64 / sizeof(uint32_t)];
} PAPA_SCHLUMPF_DATA_BUFFER_T;



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
//	libusb_device *ptDevPapaSchlumpf;
	libusb_device_handle *ptDevHandle;
//	libusb_device_handle *ptDevHandlePapaSchlumpf;
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
//			ptDevPapaSchlumpf = NULL;

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



int romloader_papa_schlumpf_device::read_data08(uint32_t ulDataSourceAddress, uint8_t *pucData)
{
	int iResult;
	uint32_t sizInBuf;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;


	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Read_8;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulAddress   = ulDataSourceAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulElemCount = 1;

	/*Send Command*/
	sizInBuf = 0;
	iResult = sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if( iResult==0 )
	{
		/*Check if error in response*/
		tPapaSchlumpfUsbCommandStatus = (PAPA_SCHLUMPF_USB_COMMAND_STATUS_T)(tBuffer.aulBuf[0]);
		if( tPapaSchlumpfUsbCommandStatus==USB_COMMAND_STATUS_Ok )
		{
			*pucData = tBuffer.aucBuf[4];
		}
		else
		{
			iResult = -1;
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::read_data16(uint32_t ulDataSourceAddress, uint16_t *pusData)
{
	int iResult;
	uint32_t sizInBuf;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;


	/* Prepare the parameter structure. */
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Read_16;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulAddress   = ulDataSourceAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulElemCount = 1;

	/* Prepare the parameter structure. */
	sizInBuf = 0;
	iResult = sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if( iResult==0 )
	{
		/*Check if error in response*/
		tPapaSchlumpfUsbCommandStatus = (PAPA_SCHLUMPF_USB_COMMAND_STATUS_T)(tBuffer.aulBuf[0]);
		if( tPapaSchlumpfUsbCommandStatus==USB_COMMAND_STATUS_Ok )
		{
			*pusData = tBuffer.ausBuf[2];
		}
		else
		{
			iResult = -1;
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::read_data32(uint32_t ulDataSourceAddress, uint32_t *pulData)
{
	int iResult;
	uint32_t sizInBuf;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;


	/* Prepare the parameter structure. */
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand      = USB_COMMAND_Read_32;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulAddress   = ulDataSourceAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulElemCount = 1;

	/* Prepare the parameter structure. */
	sizInBuf = 0;
	iResult = sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if( iResult==0 )
	{
		/*Check if error in response*/
		tPapaSchlumpfUsbCommandStatus = (PAPA_SCHLUMPF_USB_COMMAND_STATUS_T)(tBuffer.aulBuf[0]);
		if( tPapaSchlumpfUsbCommandStatus==USB_COMMAND_STATUS_Ok )
		{
			*pulData = tBuffer.aulBuf[1];
		}
		else
		{
			iResult = -1;
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::read_image(uint32_t ulNetxAddress, uint32_t ulSize, void *pvData)
{
	int iResult;
	uint32_t sizInBuf;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;


	if( ulSize>PAPA_SCHLUMPF_COMMAND_DATA_SIZE )
	{
		fprintf(stderr, "read_image size exceeds valid range!");
		iResult = -1;
	}
	else
	{
		/* Set the command. */
		tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Read_8;
		/*Set address in parameter structure (account for bytes already read)*/
		tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulAddress   = ulNetxAddress;
		/*Set size in parameter structure*/
		tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulElemCount = ulSize;

		sizInBuf = 0;
		iResult = sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
		if( iResult==0 )
		{
			tPapaSchlumpfUsbCommandStatus = (PAPA_SCHLUMPF_USB_COMMAND_STATUS_T)(tBuffer.aulBuf[0]);
			if( tPapaSchlumpfUsbCommandStatus==USB_COMMAND_STATUS_Ok )
			{
				memcpy(pvData, tBuffer.aucBuf + 4, ulSize);
			}
			else
			{
				iResult = -1;
			}
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::write_data08(uint32_t ulNetxAddress, uint8_t ucData)
{
	int iResult;
	uint32_t sizInBuf;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;


	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Write_8;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulAddress   = ulNetxAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulElemCount = 1;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.uData.ucData[0] = ucData;

	/*Send Command*/
	sizInBuf = 0;
	iResult = sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if( iResult==0 )
	{
		/*Check if error in response*/
		tPapaSchlumpfUsbCommandStatus = (PAPA_SCHLUMPF_USB_COMMAND_STATUS_T)(tBuffer.aulBuf[0]);
		if( tPapaSchlumpfUsbCommandStatus!=USB_COMMAND_STATUS_Ok )
		{
			iResult = -1;
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::write_data16(uint32_t ulNetxAddress, uint16_t usData)
{
	int iResult;
	uint32_t sizInBuf;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;


	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Write_16;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulAddress   = ulNetxAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulElemCount = 1;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.uData.usData[0] = usData;

	/*Send Command*/
	sizInBuf = 0;
	iResult = sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if( iResult==0 )
	{
		/*Check if error in response*/
		tPapaSchlumpfUsbCommandStatus = (PAPA_SCHLUMPF_USB_COMMAND_STATUS_T)(tBuffer.aulBuf[0]);
		if( tPapaSchlumpfUsbCommandStatus!=USB_COMMAND_STATUS_Ok )
		{
			iResult = -1;
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::write_data32(uint32_t ulNetxAddress, uint32_t ulData)
{
	int iResult;
	uint32_t sizInBuf;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;


	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Write_32;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulAddress   = ulNetxAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulElemCount = 1;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.uData.ulData[0] = ulData;

	/*Send Command*/
	sizInBuf = 0;
	iResult = sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if( iResult==0 )
	{
		/*Check if error in response*/
		tPapaSchlumpfUsbCommandStatus = (PAPA_SCHLUMPF_USB_COMMAND_STATUS_T)(tBuffer.aulBuf[0]);
		if( tPapaSchlumpfUsbCommandStatus!=USB_COMMAND_STATUS_Ok )
		{
			iResult = -1;
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::write_image(uint32_t ulNetxAddress, uint32_t ulSize, const void *pvData)
{
	int iResult;
	uint32_t sizInBuf;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;


	if( ulSize>PAPA_SCHLUMPF_COMMAND_DATA_SIZE )
	{
		fprintf(stderr, "write_image size exceeds valid range!");
		iResult = -1;
	}
	else
	{
		/*Set the USB command*/
		tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Write_8;

		/*Set the size*/
		tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulElemCount = ulSize;

		/*Set the destination address (account for already written bytes)*/
		tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulAddress = ulNetxAddress;

		/*Copy data to Parameter Data Buffer*/
		memcpy(tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.uData.ucData, pvData, ulSize);

		/*execute command*/
		sizInBuf = 0;
		iResult = sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
		if( iResult==0 )
		{
			/*Check if error in response*/
			tPapaSchlumpfUsbCommandStatus = (PAPA_SCHLUMPF_USB_COMMAND_STATUS_T)(tBuffer.aulBuf[0]);
			if( tPapaSchlumpfUsbCommandStatus!=USB_COMMAND_STATUS_Ok )
			{
				iResult = -1;
			}
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, PFN_CALL_PROGRESS_DATA_T pfnProgressData, void *pvProgressDataUser)
{
	int iResult;
	uint32_t sizInBuf;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;
	bool isRunning;
	int iIsRunning;
	char *pcProgressData;
	size_t sizProgressData;


	/*Set parameters*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Call;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterCall.ulFunctionAddress   = ulNetxAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterCall.ulFunctionParameter = ulParameterR0;

	/*Send Command*/
	sizInBuf = 0;
	iResult = sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if( iResult==0 )
	{
		/*Check if error in response*/
		tPapaSchlumpfUsbCommandStatus = (PAPA_SCHLUMPF_USB_COMMAND_STATUS_T)(tBuffer.aulBuf[0]);
		if( tPapaSchlumpfUsbCommandStatus!=USB_COMMAND_STATUS_Ok )
		{
			iResult = -1;
		}
		else
		{
			/*Status is OK, receive print messages*/
			isRunning = true;
			while( isRunning==true )
			{
				/*Receive Message*/
				iResult = receivePacket(tBuffer.aucBuf, &sizInBuf);
				if( iResult!=0 )
				{
					isRunning = false;
				}
				else
				{
					if( tBuffer.aulBuf[0]==USB_COMMAND_STATUS_CALL_DONE )
					{
						/*Call is Done*/
						isRunning = false;
						break;
					}
					else if( tBuffer.aulBuf[0]==USB_COMMAND_STATUS_Print )
					{
						/*Is a Print Message*/
						pcProgressData = tBuffer.acBuf + sizeof(uint32_t); //Get a pointer to the Data (minus the status)
						sizProgressData = sizInBuf - sizeof(uint32_t);       //Get the size of the Data  (minus the status)

						if( pcProgressData!=NULL )
						{
							iIsRunning = pfnProgressData(pvProgressDataUser, pcProgressData, sizProgressData);
							if( iIsRunning==0 )
							{
								isRunning = false;
							}
						}
					}
					else
					{
						/*Unknown answer*/
						isRunning = false;
						iResult = -1;
						break;
					}
				}
			}
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::sendAndReceivePackets(uint8_t* aucOutBuf, uint32_t sizOutBuf, uint8_t* aucInBuf, uint32_t* sizInBuf)
{
	int iResult;


	iResult = sendPacket(aucOutBuf, sizOutBuf);
	if( iResult==0 )
	{
		iResult = receivePacket(aucInBuf, sizInBuf);
	}

	return iResult;
}



int romloader_papa_schlumpf_device::sendPacket(uint8_t* aucOutBuf, uint32_t sizOutBuf)
{
	int iResult;
	int iUsbResult;
	int iTransfered;
	int iTimeoutMs;


	/* Be optimistic. */
	iResult = 0;

	iTransfered = 0;
	iTimeoutMs = 60000;
	iUsbResult = libusb_bulk_transfer(m_ptLibUsbDeviceHandle, 0x01, aucOutBuf, sizOutBuf, &iTransfered, iTimeoutMs);
	if( iUsbResult!=LIBUSB_SUCCESS )
	{
		//fprintf(stderr, "USB Error: Writing Data to Papa Schlumpf device failed!\n");
		iResult = -1;
	}
	else
	{
		/* Check if all bytes were transfered. */
		if( iTransfered!=sizOutBuf )
		{
			//fprintf(stderr, "USB Error: Not all bytes written to Papa Schlumpf device!\n");
			iResult = -1;
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::receivePacket(uint8_t* aucInBuf, uint32_t* pSizInBuf)
{
	int iResult;
	int iUsbResult;
	int iTransfered;
	int iTimeoutMs;


	/* Be optimistic. */
	iResult = 0;

	iTransfered = 0;
	iTimeoutMs = 60000;
	iUsbResult = libusb_bulk_transfer(m_ptLibUsbDeviceHandle, 0x81, aucInBuf, 64, &iTransfered, iTimeoutMs);
	if( iUsbResult!=LIBUSB_SUCCESS )
	{
		iResult = -1;
	}
	else
	{
		if (iTransfered == 0)
		{
			iResult = -1;
		}
		else
		{
			*pSizInBuf = iTransfered;
		}
	}

	return iResult;
}



int romloader_papa_schlumpf_device::send_start_packet(void)
{
	int iResult;
	int iLibUsbResult;
	union
	{
		unsigned char auc[sizeof(PAPA_SCHLUMPF_PARAMETER_T)];
		PAPA_SCHLUMPF_PARAMETER_T tParameter;
	} uData;
	int iLength;
	unsigned int uiTimeoutMs;
	int iTransfered;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf_device::send_start_packet():\n"));

	/* Expect success. */
	iResult = 0;

	/* Fill the structure. */
	uData.tParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Start;

	/* Wait at most 1 second. */
	uiTimeoutMs = 1000;

	/* Send data to endpoint 1. */
	iLength = sizeof(PAPA_SCHLUMPF_PARAMETER_T);
	iLibUsbResult = libusb_bulk_transfer(m_ptLibUsbDeviceHandle, 0x01, uData.auc, iLength, &iTransfered, uiTimeoutMs);
	if( iLibUsbResult==LIBUSB_SUCCESS )
	{
		/* Were all bytes transfered? */
		if( iTransfered!=iLength )
		{
			/* Not all bytes were transfered. */
			iResult = -1;
		}
	}
	else
	{
		fprintf(stderr, "%s: failed to write data: %d:%s\n", m_pcPluginId, iLibUsbResult, libusb_strerror(libusb_error(iLibUsbResult)));
		iResult = -1;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf_device::send_start_packet(): iResult=%d\n", iResult));
	return iResult;
}



/* Receive the print messages and the final result.  */
int romloader_papa_schlumpf_device::receive_report(char **ppcResponse, size_t *psizResponse, uint32_t *pulStatus)
{
	int iResult;
	int iLibUsbResult;
	int iLength;
	unsigned int uiTimeoutMs;
	int iTransfered;
	char *pcResponse;
	size_t sizResponse;
	char *pcResponseNew;
	size_t sizResponseNew;
	union
	{
		unsigned char auc[4096];
		char ac[4096];
		uint32_t aul[4096/sizeof(uint32_t)];
	} uData;
	uint32_t ulStatus;
	int iEndPacketReceived;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf_device::receive_report(): ppcResponse=%p, psizResponse=%p, pulStatus=%p\n", ppcResponse, psizResponse, pulStatus));

	/* Expect success. */
	iResult = PAPA_SCHLUMPF_ERROR_Ok;

	pcResponse = NULL;
	sizResponse = 0;
	iEndPacketReceived = 0;

	while( iEndPacketReceived==0 )
	{
		/* Receive data packets in 4096 byte chunks. */
		iLength = 4096;
		uiTimeoutMs = 10000;
		iLibUsbResult = libusb_bulk_transfer(m_ptLibUsbDeviceHandle, 0x81,(unsigned char *) uData.ac, iLength, &iTransfered, uiTimeoutMs);
		if( iLibUsbResult==LIBUSB_SUCCESS )
		{
			/* Is this the special end packet? */
			if( iTransfered==8 && uData.aul[0]==0xffffffff )
			{
				ulStatus = uData.aul[1];
				iEndPacketReceived = 1;
			}
			else if( iTransfered>0 )
			{
				/* Add the new chunk to the buffer. */
				if( pcResponse==NULL )
				{
					/* Allocate a new buffer. */
					pcResponseNew = (char*)malloc(iTransfered);
					if( pcResponseNew==NULL )
					{
						iResult = -1;
						break;
					}
					else
					{
						/* Copy the received data into the new buffer. */
						pcResponse = pcResponseNew;
						memcpy(pcResponse, uData.ac, iTransfered);
						sizResponse = iTransfered;
					}
				}
				else
				{
					sizResponseNew = sizResponse + iTransfered;
					/* Allocate a new buffer. */
					pcResponseNew = (char*)malloc(sizResponseNew);
					if( pcResponseNew==NULL )
					{
						iResult = -1;
						break;
					}
					else
					{
						/* Copy the old data into the new buffer. */
						memcpy(pcResponseNew, pcResponse, sizResponse);
						/* Append the received data to the new buffer. */
						memcpy(pcResponseNew+sizResponse, uData.ac, iTransfered);
						/* Free the old buffer. */
						free(pcResponse);
						/* Use the new buffer from now on. */
						pcResponse = pcResponseNew;
						sizResponse = sizResponseNew;
					}
				}
			}
		}
		else
		{
			iResult = -1;
			break;
		}
	}

	if( iResult!=0 )
	{
		if( pcResponse!=NULL )
		{
			free(pcResponse);
		}
		pcResponse = NULL;
		sizResponse = 0;
	}

	*ppcResponse = pcResponse;
	*psizResponse = sizResponse;
	*pulStatus = ulStatus;
	DEBUGMSG(ZONE_VERBOSE, ("romloader_papa_schlumpf_device::receive_report(): status=0x%08x\n", ulStatus));
	DEBUGMSG(ZONE_VERBOSE, ("romloader_papa_schlumpf_device::receive_report(): report=%s\n", pcResponse));

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf_device::receive_report(): iResult=%d\n", iResult));
	return iResult;
}



int romloader_papa_schlumpf_device::Connect(unsigned int uiBusNr, unsigned int uiDeviceAdr)
{
	int iResult;
	int iLibUsbResult;
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device **ptDevCnt, **ptDevEnd;
	libusb_device *ptDevice;
	libusb_device_handle *ptDevHandle;
	struct libusb_device_descriptor sDevDesc;
	unsigned int uiEntryBusNr;
	unsigned int uiEntryDeviceAdr;
	char *pcResponse;
	size_t sizResponse;
	uint32_t ulStatus;


	/* Expect success. */
	iResult = 0;

	/* Get the list of all connected USB devices. */
	ptDeviceList = NULL;
	ssizDevList = libusb_get_device_list(m_ptLibUsbContext, &ptDeviceList);
	if( ssizDevList<0 )
	{
		/* Failed to get the list of all USB devices. */
		fprintf(stderr, "%s: failed to detect USB devices: %ld:%s\n", m_pcPluginId, ssizDevList, libusb_strerror(libusb_error(ssizDevList)));
		iResult = -1;
	}
	else
	{
		/* Loop over all devices. */
		ptDevCnt = ptDeviceList;
		ptDevEnd = ptDevCnt + ssizDevList;
		while( ptDevCnt<ptDevEnd )
		{
			ptDevice = *ptDevCnt;

			/* Try to open the device. */
			iLibUsbResult = libusb_open(ptDevice, &ptDevHandle);
			if( iLibUsbResult==LIBUSB_SUCCESS )
			{
				/* Get the device descriptor. */
				iLibUsbResult = libusb_get_descriptor(ptDevHandle, LIBUSB_DT_DEVICE, 0, (unsigned char*)&sDevDesc, sizeof(struct libusb_device_descriptor));
				if( iLibUsbResult==sizeof(struct libusb_device_descriptor) )
				{
					printf("Looking at USB device VID=0x%04x, PID=0x%04x\n", sDevDesc.idVendor, sDevDesc.idProduct);

					/* Is this a "Papa Schlumpf" device? */
					if( sDevDesc.idVendor==PAPA_SCHLUMPF_USB_VENDOR_ID && sDevDesc.idProduct==PAPA_SCHLUMPF_USB_PRODUCT_ID )
					{
						printf("Found a Papa Schlumpf device!\n");

						/* Get the bus and device number */
						uiEntryBusNr = libusb_get_bus_number(ptDevice);
						uiEntryDeviceAdr = libusb_get_device_address(ptDevice);
						if( uiBusNr==uiEntryBusNr && uiDeviceAdr==uiEntryDeviceAdr )
						{
							/* Claim interface #0. */
							iLibUsbResult = libusb_claim_interface(ptDevHandle, 0);
							if( iLibUsbResult==LIBUSB_SUCCESS )
							{
								/* The handle to the device is now open. */
								m_ptLibUsbDeviceHandle = ptDevHandle;

								/* Test the PCIe connection. */
								/* Send a packet. */
								iResult = send_start_packet();
								if( iResult!=0 )
								{
									fprintf(stderr, "failed to send the start package\n");
									libusb_release_interface(ptDevHandle, 0);
									libusb_close(ptDevHandle);
								}
								else
								{
									/* Receive the response. */
									iResult = receive_report(&pcResponse, &sizResponse, &ulStatus);
									if( iResult!=0 )
									{
										fprintf(stderr, "failed to receive the report\n");
										libusb_release_interface(ptDevHandle, 0);
										libusb_close(ptDevHandle);
									}
									else
									{
										if( ulStatus!=0 )
										{
											fprintf(stderr, "the PCIE bus test failed!\n");
											fprintf(stderr, "Test report:\n");
											fwrite(pcResponse, sizResponse, 1, stderr);
											libusb_release_interface(ptDevHandle, 0);
											libusb_close(ptDevHandle);
											iResult = 0;
										}
									}
								}
							}
							else
							{
								fprintf(stderr, "%s: failed to claim the interface: %d:%s\n", m_pcPluginId, iLibUsbResult, libusb_strerror(libusb_error(iLibUsbResult)));
								libusb_close(ptDevHandle);
								iResult = -1;
							}
							break;
						}
					}
				}

				libusb_close(ptDevHandle);
			}

			/* Next device in the list... */
			++ptDevCnt;
		}

		/* Free the device list. */
		if( ptDeviceList!=NULL )
		{
			libusb_free_device_list(ptDeviceList, 1);
		}
	}

	return iResult;
}
