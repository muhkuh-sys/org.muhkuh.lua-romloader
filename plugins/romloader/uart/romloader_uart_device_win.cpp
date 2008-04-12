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

#include <wx/regex.h>

#define DEFAULT_SERIAL_POLLTIMEOUT   500UL  /**< Time in ms to wait for overlapping IO in receiver thread */
#define THREAD_TERMINATE_TIME       2000UL  /**< Time to wait for RX thread to end, before killing it */

// includes for port enumeration
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
romloader_uart_device_win::romloader_uart_device_win(wxString strPortName)
  : romloader_uart_device(strPortName)
  , m_fRunning(false)
  , m_hPort(INVALID_HANDLE_VALUE)
  , m_hComStateThread(NULL)
  , m_hTxEmpty(NULL)
  , m_hNewRxEvent(NULL)
  , m_pbRxBuffer(NULL)
  , m_ulRxBufferLen(0)
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
  if(dwEventMask & EV_RXCHAR)
  {
    //data is ready, fetch them from serial port
    DWORD   dwBytesRead = 0;
    DWORD   dwCommError = 0;
    COMSTAT tComstat    = {0};
    
    ::ClearCommError(m_hPort, &dwCommError, &tComstat);
   
    DWORD          dwBufferSize = tComstat.cbInQue;
    unsigned char* pbBuffer     = new unsigned char[dwBufferSize];
    memset(pbBuffer, 0, dwBufferSize);

    OVERLAPPED tovRead;
    memset(&tovRead, 0, sizeof(tovRead));
    tovRead.hEvent = ::CreateEvent(0,TRUE,0,0);
    
    //get the data from the queue
    BOOL fReadRet = ::ReadFile(m_hPort, pbBuffer, dwBufferSize, &dwBytesRead, &tovRead);
   
    wxASSERT(fReadRet);

    ::CloseHandle(tovRead.hEvent);

    if ( dwBytesRead > 0 )
    {
      //lock access to buffer, while resizing buffer
      wxCriticalSectionLocker locker(m_cRxBufferLock);

      unsigned long ulNewLen = m_ulRxBufferLen + dwBytesRead;

      //resize receive buffer to new length
      m_pbRxBuffer = (unsigned char*)realloc(m_pbRxBuffer, ulNewLen);
      memcpy(&m_pbRxBuffer[m_ulRxBufferLen], pbBuffer, dwBytesRead);
      m_ulRxBufferLen = ulNewLen;

      // Signal read function that new data is available
      ::SetEvent(m_hNewRxEvent);
    }

    delete [] pbBuffer;

  } 
  
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
  wxString   strMsg;

  tOvWait.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

  while(m_fRunning)
  {
    DWORD dwEventMask     = 0;
    bool  fEventAvailable = false;

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
unsigned long romloader_uart_device_win::SendRaw(const unsigned char *pbData, unsigned long ulDataLen, unsigned long ulTimeout)
{
  bool  fRet           = false;
  DWORD dwBytesWritten = 0;
  DWORD dwError;

  //We are operating in overlapped mode
  OVERLAPPED tOverlap = {0};
  tOverlap.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

  //write the Data to the comport
  BOOL  fResult = ::WriteFile(m_hPort, pbData, ulDataLen, &dwBytesWritten, &tOverlap);

  //check if everything has been written ok. if not, cancel transaction
  if(fResult)
  {
    if(dwBytesWritten == ulDataLen)
      fRet = true;
    else
      ::CancelIo(m_hPort);

  } else
  {
    //we assume the write is still pending
    dwError = GetLastError();
    if( dwError != ERROR_IO_PENDING)
    {
      wxASSERT(false);

    //check for completion of the write
    } else if(::WaitForSingleObject(tOverlap.hEvent, ulTimeout) == WAIT_OBJECT_0) 
    {
      ::GetOverlappedResult(m_hPort, &tOverlap, &dwBytesWritten, TRUE);
      
      //Check if not all bytes have been written
      if(dwBytesWritten == ulDataLen)
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
	return m_ulRxBufferLen;
}


/*****************************************************************************/
/*! Receive raw data from device
*    \param pbData    Pointer to receive buffer        
*    \param ulDataLen Length of data requested
*    \param ulTimeout Timeout in ms to wait for receive complete
*    \return Number of bytes that have been received                         */
/*****************************************************************************/
unsigned long romloader_uart_device_win::RecvRaw(unsigned char *pbData, unsigned long ulDataLen, unsigned long ulTimeout)
{
  unsigned long  ulCharsReceived = 0;
  long           lStartTime      = (long)GetTickCount();

  do
  {
    unsigned long ulDiffTime = GetTickCount() - lStartTime;

    // Lock Access to RX buffer
    m_cRxBufferLock.Enter();

    unsigned long ulReadLen = __min(m_ulRxBufferLen,
                                    ulDataLen - ulCharsReceived);

    //if data is available, copy them from the RX buffer and resize RX buffer
    if(ulReadLen > 0)
    {
      memcpy(&pbData[ulCharsReceived], m_pbRxBuffer, ulReadLen);
      
      memmove(m_pbRxBuffer, m_pbRxBuffer + ulReadLen, m_ulRxBufferLen - ulReadLen);
  
      m_ulRxBufferLen -= ulReadLen;
      m_pbRxBuffer     = (unsigned char*)realloc(m_pbRxBuffer, m_ulRxBufferLen);
    }

    //unlock access to RX buffer (allows RX Thread to place data in the buffer
    m_cRxBufferLock.Leave();

    ulCharsReceived += ulReadLen;

    if(ulDiffTime >= ulTimeout)
      break;

    //sleep if we are not finished yet, to allow RX thread to run, if data is available
    if(ulCharsReceived != ulDataLen)
      Sleep(1);

  } while(ulCharsReceived != ulDataLen);

  return ulCharsReceived;
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

  //Free pending RX Buffer
  m_cRxBufferLock.Enter();
  m_ulRxBufferLen = 0;
  free(m_pbRxBuffer);
  m_pbRxBuffer = NULL;
  m_cRxBufferLock.Leave();
}

/*****************************************************************************/
/*! Open the communication port (Needed before any interaction with this class
*    \return true on success                                                 */
/*****************************************************************************/
bool romloader_uart_device_win::Open()
{
  bool fResult = false;
  const wxChar *pcFilename;

  // close any open connection
  if(m_hPort != INVALID_HANDLE_VALUE)
  {
    Close();
  }

  wxString strName(m_strPortName);
  // append windows specific prefix
  strName.Prepend(wxT("\\\\.\\"));
  // try to open the given port
  pcFilename = strName.c_str();
  m_hPort= ::CreateFile(pcFilename,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        NULL);

  DCB tDcb;

  // port open?
  if( m_hPort == INVALID_HANDLE_VALUE )
  {
    // no -> exit with error
    wxLogError(wxT("failed to open the com port"));

  } else if(!::GetCommState(m_hPort, &tDcb))
  {
    // read current settings
      wxLogError(wxT("failed to query com settings"));
  } else
  {
      
      // modify settigs for netx debug monitor
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

    //Set our communication parameters
    if(!::SetCommState(m_hPort, &tDcb))
    {
      // failed to setup com
      wxLogError(wxT("failed to configure com"));
      
    } else if(!::SetupComm(m_hPort, 1024, 1024))
    {
      // recommend rx and tx buffer sizes
      wxLogError(wxT("failed to setup com"));
      
    } else if(!::SetCommMask(m_hPort, EV_RXCHAR | EV_TXEMPTY))
    {
      //only monitor RXCHAR and TXEMPTY
      wxLogError(wxT("failed to set com mask"));
      
    } else if(NULL == (m_hComStateThread = CreateThread(NULL,
                                                        0,
                                                        CheckComStateThread,
                                                        this,
                                                        CREATE_SUSPENDED,
                                                        NULL)))
    {
      // create the receive thread
      wxLogError(wxT("Can't create receive thread!"));
      
    } else
    {
      fResult     = true;
      // signal ready even if DTR and RTS is switched off, a virtual COM port needs this to send the data
      fResult = EscapeCommFunction(m_hPort, SETDTR);
      fResult = EscapeCommFunction(m_hPort, SETRTS);
    }
  }

  m_fRunning  = fResult;

  // close any open connection if something has failed
  if(!fResult)
  {
    Close();
  } else
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
unsigned long romloader_uart_device_win::ScanForPorts(wxArrayString *ptArray)
{
  // get the GUID for serial ports
  const GUID          tDevGuid          = GUID_DEVCLASS_PORTS;
  CM_Open_DevNode_Key pfnOpenDevNodeKey = NULL;
  SP_DEVINFO_DATA     tDeviceInfoData   = {0};
  HKEY                hkDevice          = NULL;
  HINSTANCE           hCfgMan           = NULL;
  HDEVINFO            hInfo             = NULL;
  const TCHAR*        szDeviceId        = _T("netX usb bootmonitor");
  DWORD               dwDevCnt          = 0;
  wxString            str;


  // open the mfgmgr32 lib
  if(NULL == (hCfgMan = LoadLibrary(_T("cfgmgr32"))))
  {
    wxLogError(wxT("Failed to open cfgmgr32, can not enumerate ports!"));
    
  } else if(NULL == (pfnOpenDevNodeKey = (CM_Open_DevNode_Key)GetProcAddress(hCfgMan, "CM_Open_DevNode_Key")))
  {
    wxLogError(wxT("Failed to get function pointer for 'CM_Open_DevNode_Key' from cfgmgr32, can not enumerate ports!"));

  } else if(INVALID_HANDLE_VALUE == (hInfo = SetupDiGetClassDevs(&tDevGuid, NULL, NULL, DIGCF_PRESENT)))
  {
    // failed to enumerate COMM devices
  } else
  {
    // loop over all devices
    tDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    while(0 != SetupDiEnumDeviceInfo(hInfo, dwDevCnt, &tDeviceInfoData))
    {
      TCHAR szPortName[20] = {0};
      DWORD dwPortNameLen  = sizeof(szPortName);

      // first test if this interface is a com port
      if( (ERROR_SUCCESS == pfnOpenDevNodeKey(tDeviceInfoData.DevInst, 
                                          KEY_QUERY_VALUE, 
                                          0, 
                                          RegDisposition_OpenExisting, 
                                          &hkDevice, 
                                          CM_REGISTRY_HARDWARE)) &&
        (ERROR_SUCCESS == RegQueryValueEx(hkDevice, 
                                          _T("Portname"), 
                                          NULL, 
                                          NULL, 
                                          (BYTE*)szPortName, 
                                          &dwPortNameLen)) )
      {

        // is this a com port?
        if(_tcsncmp(szPortName, _T("COM"), 3)==0 )
        {
          // found a com port
          wxLogMessage(wxT("Found COM port ") + wxString::FromAscii(szPortName));
	  ptArray->Add(szPortName);
        }
      }
      // close the device key
      RegCloseKey(hkDevice);

      // next device
      ++dwDevCnt;
    }

    // destroy temp data
    if( hInfo != INVALID_HANDLE_VALUE )
    {
        SetupDiDestroyDeviceInfoList(hInfo);
    }
  }

    // free the cfgmgr32 library
  if(NULL != hCfgMan)
      FreeLibrary(hCfgMan);

  return dwDevCnt - 1;
}


/*****************************************************************************/
/*! Cancels any pending send/receive functions
*    \return true on successful cancel request                               */
/*****************************************************************************/
bool romloader_uart_device_win::Cancel()
{
  return TRUE == ::CancelIo(m_hPort);
}

