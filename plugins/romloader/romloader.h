/***************************************************************************
 *   Copyright (C) 2007 by Christoph Thelen                                *
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


#include <vector>

#include <wx/wx.h>
#include <wx/dynlib.h>

#include <wxlua/include/wxlua.h>
#include <wxluasocket/include/wxldserv.h>
extern "C"
{
	#include <lualib.h>
}

#include "../muhkuh_plugin_interface.h"


#ifndef __ROMLOADER_H__
#define __ROMLOADER_H__

/*-----------------------------------*/

/* open the connection to the device */
typedef bool (*romloader_fn_connect)(void *pvHandle);
/* close the connection to the device */
typedef void (*romloader_fn_disconnect)(void *pvHandle);
/* returns the connection state of the device */
typedef bool (*romloader_fn_is_connected)(void *pvHandle);

/* read a byte (8bit) from the netx to the pc */
typedef int (*romloader_fn_read_data08)(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pbData);
/* read a word (16bit) from the netx to the pc */
typedef int (*romloader_fn_read_data16)(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData);
/* read a long (32bit) from the netx to the pc */
typedef int (*romloader_fn_read_data32)(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData);
/* read a byte array from the netx to the pc */
typedef int (*romloader_fn_read_image)(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pbData, unsigned long ulSize);

/* write a byte (8bit) from the pc to the netx */
typedef int (*romloader_fn_write_data08)(void *pvHandle, unsigned long ulNetxAddress, unsigned char bData);
/* write a word (16bit) from the pc to the netx */
typedef int (*romloader_fn_write_data16)(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData);
/* write a long (32bit) from the pc to the netx */
typedef int (*romloader_fn_write_data32)(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData);
/* write a byte array from the pc to the netx */
typedef int (*romloader_fn_write_image)(void *pvHandle, unsigned long ulNetxAddress, const unsigned char *pbData, unsigned long ulSize);

/* call routine */
typedef int (*romloader_fn_call)(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0);

/*-----------------------------------*/

typedef struct
{
	romloader_fn_connect		fn_connect;
	romloader_fn_disconnect		fn_disconnect;
	romloader_fn_is_connected	fn_is_connected;
	romloader_fn_read_data08	fn_read_data08;
	romloader_fn_read_data16	fn_read_data16;
	romloader_fn_read_data32	fn_read_data32;
	romloader_fn_read_image		fn_read_image;
	romloader_fn_write_data08	fn_write_data08;
	romloader_fn_write_data16	fn_write_data16;
	romloader_fn_write_data32	fn_write_data32;
	romloader_fn_write_image	fn_write_image;
	romloader_fn_call		fn_call;
} romloader_functioninterface;

/*-----------------------------------*/

// NOTE: romloader is derived from wxObject to avoid the %encapsulate wrappers for garbage collection in lua
class romloader : public wxObject
{
public:
	romloader(wxString strName, wxString strTyp, const romloader_functioninterface *ptFn, void *pvHandle, muhkuh_plugin_fn_close_instance fn_close, wxLuaState *ptLuaState);
	~romloader(void);

// *** lua interface start ***
	// open the connection to the device
	void connect(void);
	// close the connection to the device
	void disconnect(void);
	// returns the connection state of the device
	bool is_connected(void) const;

	// returns the device name
	wxString get_name(void);
	// returns the device typ
	wxString get_typ(void);

	// read a byte (8bit) from the netx to the pc
	unsigned char read_data08(unsigned long ulNetxAddress);
	// read a word (16bit) from the netx to the pc
	unsigned short read_data16(unsigned long ulNetxAddress);
	// read a long (32bit) from the netx to the pc
	unsigned long read_data32(unsigned long ulNetxAddress);
	// read a byte array from the netx to the pc
	// unsigned char *read_image(unsigned long ulNetxAddress, unsigned long ulSize);

	// write a byte (8bit) from the pc to the netx
	void write_data08(unsigned long ulNetxAddress, unsigned char bData);
	// write a word (16bit) from the pc to the netx
	void write_data16(unsigned long ulNetxAddress, unsigned short usData);
	// write a long (32bit) from the pc to the netx
	void write_data32(unsigned long ulNetxAddress, unsigned long ulData);
	// write a byte array from the pc to the netx
	void write_image(unsigned long ulNetxAddress, const unsigned char *pbData, unsigned long ulSize);

	// call routine
	void call(unsigned long ulNetxAddress, unsigned long ulParameterR0);
// *** lua interface end ***

private:
	wxString m_strName;
	wxString m_strTyp;
	romloader_functioninterface m_tFunctionInterface;
	void *m_pvHandle;

	muhkuh_plugin_fn_close_instance m_fn_close;
	// the lua state
	wxLuaState *m_ptLuaState;
};

/*-----------------------------------*/

#endif	/* __ROMLOADER_H__ */

