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


#include "romloader.h"


romloader::romloader(wxString strName, wxString strTyp, const romloader_functioninterface *ptFn, void *pvHandle, muhkuh_plugin_fn_close_instance fn_close, wxLuaState *ptLuaState)
 : wxObject()
 , m_strName(strName)
 , m_strTyp(strTyp)
 , m_pvHandle(pvHandle)
 , m_fn_close(fn_close)
 , m_ptLuaState(ptLuaState)
{
	wxString strDebug;


	memcpy(&m_tFunctionInterface, ptFn, sizeof(romloader_functioninterface));

	// *** DEBUG ***
	strDebug.Printf(wxT("created romloader at %p"), this);
	wxLogMessage(strDebug);
}


romloader::~romloader(void)
{
	wxString strDebug;


	if( m_fn_close!=NULL )
	{
		m_fn_close(m_pvHandle);
	}

	// *** DEBUG ***
	strDebug.Printf(wxT("deleted romloader at %p"), this);
	wxLogMessage(strDebug);
}


// open the connection to the device
void romloader::connect(void)
{
	m_tFunctionInterface.fn_connect(m_pvHandle);
}


// close the connection to the device
void romloader::disconnect(void)
{
	m_tFunctionInterface.fn_disconnect(m_pvHandle);
}


// returns the connection state of the device
bool romloader::is_connected(void) const
{
	return m_tFunctionInterface.fn_is_connected(m_pvHandle);
}


// returns the device name
wxString romloader::get_name(void)
{
	return m_strName;
}


// returns the device typ
wxString romloader::get_typ(void)
{
	return m_strTyp;
}


// read a byte (8bit) from the netx to the pc
unsigned char romloader::read_data08(unsigned long ulNetxAddress)
{
	wxString strMsg;
	unsigned char ucData = 0x00;
	int iResult;


	iResult = m_tFunctionInterface.fn_read_data08(m_pvHandle, ulNetxAddress, &ucData);
	if( iResult!=0 )
	{
		strMsg.Printf(wxT("read_data08 failed with error %d"), iResult);
		m_ptLuaState->terror(strMsg);
	}

	return ucData;
}


// read a word (16bit) from the netx to the pc
unsigned short romloader::read_data16(unsigned long ulNetxAddress)
{
	wxString strMsg;
	unsigned short usData = 0x0000;
	int iResult;


	iResult = m_tFunctionInterface.fn_read_data16(m_pvHandle, ulNetxAddress, &usData);
	if( iResult!=0 )
	{
		strMsg.Printf(wxT("read_data16 failed with error %d"), iResult);
		m_ptLuaState->terror(strMsg);
	}

	return usData;
}


// read a long (32bit) from the netx to the pc
unsigned long romloader::read_data32(unsigned long ulNetxAddress)
{
	wxString strMsg;
	unsigned long ulData = 0x00000000;
	int iResult;


	iResult = m_tFunctionInterface.fn_read_data32(m_pvHandle, ulNetxAddress, &ulData);
	if( iResult!=0 )
	{
		strMsg.Printf(wxT("read_data32 failed with error %d"), iResult);
		m_ptLuaState->terror(strMsg);
	}

	return ulData;
}


// read a byte array from the netx to the pc
wxString romloader::read_image(unsigned long ulNetxAddress, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	wxString strMsg;
	wxString strData;
	char *pcBuffer;
	int iResult;


	// check parameter
	if( ulSize!=0 )
	{
		// get memory for the buffer
		pcBuffer = new char[ulSize];
		iResult = m_tFunctionInterface.fn_read_image(m_pvHandle, ulNetxAddress, pcBuffer, ulSize, L, iLuaCallbackTag, pvCallbackUserData);
		if( iResult!=0 )
		{
			strMsg.Printf(wxT("read_image failed with error %d"), iResult);
			// free the memory as terror does not return
			delete[] pcBuffer;
			m_ptLuaState->terror(strMsg);
		}
		else
		{
			// convert the binary data to wxString
			strData = wxString::From8BitData(pcBuffer, ulSize);
			delete[] pcBuffer;
		}
	}
	else
	{
		wxLogDebug(wxT("ignoring read_image request with size 0"));
	}

	return strData;
}

// write a byte (8bit) from the pc to the netx
void romloader::write_data08(unsigned long ulNetxAddress, unsigned char ucData)
{
	wxString strMsg;
	int iResult;


	iResult = m_tFunctionInterface.fn_write_data08(m_pvHandle, ulNetxAddress, ucData);
	if( iResult!=0 )
	{
		strMsg.Printf(wxT("write_data08 failed with error %d"), iResult);
		m_ptLuaState->terror(strMsg);
	}
}


// write a word (16bit) from the pc to the netx
void romloader::write_data16(unsigned long ulNetxAddress, unsigned short usData)
{
	wxString strMsg;
	int iResult;


	iResult = m_tFunctionInterface.fn_write_data16(m_pvHandle, ulNetxAddress, usData);
	if( iResult!=0 )
	{
		strMsg.Printf(wxT("write_data16 failed with error %d"), iResult);
		m_ptLuaState->terror(strMsg);
	}
}


// write a long (32bit) from the pc to the netx
void romloader::write_data32(unsigned long ulNetxAddress, unsigned long ulData)
{
	wxString strMsg;
	int iResult;


	iResult = m_tFunctionInterface.fn_write_data32(m_pvHandle, ulNetxAddress, ulData);
	if( iResult!=0 )
	{
		strMsg.Printf(wxT("write_data32 failed with error %d"), iResult);
		m_ptLuaState->terror(strMsg);
	}
}


// write a byte array from the pc to the netx
void romloader::write_image(unsigned long ulNetxAddress, wxString strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	wxString strMsg;
	int iResult;


	iResult = m_tFunctionInterface.fn_write_image(m_pvHandle, ulNetxAddress, strData.To8BitData(), strData.Len(), L, iLuaCallbackTag, pvCallbackUserData);
	if( iResult!=0 )
	{
		strMsg.Printf(wxT("write_image failed with error %d"), iResult);
		m_ptLuaState->terror(strMsg);
	}
}


// call routine
void romloader::call(unsigned long ulNetxAddress, unsigned long ulParameterR0)
{
	wxString strMsg;
	int iResult;


	iResult = m_tFunctionInterface.fn_call(m_pvHandle, ulNetxAddress, ulParameterR0);
	if( iResult!=0 )
	{
		strMsg.Printf(wxT("call failed with error %d"), iResult);
		m_ptLuaState->terror(strMsg);
	}
}

