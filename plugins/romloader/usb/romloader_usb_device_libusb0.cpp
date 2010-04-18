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


#include <stdio.h>

#include "romloader_usb_device_libusb0.h"
#include "romloader_usb_main.h"



#ifdef _WINDOWS
	const char *romloader_usb_device_libusb0::m_pcLibUsb_BusPattern = "bus-%u";
	const char *romloader_usb_device_libusb0::m_pcLibUsb_DevicePattern = "\\\\.\\libusb0-%u";
#else
	const char *romloader_usb_device_libusb0::m_pcLibUsb_BusPattern = "%u";
	const char *romloader_usb_device_libusb0::m_pcLibUsb_DevicePattern = "%u";

	#include <pthread.h>
#endif



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
	const size_t sizBufferSize = 4096;
	union
	{
		unsigned char auc[sizBufferSize];
		char ac[sizBufferSize];
	} uBuffer;


	/* Allow exit at cancellation points. */
	iError = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &iOldValue);
	if( iError==0 )
	{
		iError = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldValue);
		if( iError==0 )
		{
			do
			{
				iError = usb_bulk_read(ptDevHandle, ucEndPointIn, uBuffer.ac, sizBufferSize, 200);
				if( iError>0 )
				{
					/* transfer ok! */
					writeCards(uBuffer.auc, iError);
					iError = 0;
				}
				else if( iError==-110 )
				{
					/* Timeout. */
					iError = 0;
				}
			} while( iError>=0 );
		}
	}

	pthread_exit((void*)iError);
}












const char *romloader_usb_device_libusb0::m_pcPluginNamePattern = "romloader_usb_%02x_%02x";


int romloader_usb_device_libusb0::libusb_open(libusb_device *ptDevice, libusb_device_handle **pptDevHandle)
{
	libusb_device_handle *ptDevHandle;
	int iError;


	ptDevHandle = usb_open(ptDevice);
	if( ptDevHandle!=NULL )
	{
		/* Create the receive thread. */
		iError = pthread_create(&tRxThread, NULL, romloader_usb_device_libusb0::rxThread, (void*)this);
		if( iError!=0 )
		{
			usb_close(ptDevHandle);
			ptDevHandle = NULL;
			fprintf(stderr, "%s(%p): Failed to create card mutex: %d:%s\n", m_pcPluginId, this, errno, strerror(errno));
		}
	}
	else
	{
		ptDevHandle = NULL;
		iError = -1;
	}

	*pptDevHandle = ptDevHandle;
	return iError;
}


void romloader_usb_device_libusb0::libusb_close(libusb_device_handle *dev_handle)
{
	usb_close(dev_handle);
}


int romloader_usb_device_libusb0::libusb_init(libusb_context **pptContext)
{
	usb_init();

	/* use something different from NULL */
	*pptContext = (libusb_context*)1;

	return LIBUSB_SUCCESS;
}


void romloader_usb_device_libusb0::libusb_exit(libusb_context *ptContext)
{
}


void romloader_usb_device_libusb0::libusb_set_debug(libusb_context *ptContext, int iLevel)
{
}


int romloader_usb_device_libusb0::libusb_get_device_descriptor(libusb_device *dev, LIBUSB_DEVICE_DESCRIPTOR_T *desc)
{
	*desc = dev->descriptor;
	return LIBUSB_SUCCESS;
}


uint8_t romloader_usb_device_libusb0::libusb_get_bus_number(libusb_device *dev)
{
	uint8_t ucBusNr;
	usb_bus *ptBus;
	const char *pcBusName;
	int iResult;
	unsigned int uiBusNumber;


	/* expect failure */
	ucBusNr = 0xffU;

	/* is the device valid? */
	if( dev!=NULL )
	{
		/* does the device have a bus assigned? */
		ptBus = dev->bus;
		if( ptBus!=NULL )
		{
			/* does the bus have a directory name? */
			pcBusName = ptBus->dirname;
			if( pcBusName!=NULL )
			{
				/* parse the directory name */
				iResult = sscanf(pcBusName, m_pcLibUsb_BusPattern, &uiBusNumber);
				/* does the directory name have the expected format? */
				if( iResult==1 )
				{
					/* is the bus number in the valid range? */
					if( uiBusNumber<0x80U )
					{
						/* set the result */
						ucBusNr = (uint8_t)uiBusNumber;
					}
				}
			}
		}
	}

	return ucBusNr;
}


uint8_t romloader_usb_device_libusb0::libusb_get_device_address(libusb_device *dev)
{
	uint8_t ucDeviceAddress;
	const char *pcFilename;
	int iResult;
	unsigned int uiDeviceNumber;


	/* expect failure */
	ucDeviceAddress = 0xffU;

	/* is the device valid? */
	if( dev!=NULL )
	{
		/* does the bus have a directory name? */
		pcFilename = dev->filename;
		if( pcFilename!=NULL )
		{
			/* parse the directory name */
			iResult = sscanf(pcFilename, m_pcLibUsb_DevicePattern, &uiDeviceNumber);
			/* does the directory name have the expected format? */
			if( iResult==1 )
			{
				/* is the bus number in the valid range? */
				if( uiDeviceNumber<0x80U )
				{
					/* set the result */
					ucDeviceAddress = (uint8_t)uiDeviceNumber;
				}
			}
		}
	}

	return ucDeviceAddress;
}


int romloader_usb_device_libusb0::libusb_reset_device(libusb_device_handle *dev)
{
	return usb_reset(dev);
}


int romloader_usb_device_libusb0::libusb_set_configuration(libusb_device_handle *dev, int configuration)
{
	return usb_set_configuration(dev, configuration);
}


int romloader_usb_device_libusb0::libusb_claim_interface(libusb_device_handle *dev, int iface)
{
	return usb_claim_interface(dev, iface);
}


int romloader_usb_device_libusb0::libusb_release_interface(libusb_device_handle *dev, int iface)
{
	return usb_release_interface(dev, iface);
}


ssize_t romloader_usb_device_libusb0::libusb_get_device_list(libusb_device ***list)
{
	libusb_device **ptDeviceList;
	libusb_device **ptCnt;
	ssize_t ssizDeviceList;
	struct usb_bus *ptBusses;
	struct usb_bus *ptBusCnt;
	struct usb_device *ptDevCnt;


	/* detect all busses */
	usb_find_busses();
	/* detect all devices on each bus */
	usb_find_devices();

	/* init the device list */
	ptDeviceList = NULL;

	/* init the device counter */
	ssizDeviceList = 0;

	/* get the head of the bus list */
	ptBusses = usb_get_busses();

	/* loop over all bus entries and count the devices */
	ptBusCnt = ptBusses;
	while( ptBusCnt!=NULL )
	{
		/* loop over all devices on this bus */
		ptDevCnt = ptBusCnt->devices;
		while( ptDevCnt!=NULL )
		{
			/* found one more device */
			++ssizDeviceList;

			/* next device */
			ptDevCnt = ptDevCnt->next;
		}
		/* scan next bus */
		ptBusCnt = ptBusCnt->next;
	}

	/* found any devices? */
	if( ssizDeviceList>0 )
	{
		/* allocate the array */
		ptDeviceList = (libusb_device **)malloc(ssizDeviceList*sizeof(libusb_device*));
		if( ptDeviceList==NULL )
		{
			ssizDeviceList = LIBUSB_ERROR_NO_MEM;
		}
		else
		{
			ptCnt = ptDeviceList;

			/* loop over all bus entries and count the devices */
			ptBusCnt = ptBusses;
			while( ptBusCnt!=NULL )
			{
				/* loop over all devices on this bus */
				ptDevCnt = ptBusCnt->devices;
				while( ptDevCnt!=NULL )
				{
					/* add the device to the list */
					*(ptCnt++) = ptDevCnt;

					/* next device */
					ptDevCnt = ptDevCnt->next;
				}
				/* scan next bus */
				ptBusCnt = ptBusCnt->next;
			}

			*list = ptDeviceList;
		}
	}

	return ssizDeviceList;
}


void romloader_usb_device_libusb0::libusb_free_device_list(libusb_device **list, int unref_devices)
{
	/* free the complete list */
	free(list);
}


int romloader_usb_device_libusb0::libusb_reset_and_close_device(libusb_device_handle *dev)
{
	int iResult;


	iResult = libusb_reset_device(dev);
	if( iResult==LIBUSB_SUCCESS )
	{
		libusb_close(dev);
	}
	else if( iResult==LIBUSB_ERROR_NOT_FOUND )
	{
		// the old device is already gone -> that's good, ignore the error
		libusb_close(dev);
		iResult = LIBUSB_SUCCESS;
	}

	return iResult;
}


int romloader_usb_device_libusb0::libusb_release_and_close_device(libusb_device_handle *dev_handle)
{
	int iResult;


	/* release the interface */
	iResult = libusb_release_interface(dev_handle, 0);
	if( iResult!=LIBUSB_SUCCESS )
	{
		/* failed to release interface */
		printf("%s(%p): failed to release the usb interface: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}
	else
	{
		/* close the netx device */
		libusb_close(dev_handle);
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
	libusb_device_handle *ptDevHandle;
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
		ssizDevList = libusb_get_device_list(&ptDeviceList);
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
					iResult = libusb_open(ptDev, &ptDevHandle);
					if( iResult!=LIBUSB_SUCCESS )
					{
						/* failed to open the interface, do not add it to the list */
						printf("%s(%p): failed to open device %s: %d:%s\n", m_pcPluginId, this, acName, iResult, libusb_strerror(iResult));
					}
					else
					{
						/* set the configuration */
						iResult = libusb_set_configuration(ptDevHandle, 1);
						if( iResult!=LIBUSB_SUCCESS )
						{
							/* failed to set the configuration */
							printf("%s(%p): failed to set the configuration of device %s: %d:%s\n", m_pcPluginId, this, acName, iResult, libusb_strerror(iResult));
						}
						else
						{
							/* claim the interface, 0 is the interface number */
							iResult = libusb_claim_interface(ptDevHandle, 0);
							if( iResult!=LIBUSB_SUCCESS && iResult!=LIBUSB_ERROR_BUSY )
							{
								/* failed to claim the interface */
								printf("%s(%p): failed to claim the interface of device %s: %d:%s\n", m_pcPluginId, this, acName, iResult, libusb_strerror(iResult));
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
									iResult = libusb_release_interface(ptDevHandle, 0);
									if( iResult!=LIBUSB_SUCCESS )
									{
										/* failed to release the interface */
										printf("%s(%p): failed to release the interface of device %s after a successful claim: %d:%s\n", m_pcPluginId, this, acName, iResult, libusb_strerror(iResult));
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
						libusb_close(ptDevHandle);
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



void romloader_usb_device_libusb0::card_lock_enter(void)
{
	pthread_mutex_lock(&tCardMutex);
}

void romloader_usb_device_libusb0::card_lock_leave(void)
{
	pthread_mutex_unlock(&tCardMutex);
}


romloader_usb_device_libusb0::romloader_usb_device_libusb0(const char *pcPluginId)
 : m_pcPluginId(NULL)
 , m_ptFirstCard(NULL)
 , m_ptLastCard(NULL)
 , m_fCardMutexIsInitialized(false)
{
	int iResult;


	m_pcPluginId = strdup(pcPluginId);

	iResult = libusb_init(&m_ptLibUsbContext);
	if( iResult!=LIBUSB_SUCCESS )
	{
		/* failed to create the context */
		fprintf(stderr, "%s(%p): Failed to create libusb context: %d:%s\n", m_pcPluginId, this, iResult, libusb_strerror(iResult));
	}

	/* Create the card mutex. */
	iResult = pthread_mutex_init(&tCardMutex, NULL);
	if( iResult!=0 )
	{
		fprintf(stderr, "%s(%p): Failed to create card mutex: %d:%s\n", m_pcPluginId, this, errno, strerror(errno));
	}
	else
	{
		m_fCardMutexIsInitialized = true;
		initCards();
	}
}

romloader_usb_device_libusb0::~romloader_usb_device_libusb0(void)
{
	int iResult;


	if( m_fCardMutexIsInitialized==true )
	{
		deleteCards();

		iResult = pthread_mutex_destroy(&tCardMutex);
		if( iResult!=0 )
		{
			fprintf(stderr, "%s(%p): Failed to destroy card mutex: %d:%s\n", m_pcPluginId, this, errno, strerror(errno));
		}
	}

	if( m_ptLibUsbContext!=NULL )
	{
		/* free the libusb context */
		libusb_exit(m_ptLibUsbContext);
	}

	if( m_pcPluginId!=NULL )
	{
		free(m_pcPluginId);
	}
}


void romloader_usb_device_libusb0::initCards(void)
{
	tBufferCard *ptCard;


	if( m_ptFirstCard!=NULL )
	{
		deleteCards();
	}

	ptCard = new tBufferCard;
	ptCard->pucEnd = ptCard->aucData + mc_sizCardSize;
	ptCard->pucRead = ptCard->aucData;
	ptCard->pucWrite = ptCard->aucData;
	ptCard->ptNext = NULL;

	m_ptFirstCard = ptCard;
	m_ptLastCard = ptCard;
}


void romloader_usb_device_libusb0::deleteCards(void)
{
	tBufferCard *ptCard;
	tBufferCard *ptNextCard;


	/* Lock the cards. */
	card_lock_enter();

	ptCard = m_ptFirstCard;
	while( ptCard!=NULL )
	{
		ptNextCard = ptCard->ptNext;
		delete ptCard;
		ptCard = ptNextCard;
	}
	m_ptFirstCard = NULL;
	m_ptLastCard = NULL;

	/* Unlock the cards. */
	card_lock_leave();
}


void romloader_usb_device_libusb0::writeCards(const unsigned char *pucBuffer, size_t sizBufferSize)
{
	size_t sizLeft;
	size_t sizChunk;
	tBufferCard *ptCard;


	/* Lock the cards. */
	card_lock_enter();

	sizLeft = sizBufferSize;
	while( sizLeft>0 )
	{
		// get free space in the current card
		sizChunk = m_ptLastCard->pucEnd - m_ptLastCard->pucWrite;
		// no more space -> create a new card
		if( sizChunk==0 )
		{
			ptCard = new tBufferCard;
			ptCard->pucEnd = ptCard->aucData + mc_sizCardSize;
			ptCard->pucRead = ptCard->aucData;
			ptCard->pucWrite = ptCard->aucData;
			ptCard->ptNext = NULL;
			// append new card
			m_ptLastCard->ptNext = ptCard;
			// close old card
			m_ptLastCard->pucWrite = NULL;
			// set the new last pointer
			m_ptLastCard = ptCard;
			// the new card is empty
			sizChunk = mc_sizCardSize;
		}
		// limit chunk to request size
		if( sizChunk>sizLeft )
		{
			sizChunk = sizLeft;
		}
		// copy data
		memcpy(m_ptLastCard->pucWrite, pucBuffer, sizChunk);
		// advance pointer
		m_ptLastCard->pucWrite += sizChunk;
		pucBuffer += sizChunk;
		sizLeft -= sizChunk;
	}

	// unlock the cards
	card_lock_leave();
}


size_t romloader_usb_device_libusb0::readCards(unsigned char *pucBuffer, size_t sizBufferSize)
{
	size_t sizRead;
	size_t sizLeft;


	sizLeft = sizBufferSize;
	do
	{
		sizRead = readCardData(pucBuffer, sizLeft);
		pucBuffer += sizRead;
		sizLeft -= sizRead;
	} while( sizRead!=0 && sizLeft>0 );

	return sizBufferSize-sizLeft;
}


size_t romloader_usb_device_libusb0::readCardData(unsigned char *pucBuffer, size_t sizBufferSize)
{
	size_t sizRead;
	tBufferCard *ptOldCard;
	tBufferCard *ptNewCard;


	if( m_ptFirstCard==NULL )
	{
		sizRead = 0;
	}
	else if( m_ptFirstCard->pucWrite!=NULL )
	{
		// the first card is used by the write part -> lock the cards
		card_lock_enter();

		// get the number of bytes left in this card
		sizRead = m_ptFirstCard->pucWrite - m_ptFirstCard->pucRead;
		if( sizRead>sizBufferSize )
		{
			sizRead = sizBufferSize;
		}
		// card can be empty
		if( sizRead>0 )
		{
			// copy the data
			memcpy(pucBuffer, m_ptFirstCard->pucRead, sizRead);
			// advance the read pointer
			m_ptFirstCard->pucRead += sizRead;
		}

		// unlock the cards
		card_lock_leave();
	}
	else
	{
		// the first card is not used by the write part

		// get the number of bytes left in this card
		sizRead = m_ptFirstCard->pucEnd - m_ptFirstCard->pucRead;
		if( sizRead>sizBufferSize )
		{
			sizRead = sizBufferSize;
		}
		// card can be empty for overlapping buffer grow
		if( sizRead>0 )
		{
			// copy the data
			memcpy(pucBuffer, m_ptFirstCard->pucRead, sizRead);
			// advance the read pointer
			m_ptFirstCard->pucRead += sizRead;
		}
		// reached the end of the buffer?
		if( m_ptFirstCard->pucRead>=m_ptFirstCard->pucEnd )
		{
			// card is empty, move on to next card
			ptNewCard = m_ptFirstCard->ptNext;
			if( ptNewCard!=NULL )
			{
				// remember the empty card
				ptOldCard = m_ptFirstCard;
				// move to the new first card
				m_ptFirstCard = ptNewCard;
				// delete the empty card
				delete ptOldCard;
			}
		}
	}

	return sizRead;
}


size_t romloader_usb_device_libusb0::getCardSize(void) const
{
	size_t sizData;
	tBufferCard *ptCard;


	sizData = 0;
	ptCard = m_ptFirstCard;
	while( ptCard!=NULL )
	{
		if( ptCard->pucWrite==NULL )
		{
			sizData += m_ptFirstCard->pucEnd - m_ptFirstCard->pucRead;
		}
		else
		{
			sizData += m_ptFirstCard->pucWrite - m_ptFirstCard->pucRead;
		}
		ptCard = ptCard->ptNext;
	}

	return sizData;
}


bool romloader_usb_device_libusb0::fIsDeviceNetx(libusb_device *ptDevice)
{
	bool fDeviceIsNetx;
	int iResult;
	LIBUSB_DEVICE_DESCRIPTOR_T sDevDesc;


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

const char *romloader_usb_device_libusb0::libusb_strerror(int iError)
{
}


int romloader_usb_device_libusb0::usb_bulk_pc_to_netx(libusb_device_handle *ptDevHandle, unsigned char ucEndPointOut, const unsigned char *pucDataOut, int iLength, int *piProcessed, unsigned int uiTimeoutMs)
{
	int iError;
#ifdef _WINDOWS
	char *pcDataOut = (char*)pucDataOut;
#else
	const char *pcDataOut = (const char*)pucDataOut;
#endif


	iError = usb_bulk_write(ptDevHandle, ucEndPointOut, pcDataOut, iLength, uiTimeoutMs);
	if( iError==iLength )
	{
		/* transfer ok! */
		iError = 0;
	}
	else
	{
		/* do not return 0 in case of an error */
		if( iError==0 )
		{
			iError = -1;
		}
		else if( iError==-110 )
		{
			iError = LIBUSB_ERROR_TIMEOUT;
		}
		/* transfer failed */
		iLength = 0;
	}

	if( piProcessed!=NULL )
	{
		*piProcessed = iLength;
	}

	return iError;
}


int romloader_usb_device_libusb0::usb_bulk_netx_to_pc(libusb_device_handle *ptDevHandle, unsigned char ucEndPointIn, unsigned char *pucDataIn, int iLength, int *piProcessed, unsigned int uiTimeoutMs)
{
	int iError;


	iError = usb_bulk_read(ptDevHandle, ucEndPointIn, (char*)pucDataIn, iLength, uiTimeoutMs);
	if( iError>0 )
	{
		/* transfer ok! */
		if( piProcessed!=NULL )
		{
			*piProcessed = iError;
		}
		iError = 0;
	}
	else
	{
		/* do not return 0 in case of an error */
		if( iError==0 )
		{
			iError = -1;
		}
		else if( iError==-110 )
		{
			iError = LIBUSB_ERROR_TIMEOUT;
		}
		/* transfer failed */
		if( piProcessed!=NULL )
		{
			*piProcessed = 0;
		}
	}

	return iError;
}

