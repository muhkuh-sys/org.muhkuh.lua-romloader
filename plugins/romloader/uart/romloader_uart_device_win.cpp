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
#include <setupapi.h>
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
  , m_pvCallbackUserData(NULL)
  , m_tReceiveStatus(romloader_uart_device_win::RECEIVESTATUS_Idle)
  , m_hEventSendData(NULL)
{
	m_hTxEmpty       = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hNewRxEvent    = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventSendData = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

/*****************************************************************************/
/*! Destructor                                                               */
/*****************************************************************************/
romloader_uart_device_win::~romloader_uart_device_win()
{
	Close();

	::CloseHandle(m_hTxEmpty);
	::CloseHandle(m_hNewRxEvent);
	::CloseHandle(m_hEventSendData);
}


void romloader_uart_device_win::print_error(void)
{
	char *pcMsg = NULL;
	DWORD dwError;


	dwError = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	              0,
	              dwError,
	              0,
	              pcMsg,
	              1024,
	              NULL);
	if( pcMsg!=NULL )
	{
		fprintf(stderr, "Error %d: %s\n", dwError, pcMsg);
		LocalFree(pcMsg);
	}
	else
	{
		fprintf(stderr, "Error %d.\n", dwError);
	}
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
/*! Thread function checking the state of the COM port cyclically
*     \return Thread return value                                            */
/*****************************************************************************/
DWORD romloader_uart_device_win::CheckComState()
{
	DWORD dwThreadResult;
	BOOL fOK;
	DWORD dwEventMask;
	DWORD dwResult;
	DWORD dwCommError;
	DWORD dwBytesRead;
	DWORD dwCharsInBuffer;
	unsigned char *pucBuffer;
	OVERLAPPED tOvWait = {0};
	OVERLAPPED tOvRead = {0};
	COMSTAT tComstat = {0};


	/* Be optimistic. */
	dwThreadResult = ERROR_SUCCESS;

	pucBuffer = NULL;
	dwBytesRead = 0;

	tOvWait.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if( tOvWait.hEvent==NULL )
	{
		dwThreadResult = ::GetLastError();

		fprintf(stderr, "%s(%d): Failed to create the event for tOvWait.\n", __FILE__, __LINE__);
		print_error();
	}
	else
	{
		tOvRead.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if( tOvRead.hEvent==NULL )
		{
			dwThreadResult = ::GetLastError();

			::CloseHandle(tOvWait.hEvent);

			fprintf(stderr, "%s(%d): Failed to create the event for tOvRead.\n", __FILE__, __LINE__);
			print_error();
		}
		else
		{
			while(m_fRunning)
			{
				switch(m_tReceiveStatus)
				{
				case RECEIVESTATUS_Idle:
//					fprintf(stderr, "%s(%d): RECEIVESTATUS_Idle\n", __FILE__, __LINE__);

					/* Request the communication events. */
					fOK = ::WaitCommEvent(m_hPort, &dwEventMask, &tOvWait);
					if( fOK==0 )
					{
						/* Check what happened. */
						dwResult = ::GetLastError();
						if( dwResult==ERROR_IO_PENDING )
						{
							/* The operation did not finish yet. */
//							fprintf(stderr, "%s(%d): WaitCommEvent returned ERROR_IO_PENDING\n", __FILE__, __LINE__);
							m_tReceiveStatus = RECEIVESTATUS_WaitForCommEventOverlap;
						}
						else
						{
							/* Failed to get the COMM event. */
							print_error();
							fprintf(stderr, "%s(%d): WaitCommEvent failed.\n", __FILE__, __LINE__);
							m_tReceiveStatus = RECEIVESTATUS_Error;
						}
					}
					else
					{
						/* The request was processed immediately.
						 * The event mask "dwEventMask" has values now.
						 */
						m_tReceiveStatus = RECEIVESTATUS_ProcessCommEvents;
					}
					break;


				case RECEIVESTATUS_WaitForCommEventOverlap:
//					fprintf(stderr, "%s(%d): RECEIVESTATUS_WaitForCommEventOverlap\n", __FILE__, __LINE__);

					/* The request for the communication events has not finished yet.
					 * Wait for it.
					 */
					dwResult = ::WaitForSingleObject(tOvWait.hEvent, m_PollingTimeoutForCommEventsInMs);
//					fprintf(stderr, "%s(%d): WaitForSingleObject returned %d\n", __FILE__, __LINE__, dwResult);
					switch(dwResult)
					{
					case WAIT_OBJECT_0:
						fOK = ::GetOverlappedResult(m_hPort, &tOvWait, &dwResult, FALSE);
//						fprintf(stderr, "%s(%d): GetOverlappedResult returned %d\n", __FILE__, __LINE__, fOK);
						if( fOK!=0 )
						{
//							fprintf(stderr, "%s(%d): dwResult=%d\n", __FILE__, __LINE__, dwResult);

							/* GetOverlappedResult returned the result of the WaitCommEvent function. */
							if( dwResult!=0 )
							{
								/* The event mask "dwEventMask" has values now. */
								m_tReceiveStatus = RECEIVESTATUS_ProcessCommEvents;
							}
							else
							{
								/* The WaitCommEvent function failed. */
								print_error();
								fprintf(stderr, "%s(%d): The WaitCommEvent function failed after a wait.\n", __FILE__, __LINE__);
								m_tReceiveStatus = RECEIVESTATUS_Error;
							}
						}
						else
						{
							/* Failed to get the result of the overlapped read operation. */
							print_error();
							fprintf(stderr, "%s(%d): Failed to get the result of the overlapped read operation.\n", __FILE__, __LINE__);
							m_tReceiveStatus = RECEIVESTATUS_Error;
						}
						break;

					case WAIT_TIMEOUT:
						/* Continue to wait for an event. */
						break;

					default:
						/* Error, stop. */
						print_error();
						fprintf(stderr, "%s(%d): WaitForSingleObject failed in state RECEIVESTATUS_WaitForCommEventOverlap\n", __FILE__, __LINE__);
						m_tReceiveStatus = RECEIVESTATUS_Error;
						break;
					}
					break;


				case RECEIVESTATUS_ProcessCommEvents:
//					fprintf(stderr, "%s(%d): RECEIVESTATUS_ProcessCommEvents\n", __FILE__, __LINE__);

					/* Check the event for received chars. */
					if( (dwEventMask&EV_RXCHAR)==0 )
					{
						/* No chars were received. Go back to the idle state. */
						m_tReceiveStatus = RECEIVESTATUS_Idle;
					}
					else
					{
						/* The receive buffer contains some chars. Find out how many. */
						fOK = ::ClearCommError(m_hPort, &dwCommError, &tComstat);
//						fprintf(stderr, "%s(%d): ClearCommError returned %d\n", __FILE__, __LINE__, fOK);
						if( fOK!=0 )
						{
							dwCharsInBuffer = tComstat.cbInQue;
							if( dwCharsInBuffer>0 )
							{
								/* Remove any old buffer. */
								if( pucBuffer!=NULL )
								{
									delete[] pucBuffer;
									pucBuffer = NULL;
								}
								dwBytesRead = 0;

								/* Create a new buffer for the received data. */
								pucBuffer = new unsigned char[dwCharsInBuffer];
								if( pucBuffer==NULL )
								{
									print_error();
									fprintf(stderr, "%s(%d): Failed to allocate %d bytes.\n", __FILE__, __LINE__, dwCharsInBuffer);
									m_tReceiveStatus = RECEIVESTATUS_Error;
								}
								else
								{
									/* Get the data from the receive buffer.
									 *
									 * NOTE: Get the number of bytes read with GetOverlappedResult.
									 * See "Remarks" here: https://msdn.microsoft.com/en-us/library/windows/desktop/aa365467(v=vs.85).aspx
									 *
									 * "If hFile was opened with FILE_FLAG_OVERLAPPED, the following conditions are in effect:
									 *  ...
									 *  * The lpNumberOfBytesRead parameter should be set to NULL. Use the GetOverlappedResult function to get the actual number of bytes read. ..."
									 */
									fOK = ::ReadFile(m_hPort, pucBuffer, dwCharsInBuffer, NULL, &tOvRead);
									if( fOK!=0 )
									{
										/* The read request finished immediately.
										 * Get the number of bytes read.
										 */
										fOK = ::GetOverlappedResult(m_hPort, &tOvRead, &dwBytesRead, FALSE);
										if( fOK!=0 )
										{
											m_tReceiveStatus = RECEIVESTATUS_ProcessData;
										}
										else
										{
											print_error();
											fprintf(stderr, "%s(%d): Failed to get the result of the overlapped read operation.\n", __FILE__, __LINE__);
											m_tReceiveStatus = RECEIVESTATUS_Error;
										}
									}
									else
									{
										dwResult = ::GetLastError();
										if( dwResult==ERROR_IO_PENDING )
										{
											/* The read request is still in progress. */
											m_tReceiveStatus = RECEIVESTATUS_WaitForDataOverlap;
										}
										else
										{
											print_error();
											fprintf(stderr, "%s(%d): ReadFile failed.\n", __FILE__, __LINE__);
											m_tReceiveStatus = RECEIVESTATUS_Error;
										}
									}
								}
							}
						}
						else
						{
							print_error();
							fprintf(stderr, "%s(%d): ClearCommError failed.\n", __FILE__, __LINE__);
							m_tReceiveStatus = RECEIVESTATUS_Error;
						}
					}
					break;


				case RECEIVESTATUS_WaitForDataOverlap:
//					fprintf(stderr, "%s(%d): RECEIVESTATUS_WaitForDataOverlap\n", __FILE__, __LINE__);

					/* Wait for the read operation to finish. */
					dwResult = ::WaitForSingleObject(tOvRead.hEvent, m_PollingTimeoutForReadInMs);
					switch(dwResult)
					{
					case WAIT_OBJECT_0:
						fOK = ::GetOverlappedResult(m_hPort, &tOvRead, &dwBytesRead, FALSE);
						if( fOK!=0 )
						{
							m_tReceiveStatus = RECEIVESTATUS_ProcessData;
						}
						else
						{
							print_error();
							fprintf(stderr, "%s(%d): Failed to get the result of the overlapped read operation.\n", __FILE__, __LINE__);
							m_tReceiveStatus = RECEIVESTATUS_Error;
						}
						break;

					case WAIT_TIMEOUT:
						/* Continue to wait for the data. */
						break;

					default:
						/* Error, stop. */
						print_error();
						fprintf(stderr, "%s(%d): WaitForSingleObject failed in state RECEIVESTATUS_WaitForDataOverlap\n", __FILE__, __LINE__);
						m_tReceiveStatus = RECEIVESTATUS_Error;
						break;
					}
					break;


				case RECEIVESTATUS_ProcessData:
//					fprintf(stderr, "%s(%d): RECEIVESTATUS_ProcessData\n", __FILE__, __LINE__);

					if( pucBuffer!=NULL )
					{
						if( dwBytesRead>0 )
						{
//							fprintf(stderr, "%s(%d): Received %d bytes.\n", __FILE__, __LINE__, dwBytesRead);

							writeCards(pucBuffer, dwBytesRead);

							// Signal read function that new data is available
							::SetEvent(m_hNewRxEvent);
						}

						delete[] pucBuffer;
						pucBuffer = NULL;
					}
					dwBytesRead = 0;

					/* Move back to the idle state. */
					m_tReceiveStatus = RECEIVESTATUS_Idle;

					break;


				case RECEIVESTATUS_Error:
					/* Stay in this state until the main thread gives up. */
					break;
				}
			}

			::CloseHandle(tOvRead.hEvent);
			::CloseHandle(tOvWait.hEvent);

			if( pucBuffer!=NULL )
			{
				delete[] pucBuffer;
			}
		}
	}

	return dwThreadResult;
}


/*****************************************************************************/
/*! Send a raw data stream to the device
*    \param pbData    Pointer to data stream to be sent
*    \param ulDataLen Length of data stream
*    \param ulTimeout Timeout in ms to wait for send complete
*    \return Number of bytes that have been sent                             */
/*****************************************************************************/
size_t romloader_uart_device_win::SendRaw(const void *pvData, size_t sizData, unsigned long ulTimeout)
{
	bool  fRet;
	DWORD dwBytesWritten;
	DWORD dwResult;
	BOOL fResult;


//	fprintf(stderr, "%s(%d): Sending %d bytes.\n", __FILE__, __LINE__, sizData);

	/* Be pessimistic. */
	fRet = false;

	/* Create an overlap control. */
	OVERLAPPED tOverlapWrite = {0};
	tOverlapWrite.hEvent = m_hEventSendData;

	/* Write the data to the port. */
	fResult = ::WriteFile(m_hPort, pvData, sizData, NULL, &tOverlapWrite);
//	fprintf(stderr, "%s(%d): WriteFile returned %d.\n", __FILE__, __LINE__, fResult);
	if( fResult!=0 )
	{
		/* The write request finished immediately.
		 * Get the number of bytes written.
		 */
		fResult = ::GetOverlappedResult(m_hPort, &tOverlapWrite, &dwBytesWritten, FALSE);
//		fprintf(stderr, "%s(%d): GetOverlappedResult returned %d.\n", __FILE__, __LINE__, fResult);
		if( fResult!=0 )
		{
//			fprintf(stderr, "%s(%d): dwBytesWritten=%d.\n", __FILE__, __LINE__, dwBytesWritten);
			/* Were all bytes written? */
			if( dwBytesWritten==sizData )
			{
				/* Yes -> success. */
				fRet = true;
			}
			else
			{
				/* No -> cancel any pending operations. */
				fprintf(stderr, "%s(%d): Requested to write %d bytes, but only %d were processed.\n", __FILE__, __LINE__, sizData, dwBytesWritten);
				::CancelIo(m_hPort);
			}
		}
		else
		{
			print_error();
			fprintf(stderr, "%s(%d): Failed to get the result of the overlapped write operation.\n", __FILE__, __LINE__);
		}
	}
	else
	{
		dwResult = ::GetLastError();
		if( dwResult==ERROR_IO_PENDING )
		{
			/* Wait for the write operation to finish. */
			dwResult = ::WaitForSingleObject(tOverlapWrite.hEvent, ulTimeout);
			switch(dwResult)
			{
			case WAIT_OBJECT_0:
				fResult = ::GetOverlappedResult(m_hPort, &tOverlapWrite, &dwBytesWritten, FALSE);
				if( fResult!=0 )
				{
					/* Were all bytes written? */
					if( dwBytesWritten==sizData )
					{
						/* Yes -> success. */
						fRet = true;
					}
					else
					{
						/* No -> cancel any pending operations. */
						fprintf(stderr, "%s(%d): Requested to write %d bytes, but only %d were processed.\n", __FILE__, __LINE__, sizData, dwBytesWritten);
						::CancelIo(m_hPort);
					}
				}
				else
				{
					print_error();
					fprintf(stderr, "%s(%d): Failed to get the result of the overlapped read operation.\n", __FILE__, __LINE__);
				}
				break;

			case WAIT_TIMEOUT:
				fprintf(stderr, "%s(%d): Failed to send %d bytes in %d ms: timeout\n", __FILE__, __LINE__, sizData, ulTimeout);
				break;

			default:
				/* Error, stop. */
				print_error();
				fprintf(stderr, "%s(%d): WaitForSingleObject failed\n", __FILE__, __LINE__);
				break;
			}
		}
		else
		{
			print_error();
			fprintf(stderr, "%s(%d): WriteFile failed.\n", __FILE__, __LINE__);
		}
	}

	if( fRet!=true )
	{
		dwBytesWritten = 0;
	}

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


	m_ptLog->debug("Debug(0). RecvRaw");
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
		    m_ptLog->debug("Debug(1) Timeout");
			break;
		}

		/* Sleep if we are not finished yet, to allow RX thread to run, if data is available. */
		m_ptLog->debug("Debug(2) sizCharsReceived = %d", sizData);
		if( sizCharsReceived!=sizData )
		{
			/* Wait for new data to arrive. */
			dwWaitResult = ::WaitForSingleObject(m_hNewRxEvent, ulTimeout-ulDiffTime);

			if( dwWaitResult!=WAIT_OBJECT_0 )
			{
				/* Timeout or error -> no new data arrived. */
		        m_ptLog->debug("Debug(3) Timeout");
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
		{
			::TerminateThread(m_hComStateThread, MAXDWORD);
		}

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
//		else if( !::SetCommMask(m_hPort, EV_RXCHAR | EV_TXEMPTY) )
		/* Only monitor RXCHAR. */
		else if( !::SetCommMask(m_hPort, EV_RXCHAR) )
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
	const char *pcPluginName = "romloader_uart_%s";


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
								/* Get the size of the plugin name in bytes and add one byte for the trailing 0. */
								sizEntry = _snprintf(NULL, 0, pcPluginName, szPortName) + 1;
								pcRef = (char*)malloc(sizEntry);
								if( pcRef==NULL )
								{
									fOk = false;
									break;
								}
								_snprintf(pcRef, sizEntry, pcPluginName, szPortName);
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

	if( fOk==true )
	{
		*pppcDeviceNames = ppcRef;
	}
	else
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

