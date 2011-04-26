/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen and M. Trensch                 *
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

#include "romloader_uart_device_win.h"

#define DEFAULT_SERIAL_POLLTIMEOUT   500UL  /**< Time in ms to wait for overlapping IO in receiver thread */
#define THREAD_TERMINATE_TIME       2000UL  /**< Time to wait for RX thread to end, before killing it */

#include <stdio.h>
#include <string.h>
#include <tchar.h>

// Includes for port enumeration.
#include <Setupapi.h>
#include <devguid.h>

// defines for port enumeration
typedef DWORD (WINAPI *CM_Open_DevNode_Key)(DWORD, DWORD, DWORD, DWORD, ::PHKEY, DWORD);
#define RegDisposition_OpenExisting (0x00000001)   // open key only if exists
#define CM_REGISTRY_HARDWARE        (0x00000000)

/*****************************************************************************/
/*! Constructor                                   
*    \param szPortName Short name of the communication port
*    \param szPortDesc Human-Readable description of port
*    \param eType      Boot device type (detected by scan)                   */
/*****************************************************************************/
romloader_uart_device_win::romloader_uart_device_win(const char *pcPortName)
  : romloader_uart_device(pcPortName)
  , m_fRunning(false)
  , m_hPort(INVALID_HANDLE_VALUE)
  , m_hComStateThread(NULL)
  , m_hTxEmpty(NULL)
  , m_hNewRxEvent(NULL)
{
  m_hTxEmpty    = CreateEvent(NULL, FALSE, FALSE, NULL);
  m_hNewRxEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

/*****************************************************************************/
/*! Destructor                                                               */
/*****************************************************************************/
romloader_uart_device_win::~romloader_uart_device_win()
{
  Close();

  ::CloseHandle(m_hTxEmpty);
  ::CloseHandle(m_hNewRxEvent);
}

/*****************************************************************************/
/*! Thread checking the state of the COM port cyclically (Wrapper function)
*     \param pvParam Pointer to class object                     
*     \return Thread return value                                            */
/*****************************************************************************/
DWORD romloader_uart_device_win::CheckComStateThread(void* pvParam)
{
  romloader_uart_device_win* pcDev = reinterpret_cast<romloader_uart_device_win*>(pvParam);
  
  return pcDev->CheckComState();
}

/*****************************************************************************/
/*! Function checking the state of the COM port
*     \param dwEventMask Event mask of last state change                     */
/*****************************************************************************/
void romloader_uart_device_win::CheckComEvents(DWORD dwEventMask)
{
	DWORD   dwBytesRead;
	DWORD   dwCommError;
	DWORD   dwBufferSize;
	unsigned char *pucBuffer;
	COMSTAT tComstat    = {0};


  /* *** DEBUG *** Always check for a received char, EV_RXCHAR is not set sometimes and I don't know why! */
//  if(dwEventMask & EV_RXCHAR)
//  {
	//data is ready, fetch them from serial port
	dwBytesRead = 0;
	dwCommError = 0;
    
	::ClearCommError(m_hPort, &dwCommError, &tComstat);
   
	dwBufferSize = tComstat.cbInQue;
	if( dwBufferSize>0 )
	{
		pucBuffer = new unsigned char[dwBufferSize];
		memset(pucBuffer, 0, dwBufferSize);

		OVERLAPPED tovRead;
		memset(&tovRead, 0, sizeof(tovRead));
		tovRead.hEvent = ::CreateEvent(0,TRUE,0,0);
    
		//get the data from the queue
		BOOL fReadRet = ::ReadFile(m_hPort, pucBuffer, dwBufferSize, &dwBytesRead, &tovRead);
   
//		wxASSERT(fReadRet);

		::CloseHandle(tovRead.hEvent);

		if ( dwBytesRead > 0 )
		{
			writeCards(pucBuffer, dwBytesRead);

			// Signal read function that new data is available
			::SetEvent(m_hNewRxEvent);
		}

		delete[] pucBuffer;
	}
//  } 
  
	if(dwEventMask & EV_TXEMPTY)
	{
		//Signal waiting thread data has been written
		::SetEvent(m_hTxEmpty);
	}
}

/*****************************************************************************/
/*! Thread function checking the state of the COM port cyclically
*     \return Thread return value                                            */
/*****************************************************************************/
DWORD romloader_uart_device_win::CheckComState()
{
  OVERLAPPED tOvWait     = {0};


  tOvWait.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

  while(m_fRunning)
  {
    DWORD dwEventMask     = 0;
    bool  fEventAvailable = false;
    DWORD dwBla           = 0;

    tOvWait.Internal     = 0;
    tOvWait.InternalHigh = 0;
    tOvWait.Offset       = 0;
    tOvWait.OffsetHigh   = 0;

    ::ResetEvent(tOvWait.hEvent);

    if(WaitCommEvent(m_hPort, &dwEventMask, &tOvWait))
    {
      // no overlapped wait needed, data is ready
      fEventAvailable = true;
    } else
    {
      // Event not set, check if we are now in overlapped mode
      DWORD dwLastError = GetLastError();

      if(ERROR_IO_PENDING != dwLastError)
      {
        //no IO Event and not overlapped pending. This should never happen
        //To allow other threads to run, let us sleep until next try
        Sleep(10);
        
      } else
      {
        //we are in overlapped mode and wait comm event is pending, so check the overlapped event
        DWORD dwWaitRes = WaitForSingleObject(tOvWait.hEvent, DEFAULT_SERIAL_POLLTIMEOUT);
  
        if(WAIT_OBJECT_0 == dwWaitRes)
        {
          //WaitComEvent is now available, so we can check the COM state
          fEventAvailable = true;
        }
      }
    }

    //WaitComEvent succeeded, so check the state now
    if(fEventAvailable)
    {
      CheckComEvents(dwEventMask);
    }
  }

  ::CloseHandle(tOvWait.hEvent);

  return 0;
}


/*****************************************************************************/
/*! Send a raw data stream to the device
*    \param pbData    Pointer to data stream to be sent
*    \param ulDataLen Length of data stream
*    \param ulTimeout Timeout in ms to wait for send complete
*    \return Number of bytes that have been sent                             */
/*****************************************************************************/
size_t romloader_uart_device_win::SendRaw(const unsigned char *pucData, size_t sizData, unsigned long ulTimeout)
{
  bool  fRet           = false;
  DWORD dwBytesWritten = 0;
  DWORD dwError;

  //We are operating in overlapped mode
  OVERLAPPED tOverlap = {0};
  tOverlap.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

  //write the Data to the comport
  BOOL  fResult = ::WriteFile(m_hPort, pucData, sizData, &dwBytesWritten, &tOverlap);

  //check if everything has been written ok. if not, cancel transaction
  if(fResult)
  {
    if(dwBytesWritten == sizData)
      fRet = true;
    else
      ::CancelIo(m_hPort);

  } else
  {
    //we assume the write is still pending
    dwError = GetLastError();
    if( dwError != ERROR_IO_PENDING)
    {
//      wxASSERT(false);

    //check for completion of the write
    } else if(::WaitForSingleObject(tOverlap.hEvent, ulTimeout) == WAIT_OBJECT_0) 
    {
      ::GetOverlappedResult(m_hPort, &tOverlap, &dwBytesWritten, TRUE);
      
      //Check if not all bytes have been written
      if(dwBytesWritten == sizData)
        fRet = true;
      else
        ::CancelIo(m_hPort);
    }
  }

  ::CloseHandle(tOverlap.hEvent);

//  if(fRet)
//  {
//    //wait until txempty
//    DWORD dwWaitRes = WaitForSingleObject(m_hTxEmpty, 10000);
//    wxASSERT(dwWaitRes == WAIT_OBJECT_0);
//  }

  return dwBytesWritten;
}

/*****************************************************************************/
/*! Flushes any pending data
*    \return true on success                                                 */
/*****************************************************************************/
bool romloader_uart_device_win::Flush(void)
{
  return (TRUE == FlushFileBuffers(m_hPort));
}


/*****************************************************************************/
/*! Check if any data is ready to grab
*    \return number of waiting bytes                                         */
/*****************************************************************************/
unsigned long romloader_uart_device_win::Peek(void)
{
	return getCardSize();
}


/*****************************************************************************/
/*! Receive raw data from device
*    \param pbData    Pointer to receive buffer        
*    \param ulDataLen Length of data requested
*    \param ulTimeout Timeout in ms to wait for receive complete
*    \return Number of bytes that have been received                         */
/*****************************************************************************/
size_t romloader_uart_device_win::RecvRaw(unsigned char *pucData, size_t sizData, unsigned long ulTimeout)
{
	size_t sizCharsReceived;
	long lStartTime;
	unsigned long ulDiffTime;
	size_t sizLeft;
	size_t sizRead;
	DWORD dwWaitResult;


	sizCharsReceived = 0;
	lStartTime = (long)GetTickCount();
	do
	{
		sizLeft = sizData - sizCharsReceived;
		sizRead = readCards(pucData+sizCharsReceived, sizLeft);
		sizCharsReceived += sizRead;

		/* Check for timeout. */
		ulDiffTime = GetTickCount() - lStartTime;
		if( ulDiffTime >= ulTimeout )
		{
			/* Timeout! */
			break;
		}

		/* Sleep if we are not finished yet, to allow RX thread to run, if data is available. */
		if( sizCharsReceived!=sizData )
		{
			/* Wait for new data to arrive. */
			dwWaitResult = ::WaitForSingleObject(m_hNewRxEvent, ulTimeout-ulDiffTime);
			if( dwWaitResult!=WAIT_OBJECT_0 )
			{
				/* Timeout or error -> no new data arrived. */
				break;
			}
		}
	} while( sizCharsReceived!=sizData );

	return sizCharsReceived;
}

/*****************************************************************************/
/*! Closes any open connection                                               */
/*****************************************************************************/
void romloader_uart_device_win::Close(void)
{
  // Check if RX Thread is running, and terminate it
  if(NULL != m_hComStateThread)
  {
    m_fRunning = false;
    
    if(WaitForSingleObject(m_hComStateThread, THREAD_TERMINATE_TIME) == WAIT_TIMEOUT)
      ::TerminateThread(m_hComStateThread, MAXDWORD);

    m_hComStateThread = NULL;
  }

  // Check if the device is open, and close it
  if(m_hPort != INVALID_HANDLE_VALUE)
  {
    // Close it
    ::CloseHandle(m_hPort);
    // device is now closed
    m_hPort = INVALID_HANDLE_VALUE;
  }

	// delete the cards
	deleteCards();
}

/*****************************************************************************/
/*! Open the communication port (Needed before any interaction with this class
*    \return true on success                                                 */
/*****************************************************************************/
bool romloader_uart_device_win::Open()
{
	bool fResult = false;
	DCB tDcb;
	char acPath[MAX_PATH];


	/* Close any open connection. */
	if( m_hPort!=INVALID_HANDLE_VALUE )
	{
		Close();
	}

	/* Create the cards. */
	initCards();

	_snprintf(acPath, sizeof(acPath), "\\\\.\\%s", m_pcPortName);
  	/* Open the port. */
	m_hPort= ::CreateFile(acPath,
			      GENERIC_READ | GENERIC_WRITE,
	                      0,
	                      NULL,
	                      OPEN_EXISTING,
	                      FILE_FLAG_OVERLAPPED,
	                      NULL);
	if( m_hPort==INVALID_HANDLE_VALUE )
	{
		/* No -> exit with error. */
		fprintf(stderr, "failed to open the com port");
	}
	else if(!::GetCommState(m_hPort, &tDcb))
	{
		/* Read current settings. */
		fprintf(stderr, "failed to query com settings");
	}
	else
	{
		/* Apply settigs for netx debug monitor. */
		tDcb.BaudRate          = CBR_115200;
		tDcb.ByteSize          = 8;
		tDcb.Parity            = NOPARITY;
		tDcb.StopBits          = ONESTOPBIT;
		tDcb.fBinary           = true;
		tDcb.fParity           = false;
		tDcb.fOutxCtsFlow      = false;
		tDcb.fOutxDsrFlow      = false;
		tDcb.fDtrControl       = DTR_CONTROL_DISABLE;
		tDcb.fDsrSensitivity   = false;
		tDcb.fTXContinueOnXoff = false;
		tDcb.fOutX             = false;
		tDcb.fInX              = false;
		tDcb.fErrorChar        = false;
		tDcb.fNull             = false;
		tDcb.fRtsControl       = RTS_CONTROL_DISABLE;
		tDcb.fAbortOnError     = false;

		/* Set the communication parameters. */
		if( !::SetCommState(m_hPort, &tDcb) )
		{
			/* Failed to setup com. */
			fprintf(stderr, "failed to configure com");
		}
		/* Recommend rx and tx buffer sizes. */
		else if( !::SetupComm(m_hPort, 1024, 1024) )
		{
			fprintf(stderr, "failed to setup com");
		}
		/* Only monitor RXCHAR and TXEMPTY. */
		else if( !::SetCommMask(m_hPort, EV_RXCHAR | EV_TXEMPTY) )
		{
			fprintf(stderr, "failed to set com mask");
		}
		else if(NULL == (m_hComStateThread = CreateThread(NULL,
                                                        0,
                                                        CheckComStateThread,
                                                        this,
                                                        CREATE_SUSPENDED,
                                                        NULL)))
		{
			/* Create the receive thread. */
			fprintf(stderr, "Can't create receive thread!");
		}
		else
		{
			fResult     = true;
			// signal ready even if DTR and RTS is switched off, a virtual COM port needs this to send the data
			fResult &= (EscapeCommFunction(m_hPort, SETDTR) == TRUE);
			fResult &= (EscapeCommFunction(m_hPort, SETRTS) == TRUE);
		}
	}

	m_fRunning  = fResult;

	/* Close any open connection if something has failed. */
	if( fResult!=true )
	{
		Close();
	}
	else
	{
		ResumeThread(m_hComStateThread);
	}

	return fResult;
}


/*****************************************************************************/
/*! Scan all available RS232 ports and place them into vector
*    \param cvList  Reference to vector to place found devices in
*    \return Number of devices found                                         */
/*****************************************************************************/
unsigned long romloader_uart_device_win::ScanForPorts(char ***pppcDeviceNames)
{
	bool fOk;
	const GUID          tDevGuid          = GUID_DEVCLASS_PORTS;
	CM_Open_DevNode_Key pfnOpenDevNodeKey = NULL;
	SP_DEVINFO_DATA     tDeviceInfoData   = {0};
	HKEY                hkDevice          = NULL;
	HINSTANCE           hCfgMan           = NULL;
	HDEVINFO            hInfo             = NULL;
	const TCHAR*        szDeviceId        = "netX usb bootmonitor";
	DWORD dwDevCnt;
	size_t sizRefCnt;
	size_t sizRefMax;
	size_t sizEntry;
	char *pcRef;
	char **ppcRef;
	char **ppcRefNew;


	/* Be optimistic. */
	fOk = true;

	/* No device list allocated yet. */
	sizRefCnt = 0;
	sizRefMax = 0;
	ppcRef = NULL;

	/* Open the mfgmgr32 lib. */
	hCfgMan = LoadLibrary("cfgmgr32");
	if( hCfgMan==NULL )
	{
		fprintf(stderr, "Failed to open cfgmgr32, can not enumerate ports!");
		fOk = false;
	}
	else
	{
		pfnOpenDevNodeKey = (CM_Open_DevNode_Key)GetProcAddress(hCfgMan, "CM_Open_DevNode_Key");
		if( pfnOpenDevNodeKey==NULL )
		{
			fprintf(stderr, "Failed to get function pointer for 'CM_Open_DevNode_Key' from cfgmgr32, can not enumerate ports!");
			fOk = false;
		}
		else
		{
			hInfo = SetupDiGetClassDevs(&tDevGuid, NULL, NULL, DIGCF_PRESENT);
			if( hInfo==INVALID_HANDLE_VALUE )
			{
				fprintf(stderr, "failed to enumerate COMM devices.\n");
				fOk = false;
			}
			else
			{
				/* Init References array. */
				sizRefCnt = 0;
				sizRefMax = 16;
				ppcRef = (char**)malloc(sizRefMax*sizeof(char*));
				if( ppcRef==NULL )
				{
					fprintf(stderr, "out of memory!\n");
					fOk = false;
				}
				else
				{
					/* Count all devices. */
					tDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

					dwDevCnt = 0;
					while( SetupDiEnumDeviceInfo(hInfo, dwDevCnt, &tDeviceInfoData)!=0 )
					{
						TCHAR szPortName[20] = {0};
						DWORD dwPortNameLen  = sizeof(szPortName);

						/* First test if this interface is a com port. */
						if( (ERROR_SUCCESS == pfnOpenDevNodeKey(tDeviceInfoData.DevInst, 
				                                        KEY_QUERY_VALUE, 
				                                        0, 
				                                        RegDisposition_OpenExisting, 
				                                        &hkDevice, 
				                                        CM_REGISTRY_HARDWARE)) &&
						    (ERROR_SUCCESS == RegQueryValueEx(hkDevice,
									"Portname",
									NULL,
									NULL,
									(BYTE*)szPortName,
									&dwPortNameLen)) )
						{
							/* Is this a com port? */
							if(_tcsncmp(szPortName, "COM", 3)==0 )
							{
								/* Found a com port. */

								/* Is enough space in the array for one more entry? */
								if( sizRefCnt>=sizRefMax )
								{
									/* No -> expand the array. */
									sizRefMax *= 2;
									/* Detect overflow or limitation. */
									if( sizRefMax<=sizRefCnt )
									{
										fOk = false;
										break;
									}
									/* Reallocate the array. */
									ppcRefNew = (char**)realloc(ppcRef, sizRefMax*sizeof(char*));
									if( ppcRefNew==NULL )
									{
										fOk = false;
										break;
									}
									ppcRef = ppcRefNew;
								}
								sizEntry = strlen(szPortName) + 1;
								pcRef = (char*)malloc(sizEntry);
								if( pcRef==NULL )
								{
									fOk = false;
									break;
								}
								memcpy(pcRef, szPortName, sizEntry);
								ppcRef[sizRefCnt++] = pcRef;
								printf("Found COM port %s\n", pcRef);
							}
						}

						/* Close the device key. */
						RegCloseKey(hkDevice);

						/* Next device. */
						++dwDevCnt;
					}
				}

				/* Destroy temp data. */
				if( hInfo!=INVALID_HANDLE_VALUE )
				{
					SetupDiDestroyDeviceInfoList(hInfo);
				}
			}

			/* Free the cfgmgr32 library. */
			if( hCfgMan!=NULL )
			{
				FreeLibrary(hCfgMan);
			}
		}
	}

	if( fOk!=true )
	{
		/* Delete the complete array. */
		if( ppcRef!=NULL )
		{
			char **ppcCnt;
			char **ppcEnd;


			ppcCnt = ppcRef;
			ppcEnd = ppcRef + sizRefCnt;
			do
			{
				pcRef = *(ppcCnt++);
				if( pcRef!=NULL )
				{
					free(pcRef);
				}
			} while( ppcCnt<ppcEnd );
			free(ppcRef);
		}

		sizRefCnt = 0;
	}

	return sizRefCnt;
}


/*****************************************************************************/
/*! Cancels any pending send/receive functions
*    \return true on successful cancel request                               */
/*****************************************************************************/
bool romloader_uart_device_win::Cancel()
{
  return TRUE == ::CancelIo(m_hPort);
}

