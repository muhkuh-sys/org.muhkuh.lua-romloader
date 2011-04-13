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

#include <pthread.h>
#include <termios.h>
#include <unistd.h>


#ifndef __ROMLOADER_UART_DEVICE_LINUX_H__
#define __ROMLOADER_UART_DEVICE_LINUX_H__

#include "romloader_uart_device.h"


class romloader_uart_device_linux;

typedef struct
{
	int hPort;
	romloader_uart_device_linux *ptParent;
} RXTHREAD_PDATA_T;


class romloader_uart_device_linux : public romloader_uart_device
{
public:
	romloader_uart_device_linux(const char *pcPortName);
	virtual ~romloader_uart_device_linux();

	virtual bool Open(void);
	virtual void Close(void);

	size_t SendRaw(const unsigned char *pucData, size_t sizData, unsigned long ulTimeout);
	bool Cancel(void);
	size_t RecvRaw(unsigned char *pucData, size_t sizData, unsigned long ulTimeout);
	bool Flush(void);
	unsigned long Peek(void);

	static size_t scanSysFs(char ***pppcPortNames);
	static size_t ScanForPorts(char ***pppcPortNames);

protected:
	unsigned long GetMaxBlockSize(void) { return 4096; }

	int m_hPort;
	struct termios m_tOldAttribs;

private:
	void *m_pvCallbackUserData;

	/* the rx thread and its data structure */
	bool m_fRxThreadIsRunning;
	pthread_t m_tRxThread;
	RXTHREAD_PDATA_T m_tRxPData;
};


#endif  /* __ROMLOADER_UART_DEVICE_LINUX_H__ */
