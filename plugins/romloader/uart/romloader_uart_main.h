/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen                                *
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

#include <vector>

#ifndef __ROMLOADER_UART_MAIN_H__
#define __ROMLOADER_UART_MAIN_H__

#include "../romloader.h"

#ifdef __LINUX__
	#include "romloader_uart_device_linux.h"
	typedef romloader_uart_device_linux romloader_uart_device_platform;
#else
	#ifdef __WINDOWS__
		#include "romloader_uart_device_win.h"
		typedef romloader_uart_device_win romloader_uart_device_platform;
	#else
		#error "Unknown platform, please add uart handler!"
	#endif
#endif

/*-----------------------------------*/

extern "C"
{
	WXEXPORT int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId);
	WXEXPORT int fn_init_lua(wxLuaState *ptLuaState);
	WXEXPORT int fn_leave(void);
	WXEXPORT const muhkuh_plugin_desc *fn_get_desc(void);
	WXEXPORT int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList);

	WXEXPORT romloader *romloader_uart_create(void *pvHandle);
}


typedef void (*romloader_uart_plugin_fn_close_instance)(wxString &strInterface);


class romloader_uart : public romloader
{
public:
	romloader_uart(wxString strName, wxString strTyp, const romloader_functioninterface *ptFn, void *pvHandle, romloader_uart_plugin_fn_close_instance fn_close, wxLuaState *ptLuaState);
	~romloader_uart(void);

// *** lua interface start ***
	// open the connection to the device
	virtual void connect(void);
	// close the connection to the device
	virtual void disconnect(void);
	// returns the connection state of the device
	virtual bool is_connected(void) const;

	// read a byte (8bit) from the netx to the pc
	virtual double read_data08(double dNetxAddress);
	// read a word (16bit) from the netx to the pc
	virtual double read_data16(double dNetxAddress);
	// read a long (32bit) from the netx to the pc
	virtual double read_data32(double dNetxAddress);
	// read a byte array from the netx to the pc
	virtual wxString read_image(double dNetxAddress, double dSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// write a byte (8bit) from the pc to the netx
	virtual void write_data08(double dNetxAddress, double dData);
	// write a word (16bit) from the pc to the netx
	virtual void write_data16(double dNetxAddress, double dData);
	// write a long (32bit) from the pc to the netx
	virtual void write_data32(double dNetxAddress, double dData);
	// write a byte array from the pc to the netx
	virtual void write_image(double dNetxAddress, wxString strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// call routine
	virtual void call(double dNetxAddress, double dParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
// *** lua interface end ***

private:
	bool chip_init(void);
	int write_data(wxString &strData, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData, wxString &strErrorMsg);
	bool parseDumpLine(const char *pcLine, size_t sizLineLen, unsigned long ulAddress, unsigned long ulElements, unsigned char *pucBuffer, wxString &strErrorMsg);
  unsigned int buildCrc(const char *pcData, size_t sizDataLen);

	bool m_fIsConnected;

	romloader_uart_device *m_ptUartDev;

	romloader_uart_plugin_fn_close_instance m_fn_uart_close;

	wxString m_strInterface;
	wxString m_strTyp;

	// formatted name for log message
	wxString m_strMe;
};


/*-----------------------------------*/

#endif	/* __ROMLOADER_UART_MAIN_H__ */

