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

#include <wx/wx.h>

#include <termios.h>


#ifndef __ROMLOADER_UART_DEVICE_LINUX_H__
#define __ROMLOADER_UART_DEVICE_LINUX_H__

#include "romloader_uart_device.h"


class romloader_uart_device_linux : public romloader_uart_device
{
public:
	romloader_uart_device_linux(wxString strPortName);
	virtual ~romloader_uart_device_linux();

	virtual bool		Open(void);
	virtual void		Close(void);

	unsigned long		SendRaw(const unsigned char *pbData, unsigned long ulDataLen, unsigned long ulTimeout);
	bool			Cancel(void);
	unsigned long		RecvRaw(unsigned char *pbData, unsigned long ulDataLen, unsigned long ulTimeout);
	bool			GetLine(wxString &strData, const char *pcEol, unsigned long ulTimeout);
	bool			Flush(void);
	unsigned long		Peek(void);

	static bool		scanSysFs(wxArrayString *ptArray);
	static void		ScanForPorts(wxArrayString *ptArray);

protected:
	unsigned long GetMaxBlockSize(void) { return 64; }

	int m_hPort;
	struct termios m_tOldAttribs;

private:
	PFN_PROGRESS_CALLBACK m_pfnProgressCallback;
	void *m_pvCallbackUserData;
};

#endif	/* __ROMLOADER_UART_DEVICE_LINUX_H__ */
