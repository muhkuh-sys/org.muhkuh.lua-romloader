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

#ifndef __ROMLOADER_UART_DEVICE_WIN_H__
#define __ROMLOADER_UART_DEVICE_WIN_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "romloader_uart_device.h"


class romloader_uart_device_win : public romloader_uart_device  
{
public:
	romloader_uart_device_win(const char *pcPortName);
	virtual ~romloader_uart_device_win();

	static DWORD WINAPI CheckComStateThread(void* pvParam);

	virtual bool Open(void);
	virtual void Close(void);

	size_t SendRaw(const void *pvData, size_t sizData, unsigned long ulTimeout);
	bool Cancel(void);
	size_t RecvRaw(unsigned char *pucData, size_t sizData, unsigned long ulTimeout);
	bool Flush(void);
	unsigned long Peek(void);

	static unsigned long  ScanForPorts(char ***pppcDeviceNames);


protected:
	unsigned long GetMaxBlockSize(void) { return 4096; }

	void              CheckComEvents(DWORD dwEventMask);
	DWORD             CheckComState();

	bool              m_fRunning;       /**< true if the receiver thread should run. false to end it*/
	HANDLE            m_hPort;          /**< Handle to the serial port */
	HANDLE            m_hComStateThread;/**< Handle to the receiver thread */

	HANDLE            m_hTxEmpty;       /**< Handle to the TX Empty event being signalled, when all data has been written */
	HANDLE            m_hNewRxEvent;    /**< Handle to the new RX Data available event */

private:
	typedef enum RECEIVESTATUS_ENUM
	{
		RECEIVESTATUS_Idle = 0,
		RECEIVESTATUS_WaitForCommEventOverlap = 1,
		RECEIVESTATUS_ProcessCommEvents = 2,
		RECEIVESTATUS_WaitForDataOverlap = 3,
		RECEIVESTATUS_ProcessData = 4,
		RECEIVESTATUS_Error = 5
	} RECEIVESTATUS_T;

	static const DWORD m_PollingTimeoutForCommEventsInMs = 500;
	static const DWORD m_PollingTimeoutForReadInMs = 500;


	void print_error(void);
	void *m_pvCallbackUserData;
	RECEIVESTATUS_T m_tReceiveStatus;
	HANDLE m_hEventSendData;
};

#endif  /* __ROMLOADER_UART_DEVICE_WIN_H__ */
