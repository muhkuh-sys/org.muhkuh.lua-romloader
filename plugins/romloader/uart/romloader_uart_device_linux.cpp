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


#include <wx/dir.h>
#include <wx/filename.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>


#include "romloader_uart_device_linux.h"


romloader_uart_device_linux::romloader_uart_device_linux(wxString strPortName)
  : romloader_uart_device(strPortName)
  , m_hPort(-1)
  , m_ptConditionRxDataAvail(NULL)
  , m_ptRxThread(NULL)
{
	m_ptConditionRxDataAvail = new wxCondition(mutexRxDataAvail);
}

romloader_uart_device_linux::~romloader_uart_device_linux()
{
	Close();

	// free the conditions
	if( m_ptConditionRxDataAvail!=NULL )
	{
		delete m_ptConditionRxDataAvail;
	}
}


bool romloader_uart_device_linux::Open(void)
{
	bool fResult;
	const wxChar *pcFilename;
	int iRes;
	struct termios tNewAttribs = {0};
	pid_t pidRx;
	int iRxExitCode;


	// expect failure
	fResult = false;

	// close any open connection
	Close();

	// lock the "data available" mutex
	mutexRxDataAvail.Lock();

	// create the cards
	initCards();

	// try to open the connection
	m_hPort = open(m_strPortName.fn_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	// port open?
	if( m_hPort==-1 )
	{
		// no -> exit with error
		wxLogError(_("failed to open the com port %s: %s"), m_strPortName.fn_str(), strerror(errno));
	}
	else
	{
		// save current port settings so commands are interpreted right for this program
		tcgetattr(m_hPort, &m_tOldAttribs);

		// set new port settings for non-canonical input processing  //must be NOCTTY
		cfmakeraw(&tNewAttribs);
		tNewAttribs.c_cflag |= CREAD | CLOCAL;
		tNewAttribs.c_cflag &= ~CRTSCTS;

		iRes = cfsetispeed(&tNewAttribs, B115200);
		iRes = cfsetospeed(&tNewAttribs, B115200);

		iRes = tcsetattr(m_hPort, TCSAFLUSH, &tNewAttribs);

		// create a new receive thread
		m_ptRxThread = new rxThread(m_hPort, this);
		m_ptRxThread->Create();
//		m_ptRxThread->SetPriority(WXTHREAD_MAX_PRIORITY);
		m_ptRxThread->Run();

		fResult = true;
	}

	return fResult;
}


void romloader_uart_device_linux::Close(void)
{
	pid_t pidTerm;
	int iStatus;


	if( m_hPort!=-1 )
	{
		tcsetattr(m_hPort, TCSANOW, &m_tOldAttribs);
		close(m_hPort);

		m_hPort = -1;
	}

	// stop rx thread
	if( m_ptRxThread!=NULL )
	{
		m_ptRxThread->Wait();
		delete m_ptRxThread;
	}

	// delete the cards
	deleteCards();
}


unsigned long romloader_uart_device_linux::SendRaw(const unsigned char *pbData, unsigned long ulDataLen, unsigned long ulTimeout)
{
	ssize_t ssizBytesWritten;
	size_t sizChunk;
	unsigned long ulBytesWritten;
	int iErrno;


	ulBytesWritten = 0;
	do
	{
		sizChunk = ulDataLen - ulBytesWritten;
		ssizBytesWritten = write(m_hPort, pbData + ulBytesWritten, sizChunk);
		if( ssizBytesWritten==-1 )
		{
			iErrno = errno;
			if( iErrno==0 )
			{
				wxLogMessage(wxT("Spurious write error detected, write returns -1, but errno is 0."));
				ssizBytesWritten = 0;
			}
			else if( iErrno==EAGAIN )
			{
				// flush device
				wxLogMessage(wxT("device is busy, flushing..."));
				if( tcdrain(m_hPort)==0 )
				{
					ssizBytesWritten = 0;
				}
				else
				{
					iErrno = errno;
					wxLogError(wxT("flush failed with errno: %d, strerror: %s"), iErrno, strerror(iErrno));
				}
			}
			else
			{
				wxLogError(wxT("romloader_uart_device_linux(%p): failed to write %d bytes at offset %d of %d total"), this, sizChunk, ulBytesWritten, ulDataLen);
				wxLogError(wxT("write failed with result: %d, errno: %d, strerror: %s"), ssizBytesWritten, iErrno, strerror(iErrno));
				break;
			}
		}
		else if( ssizBytesWritten<0 || ssizBytesWritten>sizChunk )
		{
			iErrno = errno;
			wxLogError(wxT("romloader_uart_device_linux(%p): failed to write %d bytes at offset %d of %d total, result: %d"), this, sizChunk, ulBytesWritten, ulDataLen, ssizBytesWritten);
			wxLogError(wxT("write failed with result: %d, errno: %d, strerror: %s"), ssizBytesWritten, iErrno, strerror(iErrno));
			break;
		}
		ulBytesWritten += ssizBytesWritten;
	} while( ulBytesWritten<ulDataLen );
	
	return ulBytesWritten;
}


unsigned long romloader_uart_device_linux::RecvRaw(unsigned char *pbData, unsigned long ulDataLen, unsigned long ulTimeout)
{
	unsigned long ulTimeLeft;
	size_t sizDataLeft;
	wxCondError tCondErr;
	size_t sizRead;
	struct timeval tStartTime;
	struct timeval tTimeout;
	struct timeval tEndTime;
	struct timeval tTime;
	struct timeval tTimeLeft;
	int iRes;


	sizDataLeft = ulDataLen;

	// get start time
	iRes = gettimeofday(&tStartTime, NULL);
	if( iRes!=0 )
	{
		wxLogError(wxT("gettimeofday failed with result %d, errno: %d (%s)"), iRes, errno, strerror(errno));
	}
	else
	{
		// set timeout structure
		tTimeout.tv_sec = (ulTimeout/1000);
		tTimeout.tv_usec = (ulTimeout%1000) * 1000;
		timeradd(&tStartTime, &tTimeout, &tEndTime);

		do
		{
			sizRead = readCards(pbData, sizDataLeft);
			sizDataLeft -= sizRead;
			if( sizDataLeft>0 )
			{
				// get new timeout value
				iRes = gettimeofday(&tTime, NULL);
				if( iRes!=0 )
				{
					wxLogError(wxT("gettimeofday failed with result %d, errno: %d (%s)"), iRes, errno, strerror(errno));
					break;
				}
				// get time difference
				timersub(&tEndTime, &tTime, &tTimeLeft);
				if( timerisset(&tTimeLeft)==0 )
				{
					break;
				}
				else
				{
					// convert diff to ms
					ulTimeout = tTimeLeft.tv_sec*1000 + tTimeLeft.tv_usec/1000;
				}

				// wait for data
				tCondErr = m_ptConditionRxDataAvail->WaitTimeout(ulTimeout);
				if( tCondErr!=wxCOND_NO_ERROR )
				{
					// timeout
					break;
				}
			}
		} while( sizDataLeft>0 );
	}

	return ulDataLen - sizDataLeft;
}


/*****************************************************************************/
/*! Check if any data is ready to grab
*    \return number of waiting bytes                                         */
/*****************************************************************************/
unsigned long romloader_uart_device_linux::Peek(void)
{
	return getCardSize();
}


/*****************************************************************************/
/*! Flushes any pending data
*    \return true on success                                                 */
/*****************************************************************************/
bool romloader_uart_device_linux::Flush(void)
{
	int iResult;


	// wait until all pending data is sent
	iResult = tcdrain(m_hPort);
	// convert the result to bool
	return (iResult==0);
}


/*****************************************************************************/
/*! Scan sysfs mount for tty style devices
*    \param cvList  Reference to vector to place found devices in
*    \return Successflag                                         */
/*****************************************************************************/
bool romloader_uart_device_linux::scanSysFs(wxArrayString *ptArray)
{
	bool fSuccess;
	wxDir dirClass;
	wxDir dirDevice;
	wxString strClassDir = wxT("/sys/class/tty");
	wxString strDescription;
	wxString strSubDir;
	wxString strProbePath;
	bool fFoundEntry;
	wxString strDevice;


	wxLogInfo(wxT("trying to get the list of available tty devices from the sysfs filesystem"));

	fSuccess = wxDir::Exists(strClassDir);
	if( fSuccess==false )
	{
		wxLogWarning(wxT("failed to open ") + strClassDir);
		wxLogWarning(wxT("is the kernel >= 2.6 ? is sysfs mounted?"));
	}
	else
	{
		// set the directory to the class 
		fSuccess = dirClass.Open(strClassDir);
		if( fSuccess==false )
		{
			wxLogWarning(wxT("failed to open ") + strClassDir);
		}
		else
		{
			// directory is open, loop over all entries
			fFoundEntry = dirClass.GetFirst(&strSubDir, wxEmptyString, wxDIR_DIRS);
			while( fFoundEntry )
			{
				// dive into the subdirectory and check for a device folder
				strProbePath = dirClass.GetName() + wxFileName::GetPathSeparator() + strSubDir + wxFileName::GetPathSeparator() + wxT("device");
				fFoundEntry = wxDir::Exists(strProbePath);
				if( fFoundEntry==true )
				{
					fFoundEntry = dirDevice.Open(strProbePath);
					if( fFoundEntry==true )
					{
						// construct the device name
						strDevice = wxT("/dev/") + strSubDir;
						ptArray->Add(strDevice);
					}
				}
				fFoundEntry = dirClass.GetNext(&strSubDir);
			}
		}
	}

	return fSuccess;
}


void romloader_uart_device_linux::ScanForPorts(wxArrayString *ptArray)
{
	bool fSuccess;
	wxString strName;
	int iCnt;


	// TODO: let user specify a regexp like '/dev/tty.*' and add all of them to the list

	// try to enumerate the com ports from the sysfs
	fSuccess = scanSysFs(ptArray);
	if( fSuccess==false )
	{
		// fallback to the default ports on ttyS0 - ttyS3
		for(iCnt=0; iCnt<4; ++iCnt)
		{
			// construct the name
			strName.Printf("/dev/ttyS%d", iCnt);
			ptArray->Add(strName);
		}
	}
}

/*****************************************************************************/
/*! Cancels any pending send/receive functions
*    \return true on successful cancel request                               */
/*****************************************************************************/
bool romloader_uart_device_linux::Cancel()
{
  //TODO: How to cancel a pending transfer
  return true;
}







rxThread::rxThread(int hPort, romloader_uart_device_linux *ptParent)
 : wxThread(wxTHREAD_JOINABLE)
 , m_hPort(hPort)
 , m_ptParent(ptParent)
{
	
}

wxThread::ExitCode rxThread::Entry(void)
{
	bool fDestroy;
	fd_set tRead;
	struct timeval tTimeout;
	size_t sizRead;
	size_t sizWrite;
	int iSelect;
	int iExitCode;
	const size_t sizBufSize = 1024;
	unsigned char aucBuf[sizBufSize];


	fDestroy = false;
	iExitCode = EXIT_SUCCESS;

	// loop until all requested bytes are received or the timeout strikes
	do
	{
		// clear the descriptor list
		FD_ZERO(&tRead);
		// add the tty handle to the descriptor list
		FD_SET(m_hPort, &tRead);

		tTimeout.tv_sec = 0;
		tTimeout.tv_usec = 100000;

		// wait for data on the tty
		iSelect = select(m_hPort + 1, &tRead, NULL, NULL, &tTimeout);
		if( iSelect==-1 )
		{
			// failed to get data -> maybe tty was destroyed (e.g. usb plugged out)
			fDestroy = true;
		}
		else if( iSelect==1 )
		{
			// receive some bytes
			sizRead = read(m_hPort, aucBuf, sizBufSize);
			// NOTE: accept 0 bytes here, it might happen that select triggers but no data present
			if( sizRead<0 )
			{
				fDestroy = true;
			}
			else
			{
				// put the received data into the cards
				m_ptParent->writeCards(aucBuf, sizRead);

				m_ptParent->m_ptConditionRxDataAvail->Signal();
			}
		}
		else
		{
			fDestroy = TestDestroy();
		}
	} while( fDestroy==false );

	return (wxThread::ExitCode)iExitCode;
}

