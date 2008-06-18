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


#include "dpm.h"


dpm::dpm(wxString strName, wxString strTyp, const dpm_functioninterface *ptFn, void *pvHandle, muhkuh_plugin_fn_close_instance fn_close, wxLuaState *ptLuaState)
 : wxObject()
 , m_strName(strName)
 , m_strTyp(strTyp)
 , m_pvHandle(pvHandle)
 , m_fn_close(fn_close)
 , m_ptLuaState(ptLuaState)
{
	if( ptFn==NULL )
	{
		memset(&m_tFunctionInterface, 0, sizeof(dpm_functioninterface));
	}
	else
	{
		memcpy(&m_tFunctionInterface, ptFn, sizeof(dpm_functioninterface));
	}

	// *** DEBUG ***
	wxLogMessage(wxT("created dpm at %p"), this);
}


dpm::~dpm(void)
{
	if( m_fn_close!=NULL )
	{
		m_fn_close(m_pvHandle);
	}

	// *** DEBUG ***
	wxLogMessage(wxT("deleted dpm at %p"), this);
}


// open the connection to the device
void dpm::connect(void)
{
	wxString strMsg;


	if( m_tFunctionInterface.fn_connect==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing connect function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		m_tFunctionInterface.fn_connect(m_pvHandle);
	}
}


// close the connection to the device
void dpm::disconnect(void)
{
	wxString strMsg;


	if( m_tFunctionInterface.fn_disconnect==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing disconnect function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		m_tFunctionInterface.fn_disconnect(m_pvHandle);
	}
}


// returns the connection state of the device
bool dpm::is_connected(void) const
{
	bool fIsConnected = false;
	wxString strMsg;


	if( m_tFunctionInterface.fn_is_connected==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing is_connected function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		fIsConnected = m_tFunctionInterface.fn_is_connected(m_pvHandle);
	}

	return fIsConnected;
}


// returns the device name
wxString dpm::get_name(void)
{
	return m_strName;
}


// returns the device typ
wxString dpm::get_typ(void)
{
	return m_strTyp;
}


// read a byte (8bit) from the dpm
unsigned char dpm::read_data08(unsigned long ulNetxAddress)
{
	wxString strMsg;
	unsigned char ucData = 0x00;
	int iResult;


	if( m_tFunctionInterface.fn_read_data08==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing read_data08 function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		iResult = m_tFunctionInterface.fn_read_data08(m_pvHandle, ulNetxAddress, &ucData);
		if( iResult!=0 )
		{
			strMsg.Printf(wxT("read_data08 failed with error %d"), iResult);
			m_ptLuaState->wxlua_Error(strMsg);
		}
	}

	return ucData;
}


// read a word (16bit) from the dpm
unsigned short dpm::read_data16(unsigned long ulNetxAddress)
{
	wxString strMsg;
	unsigned short usData = 0x0000;
	int iResult;


	if( m_tFunctionInterface.fn_read_data16==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing read_data16 function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		iResult = m_tFunctionInterface.fn_read_data16(m_pvHandle, ulNetxAddress, &usData);
		if( iResult!=0 )
		{
			strMsg.Printf(wxT("read_data16 failed with error %d"), iResult);
			m_ptLuaState->wxlua_Error(strMsg);
		}
	}

	return usData;
}


// read a long (32bit) from the dpm
unsigned long dpm::read_data32(unsigned long ulNetxAddress)
{
	wxString strMsg;
	unsigned long ulData = 0x00000000;
	int iResult;


	if( m_tFunctionInterface.fn_read_data32==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing read_data32 function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		iResult = m_tFunctionInterface.fn_read_data32(m_pvHandle, ulNetxAddress, &ulData);
		if( iResult!=0 )
		{
			strMsg.Printf(wxT("read_data32 failed with error %d"), iResult);
			m_ptLuaState->wxlua_Error(strMsg);
		}
	}

	return ulData;
}


// read a byte array from the dpm
wxString dpm::read_image(unsigned long ulNetxAddress, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	wxString strMsg;
	wxString strData;
	char *pcBuffer;
	int iResult;


	if( m_tFunctionInterface.fn_read_image==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing read_image function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		// check parameter
		if( ulSize!=0 )
		{
			// get memory for the buffer
			pcBuffer = new char[ulSize];
			iResult = m_tFunctionInterface.fn_read_image(m_pvHandle, ulNetxAddress, pcBuffer, ulSize, L, iLuaCallbackTag, pvCallbackUserData);
			if( iResult!=0 )
			{
				strMsg.Printf(wxT("read_image failed with error %d"), iResult);
				// free the buffer
				delete[] pcBuffer;
				m_ptLuaState->wxlua_Error(strMsg);
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
	}

	return strData;
}

// write a byte (8bit) to the dpm
void dpm::write_data08(unsigned long ulNetxAddress, unsigned char ucData)
{
	wxString strMsg;
	int iResult;


	if( m_tFunctionInterface.fn_write_data08==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing write_data08 function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		iResult = m_tFunctionInterface.fn_write_data08(m_pvHandle, ulNetxAddress, ucData);
		if( iResult!=0 )
		{
			strMsg.Printf(wxT("write_data08 failed with error %d"), iResult);
			m_ptLuaState->wxlua_Error(strMsg);
		}
	}
}


// write a word (16bit) to the dpm
void dpm::write_data16(unsigned long ulNetxAddress, unsigned short usData)
{
	wxString strMsg;
	int iResult;


	if( m_tFunctionInterface.fn_write_data16==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing write_data16 function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		iResult = m_tFunctionInterface.fn_write_data16(m_pvHandle, ulNetxAddress, usData);
		if( iResult!=0 )
		{
			strMsg.Printf(wxT("write_data16 failed with error %d"), iResult);
			m_ptLuaState->wxlua_Error(strMsg);
		}
	}
}


// write a long (32bit) to the dpm
void dpm::write_data32(unsigned long ulNetxAddress, unsigned long ulData)
{
	wxString strMsg;
	int iResult;


	if( m_tFunctionInterface.fn_write_data32==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing write_data32 function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		iResult = m_tFunctionInterface.fn_write_data32(m_pvHandle, ulNetxAddress, ulData);
		if( iResult!=0 )
		{
			strMsg.Printf(wxT("write_data32 failed with error %d"), iResult);
			m_ptLuaState->wxlua_Error(strMsg);
		}
	}
}


// write a byte array to the dpm
void dpm::write_image(unsigned long ulNetxAddress, wxString strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	wxString strMsg;
	int iResult;


	if( m_tFunctionInterface.fn_write_image==NULL )
	{
		strMsg.Printf(m_strName + wxT(" (%p): missing write_image function, ignoring request."), this);
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
		iResult = m_tFunctionInterface.fn_write_image(m_pvHandle, ulNetxAddress, strData.To8BitData(), strData.Len(), L, iLuaCallbackTag, pvCallbackUserData);
		if( iResult!=0 )
		{
			strMsg.Printf(wxT("write_image failed with error %d"), iResult);
			m_ptLuaState->wxlua_Error(strMsg);
		}
	}
}

