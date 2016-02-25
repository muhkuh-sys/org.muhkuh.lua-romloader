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


#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>


#include "romloader_uart_device_linux.h"


void *romloader_uart_rx_thread(void *pvParameter)
{
	RXTHREAD_PDATA_T *ptPData;
	int iOldState;
	bool fDestroy;
	fd_set tRead;
	struct timeval tTimeout;
	size_t sizRead;
	size_t sizWrite;
	int iResult;
	ptrdiff_t iExitCode;
	const size_t sizBufSize = 1024;
	unsigned char aucBuf[sizBufSize];


	/* Get the parameter. */
	ptPData = (RXTHREAD_PDATA_T*)pvParameter;
//	printf("romloader_uart_rx_thread: hPort=%d\n", ptPData->hPort);
//	printf("romloader_uart_rx_thread: ptParent=%p\n", ptPData->ptParent);

	/* Allow to cancel this thread. */
	iResult = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &iOldState);
	if( iResult!=0 )
	{
		fprintf(stderr, "romloader_uart_rx_thread: pthread_setcancelstate failed with error %d\n", iResult);
	}
	else
	{
		fDestroy = false;
		iExitCode = EXIT_SUCCESS;

		/* Loop until the parent thread asks to quit. */
		do
		{
			/* Clear the descriptor list. */
			FD_ZERO(&tRead);
			/* Add the tty handle to the descriptor list. */
			FD_SET(ptPData->hPort, &tRead);

			/* Set the timeout to 0.1 seconds. */
			tTimeout.tv_sec = 0;
			tTimeout.tv_usec = 100000;

			/* Wait for data on the tty. */
			iResult = select(ptPData->hPort + 1, &tRead, NULL, NULL, &tTimeout);
			if( iResult==-1 )
			{
				/* Failed to get data -> maybe tty was destroyed (e.g. usb plugged out). */
				fDestroy = true;
			}
			else if( iResult==1 )
			{
				/* Receive some bytes. */
				sizRead = read(ptPData->hPort, aucBuf, sizBufSize);
				/* NOTE: accept 0 bytes here, it might happen that select triggers but no data is present. */
				if( sizRead<0 )
				{
					fDestroy = true;
				}
				else
				{
					/* Put the received data into the cards. */
					ptPData->ptParent->writeCards(aucBuf, sizRead);
				}
			}

			pthread_testcancel();
		} while( fDestroy==false );
	}

	pthread_exit((void*)iExitCode);
}


romloader_uart_device_linux::romloader_uart_device_linux(const char *pcPortName)
  : romloader_uart_device(pcPortName)
  , m_hPort(-1)
  , m_fRxThreadIsRunning(false)
{
	int iResult;


	iResult = pthread_cond_init(&m_tRxDataAvail_Condition, NULL);
	if( iResult!=0 )
	{
		fprintf(stderr, "romloader_uart_device_linux(%p),'%s': failed to init the condition: %d\n", this, m_pcPortName, iResult);
	}
	else
	{
		iResult = pthread_mutex_init(&m_tRxDataAvail_Mutex, NULL);
		if( iResult!=0 )
		{
			fprintf(stderr, "romloader_uart_device_linux(%p),'%s': failed to init the mutex: %d\n", this, m_pcPortName, iResult);
		}
//		else
//		{
//			printf("romloader_uart_device_linux(%p),'%s': created\n", this, m_pcPortName);
//		}
	}
}


romloader_uart_device_linux::~romloader_uart_device_linux(void)
{
	int iResult;


	Close();
	
	iResult = pthread_mutex_destroy(&m_tRxDataAvail_Mutex);
	if( iResult!=0 )
	{
		fprintf(stderr, "romloader_uart_device_linux(%p),'%s': failed to destroy the mutex: %d\n", this, m_pcPortName, iResult);
	}

	iResult = pthread_cond_destroy(&m_tRxDataAvail_Condition);
	if( iResult!=0 )
	{
		fprintf(stderr, "romloader_uart_device_linux(%p),'%s': failed to destroy the condition: %d\n", this, m_pcPortName, iResult);
	}
}


bool romloader_uart_device_linux::Open(void)
{
	bool fResult;
	int iResult;
	struct termios tNewAttribs = {0};
	pid_t pidRx;
	int iRxExitCode;
	char acDeviceFile[PATH_MAX];


	/* Expect failure. */
	fResult = false;

	/* Close an open connection. */
	Close();

	/* Create the cards. */
	initCards();

	/* Construct the device path. */
	snprintf(acDeviceFile, PATH_MAX, "/dev/%s", m_pcPortName);

	/* Try to open the connection. */
	m_hPort = open(acDeviceFile, O_RDWR | O_NOCTTY | O_NONBLOCK);
	/* Did the open operation succeed? */
	if( m_hPort==-1 )
	{
		/* no -> exit with error */
		fprintf(stderr, "failed to open the serial port %s: %s", acDeviceFile, strerror(errno));
	}
	else
	{
		/* Remember the current port settings. Use this to restore the settings when the port is closed. */
		tcgetattr(m_hPort, &m_tOldAttribs);

		/* Get current port settings and apply all settings on this base. */
		tcgetattr(m_hPort, &tNewAttribs);

		/* Set new port settings for non-blocking input processing. */
		cfmakeraw(&tNewAttribs);
		tNewAttribs.c_cflag |= CREAD | CLOCAL;
		tNewAttribs.c_cflag &= ~CRTSCTS;
		/* Set 1 stop bit. */
		tNewAttribs.c_cflag &= ~CSTOPB;

		iResult = cfsetispeed(&tNewAttribs, B115200);
		if( iResult!=0 )
		{
			fprintf(stderr, "Failed to set input speed of '%s' to 115200: %d\n", acDeviceFile, iResult);
		}
		else
		{
			iResult = cfsetospeed(&tNewAttribs, B115200);
			if( iResult!=0 )
			{
				fprintf(stderr, "Failed to set output speed of '%s' to 115200: %d\n", acDeviceFile, iResult);
			}
			else
			{
				iResult = tcflush(m_hPort, TCIOFLUSH);
				if( iResult!=0 )
				{
					fprintf(stderr, "Failed to apply new parameters to '%s': %d\n", acDeviceFile, iResult);
				}
				else
				{
					iResult = tcsetattr(m_hPort, TCSANOW, &tNewAttribs);
					if( iResult!=0 )
					{
						fprintf(stderr, "Failed to apply new parameters to '%s': %d\n", acDeviceFile, iResult);
					}
					else
					{
						/*
						 * Create a new receive thread.
						 */
						m_tRxPData.hPort = m_hPort;
						m_tRxPData.ptParent = this;
						iResult = pthread_create(&m_tRxThread, NULL, romloader_uart_rx_thread, (void*)(&m_tRxPData));
						if( iResult==0 )
						{
							m_fRxThreadIsRunning = true;
							fResult = true;
						}
						else
						{
							fprintf(stderr, "Failed to create receive thread: %d\n", iResult);
						}
					}
				}
			}
		}
	}

	return fResult;
}



void romloader_uart_device_linux::Close(void)
{
	pid_t pidTerm;
	void *pvStatus;


	if( m_hPort!=-1 )
	{
		tcsetattr(m_hPort, TCSANOW, &m_tOldAttribs);
		close(m_hPort);

		m_hPort = -1;
	}

	/* Stop the receive thread. */
	fprintf(stderr, "Close: m_fRxThreadIsRunning=%d\n", m_fRxThreadIsRunning);
	if( m_fRxThreadIsRunning==true )
	{
//		fprintf(stderr, "pthread_cancel\n");
		pthread_cancel(m_tRxThread);
//		fprintf(stderr, "pthread_join\n");
		pthread_join(m_tRxThread, &pvStatus);
//		printf("rxthread returned status %p.\n", pvStatus);

		m_fRxThreadIsRunning = false;
	}

	/* Delete the cards. */
	deleteCards();
}


size_t romloader_uart_device_linux::SendRaw(const unsigned char *pucData, size_t sizData, unsigned long ulTimeout)
{
	ssize_t ssizBytesWritten;
	size_t sizChunk;
	size_t sizBytesWritten;
	int iErrno;


	sizBytesWritten = 0;
	do
	{
		sizChunk = sizData - sizBytesWritten;
		ssizBytesWritten = write(m_hPort, pucData + sizBytesWritten, sizChunk);
		if( ssizBytesWritten==-1 )
		{
			iErrno = errno;
			if( iErrno==0 )
			{
				fprintf(stderr, "Spurious write error detected, write returns -1, but errno is 0.");
				ssizBytesWritten = 0;
			}
			else if( iErrno==EAGAIN )
			{
				// flush device
				fprintf(stderr, "device is busy, flushing...");
				if( tcdrain(m_hPort)==0 )
				{
					ssizBytesWritten = 0;
				}
				else
				{
					iErrno = errno;
					fprintf(stderr, "flush failed with errno: %d, strerror: %s", iErrno, strerror(iErrno));
				}
			}
			else
			{
				fprintf(stderr, "romloader_uart_device_linux(%p): failed to write %ld bytes at offset %ld of %ld total", this, sizChunk, sizBytesWritten, sizData);
				fprintf(stderr, "write failed with result: %ld, errno: %d, strerror: %s", ssizBytesWritten, iErrno, strerror(iErrno));
				break;
			}
		}
		else if( ssizBytesWritten<0 || ssizBytesWritten>sizChunk )
		{
			iErrno = errno;
			fprintf(stderr, "romloader_uart_device_linux(%p): failed to write %ld bytes at offset %ld of %ld total, result: %ld", this, sizChunk, sizBytesWritten, sizData, ssizBytesWritten);
			fprintf(stderr, "write failed with result: %ld, errno: %d, strerror: %s", ssizBytesWritten, iErrno, strerror(iErrno));
			break;
		}
		sizBytesWritten += ssizBytesWritten;
	} while( sizBytesWritten<sizData );
	
	return sizBytesWritten;
}


size_t romloader_uart_device_linux::RecvRaw(unsigned char *pucData, size_t sizData, unsigned long ulTimeout)
{
	unsigned long ulTimeLeft;
	size_t sizDataLeft;
	size_t sizRead;
	struct timespec tEndTime;
	int iResult;
	int iWaitResult;


	sizDataLeft = sizData;

	/* Get absolute end time. */
	iResult = clock_gettime(CLOCK_REALTIME, &tEndTime);
	if( iResult!=0 )
	{
		fprintf(stderr, "clock_gettime failed with result %d, errno: %d (%s)", iResult, errno, strerror(errno));
	}
	else
	{
		/* Add the timeout to the current time. */
		tEndTime.tv_sec += (ulTimeout/1000UL);
		tEndTime.tv_nsec += (ulTimeout%1000UL) * 1000000UL;
		while( tEndTime.tv_nsec>1000000000UL )
		{
			tEndTime.tv_nsec -= 1000000000UL;
			++tEndTime.tv_sec;
		}

		do
		{
			sizRead = readCards(pucData, sizDataLeft);
			sizDataLeft -= sizRead;
			pucData += sizRead;
			if( sizDataLeft>0 )
			{
				/* Wait for data. */
				iResult = pthread_mutex_lock(&m_tRxDataAvail_Mutex);
				iWaitResult = pthread_cond_timedwait(&m_tRxDataAvail_Condition, &m_tRxDataAvail_Mutex, &tEndTime);
				iResult = pthread_mutex_unlock(&m_tRxDataAvail_Mutex);

				if( iWaitResult==ETIMEDOUT )
				{
					/* Timeout */
					break;
				}
			}
		} while( sizDataLeft>0 );
	}

	return sizData - sizDataLeft;
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
/*! Check if a device is a real serial port.
*    \param pcDevicePath  Pointer to the 0 terminated path of the device.
*    \return Successflag                                         */
/*****************************************************************************/
int romloader_uart_device_linux::isDeviceRealSerialPort(const char *pcDevicePath)
{
	int iResult;
	int iIoctlResult;
	int iPortFd;
	int iModemBitsStatus;


	/* Be pessimistic. */
	iResult = -1;

	/* Try to open the device. */
	iPortFd = open(pcDevicePath, O_RDONLY|O_NONBLOCK|O_NOCTTY);
	if( iPortFd!=-1 )
	{
		/* Try to get the modem bits. */
		iIoctlResult = ioctl(iPortFd, TIOCMGET, &iModemBitsStatus);
		if( iIoctlResult==0 )
		{
			/* The device supprts modem bits, so there is a good chance that this is a real serial port. */
			iResult = 0;
		}

		/* Close the device. */
		close(iPortFd);
	}

	return iResult;
}



/*****************************************************************************/
/*! Scan sysfs mount for tty style devices
*    \param cvList  Reference to vector to place found devices in
*    \return Successflag                                         */
/*****************************************************************************/
size_t romloader_uart_device_linux::scanSysFs(char ***pppcPortNames)
{
	int iResult;
	int iNonFatalResult;
	const char *pcClassDir = "/sys/class/tty";
	const char *pcPluginName = "romloader_uart_%s";
	struct stat tStatBuf;
	DIR *ptDir;
	struct dirent tDirEntry;
	struct dirent *ptDirEntry;
	char strDevicePath[PATH_MAX];

	size_t sizRefCnt;
	size_t sizRefMax;
	char **ppcRef;
	char **ppcRefNew;
	size_t sizEntry;
	char *pcRef;


	/* Init References array. */
	sizRefCnt = 0;
	sizRefMax = 16;
	ppcRef = (char**)malloc(sizRefMax*sizeof(char*));
	if( ppcRef==NULL )
	{
		fprintf(stderr, "out of memory!\n");
		iResult = -1;
	}
	else
	{
		fprintf(stderr, "romloader_uart_device_linux: trying to get the list of available tty devices from the sysfs folder %s\n", pcClassDir);

		/* Does the sys folder exist? */
		iResult = stat(pcClassDir, &tStatBuf);
		if( iResult!=0 )
		{
			fprintf(stderr, "romloader_uart_device_linux: failed to stat '%s': (%d) %s\n", pcClassDir, errno, strerror(errno));
			fprintf(stderr, "romloader_uart_device_linux: is the kernel >= 2.6 ? is sysfs mounted?\n");
		}
		else
		{
			/* Iterate over the directory entries. */
			ptDir = opendir(pcClassDir);
			if( ptDir==NULL )
			{
				fprintf(stderr, "romloader_uart_device_linux: failed to open '%s': (%d) %s\n", pcClassDir, errno, strerror(errno));
			}
			else
			{
				/* Directory is open, loop over all entries. */
				do
				{
					iResult = readdir_r(ptDir, &tDirEntry, &ptDirEntry);
					if( iResult==0 )
					{
						/* Reached the end of the directory? */
						if( ptDirEntry==NULL )
						{
							/* Yes -> stop searching for devices. */
							break;
						}
						/* Is the entry a folder or a link? */
						else if( tDirEntry.d_type==DT_DIR || tDirEntry.d_type==DT_LNK || tDirEntry.d_type==DT_UNKNOWN )
						{
							/* Is this not the '.' or '..' entry? */
							if( strcmp(".", tDirEntry.d_name)!=0 && strcmp("..", tDirEntry.d_name)!=0 )
							{
								/* Construct the full path to the device. */
								snprintf(strDevicePath, PATH_MAX-1, "/dev/%s", tDirEntry.d_name);
								/* Is the device a real serial port? */
								iNonFatalResult = isDeviceRealSerialPort(strDevicePath);
								if( iNonFatalResult==0 )
								{
									/* Yes -> this is a real serial port. */

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
										ppcRefNew = (char**)realloc(ppcRef, sizRefMax*sizeof(char*));
										if( ppcRefNew==NULL )
										{
											iResult = -1;
											break;
										}
										ppcRef = ppcRefNew;
									}
									sizEntry = strlen(pcPluginName)-2 + strlen(tDirEntry.d_name) + 1;
									pcRef = (char*)malloc(sizEntry);
									if( pcRef==NULL )
									{
										iResult = -1;
										break;
									}
									snprintf(pcRef, sizEntry, pcPluginName, tDirEntry.d_name);
									ppcRef[sizRefCnt++] = pcRef;
								}
							}
						}
					}
				} while( iResult==0 );
			}
		}

		if( iResult!=0 )
		{
			/* Free all strings in the list. */
			while( sizRefCnt!=0 )
			{
				--sizRefCnt;
				free(ppcRef[sizRefCnt]);
			}
			/* Free the list itself. */
			free(ppcRef);
		}
	}

	if( iResult!=0 )
	{
		sizRefCnt = -1;
	}

	*pppcPortNames = ppcRef;
	return sizRefCnt;
}


size_t romloader_uart_device_linux::ScanForPorts(char ***pppcDeviceNames)
{
	char **ppcDeviceNames;
	char *pcDeviceName;
	size_t sizCnt;
	size_t sizDevices;
	static const char *pcFallbackName = "romloader_uart_/dev/ttyS%d";


	sizDevices = 0;
	ppcDeviceNames = NULL;

	/* Try to enumerate the com ports from the sysfs. */
	sizDevices = scanSysFs(&ppcDeviceNames);
	if( sizDevices==-1 )
	{
		sizDevices = 4;
		ppcDeviceNames = (char**)malloc(sizDevices*sizeof(char*));
		if( ppcDeviceNames==NULL )
		{
			sizDevices = 0;
		}
		else
		{
			/* Fallback to the default ports on ttyS0 - ttyS3. */
			for(sizCnt=0; sizCnt<sizDevices; ++sizCnt)
			{
				/* Construct the name. */
				pcDeviceName = (char*)malloc(strlen(pcFallbackName)+1);
				if( pcDeviceName==NULL )
				{
					while( sizCnt>0 )
					{
						--sizCnt;
						free(pcDeviceName);
					}
					free(ppcDeviceNames);
					ppcDeviceNames = NULL;
					sizDevices = 0;
					break;
				}
				sprintf(pcDeviceName, pcFallbackName, sizCnt);
				ppcDeviceNames[sizCnt] = pcDeviceName;
			}
		}
	}

	*pppcDeviceNames = ppcDeviceNames;

	return sizDevices;
}

/*****************************************************************************/
/*! Cancels any pending send/receive functions
*    \return true on successful cancel request                               */
/*****************************************************************************/
bool romloader_uart_device_linux::Cancel()
{
	/* TODO: How to cancel a pending transfer? */
	return true;
}

