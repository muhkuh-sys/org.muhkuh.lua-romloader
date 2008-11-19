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
#include <wx/regex.h>

#include "romloader_uart_main.h"
#include "_luaif/romloader_uart_wxlua_bindings.h"

/*-------------------------------------*/

static muhkuh_plugin_desc plugin_desc =
{
	wxT("Romloader Uart Plugin"),
	wxT(""),
	{ 0, 0, 1 }
};


static wxLuaState *m_ptLuaState;

static wxArrayString astrDetectedDevices;
static wxArrayString astrBusyDevices;

/*-------------------------------------*/

int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId)
{
	wxLog *pOldLogTarget;


	// clear lists
	astrDetectedDevices.Clear();
	astrBusyDevices.Clear();

	/* set main app's log target */
	pOldLogTarget = wxLog::GetActiveTarget();
	if( pOldLogTarget!=ptLogTarget )
	{
		wxLog::SetActiveTarget(ptLogTarget);
		if( pOldLogTarget!=NULL )
		{
			delete pOldLogTarget;
		}
	}

	/* say hi */
	wxLogMessage(wxT("romloader uart plugin init"));

	/* remember id */
	plugin_desc.strPluginId = strPluginId;

	/* init the lua state */
	m_ptLuaState = NULL;

	return 0;
}


/*-------------------------------------*/

int fn_init_lua(wxLuaState *ptLuaState)
{
	bool fSuccess;


	/* remember the lua state for instance creation */
	m_ptLuaState = ptLuaState;

	/* init the lua bindings */
	fSuccess = wxLuaBinding_romloader_uart_lua_init();
	if( fSuccess!=true )
	{
		wxLogMessage(wxT("failed to init romloader_uart lua bindings"));
		return -1;
	}

	return 0;
}


/*-------------------------------------*/

int fn_leave(void)
{
	size_t sizLeft;
	size_t sizCnt;


	wxLogMessage(wxT("romloader uart plugin leave"));

	sizLeft = astrBusyDevices.GetCount();
	if( sizLeft>0 )
	{
		wxLogError(wxT("not all instances closed:"));
		sizCnt = 0;
		do
		{
			wxLogError(astrBusyDevices.Item(sizCnt));
			++sizCnt;
		} while( sizCnt<sizLeft );
	}

	return 0;
}

/*-------------------------------------*/

const muhkuh_plugin_desc *fn_get_desc(void)
{
	return &plugin_desc;
}

/*-------------------------------------*/

int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList)
{
	size_t sizCnt, sizMax;
	wxString strInterface;
	bool fIsInUse;
	muhkuh_plugin_instance *ptInst;


	// clear list of detected devices
	astrDetectedDevices.Clear();

	// detect all interfaces
	romloader_uart_device_platform::ScanForPorts(&astrDetectedDevices);

	sizCnt = 0;
	sizMax = astrDetectedDevices.Count();
	while( sizCnt<sizMax )
	{
		strInterface = astrDetectedDevices.Item(sizCnt);
		// is the interface in use?
		fIsInUse = (astrBusyDevices.Index(strInterface, true, false)!=wxNOT_FOUND );

		ptInst = new muhkuh_plugin_instance(strInterface, plugin_desc.strPluginId, fIsInUse, wxT("muhkuh.romloader_uart_create"), (void*)sizCnt);
		pvInterfaceList->push_back(ptInst);

		++sizCnt;
	}

	return sizMax;
}


/*-------------------------------------*/

void romloader_uart_close_instance(wxString &strInterface)
{
	int iIdx;


	wxLogMessage(wxT("romloader_uart: close instance ") + strInterface);

	iIdx = astrBusyDevices.Index(strInterface, true, false);
	if( iIdx==wxNOT_FOUND )
	{
		wxLogMessage(wxT("romloader_uart: close request for unknown interface: ") + strInterface);
	}
	else
	{
		astrBusyDevices.RemoveAt(iIdx);
	}
}


/*-------------------------------------*/


romloader *romloader_uart_create(void *pvHandle)
{
	size_t sizIdx;
	wxString strInterface;
	wxString strTyp;
	romloader *ptInstance;
	bool fIsUsed;


	ptInstance = NULL;

	sizIdx = (size_t)pvHandle;
	if( sizIdx>=0 && sizIdx<astrDetectedDevices.Count() )
	{
		strInterface = astrDetectedDevices.Item(sizIdx);
		// is the device really free?
		if( astrBusyDevices.Index(strInterface, true, false)!=wxNOT_FOUND )
		{
			wxLogMessage(wxT("Interface %s is already in use!"), strInterface.fn_str());
		}
		else
		{
			strTyp = plugin_desc.strPluginId;
			ptInstance = new romloader_uart(strInterface, plugin_desc.strPluginId, NULL, pvHandle, romloader_uart_close_instance, m_ptLuaState);
			// add this interface to the busy list
			astrBusyDevices.Add(strInterface);
		}
	}

	return ptInstance;
}


/*-------------------------------------*/


romloader_uart::romloader_uart(wxString strName, wxString strTyp, const romloader_functioninterface *ptFn, void *pvHandle, romloader_uart_plugin_fn_close_instance fn_close, wxLuaState *ptLuaState)
 : romloader(strName, strTyp, NULL, NULL, NULL, ptLuaState)
 , m_fIsConnected(false)
 , m_ptUartDev(NULL)
 , m_fn_uart_close(fn_close)
{
	m_strMe.Printf(wxT("romloader_uart(%p): "), this);

	wxLogMessage(m_strMe + wxT("constructor"));

	m_strInterface = strName;
	m_strTyp = strTyp;

	m_ptUartDev = new romloader_uart_device_platform(strName);
}


romloader_uart::~romloader_uart(void)
{
	wxLogMessage(m_strMe + wxT("destructor"));

  if( m_fIsConnected==true )
  {
    disconnect();
  }

	if( m_fn_uart_close!=NULL )
	{
		m_fn_uart_close(m_strInterface);
	}
}


bool romloader_uart::chip_init(void)
{
	bool fResult;


	switch( m_tChiptyp )
	{
	case ROMLOADER_CHIPTYP_NETX500:
	case ROMLOADER_CHIPTYP_NETX100:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// aboot does not set the serial vectors
			write_data32(0x10001ff0, 0);
			write_data32(0x10001ff4, 0);
			write_data32(0x10001ff8, 0);
			write_data32(0x10001ffc, 0);
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_NETX50:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			// netx50 needs sdram fix
			wxLogMessage(wxT("running netx50 sdram fix..."));
			write_data32(0x1c005830, 0x00000001);
			write_data32(0x1c005104, 0xbffffffc);
			write_data32(0x1c00510c, 0x00480001);
			write_data32(0x1c005110, 0x00000001);
			wxLogMessage(wxT("netx50 sdram fix ok!"));
			write_data32(0x1c00510c, 0);
			write_data32(0x1c005110, 0);
			write_data32(0x1c005830, 0);
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_UNKNOWN:
		fResult = false;
		break;
	}

	return fResult;
}


void romloader_uart::connect(void)
{
	wxString strErrorMsg;


	wxLogMessage(m_strMe + wxT("connect"));

	if( m_ptUartDev!=NULL && m_fIsConnected==false )
	{
		if( m_ptUartDev->Open()!=true )
		{
			strErrorMsg = wxT("failed to open device!");
		}
		else if( m_ptUartDev->IdentifyLoader()!=true )
		{
			strErrorMsg = wxT("failed to identify loader!");
		}
		else if( detect_chiptyp()!=true )
		{
			strErrorMsg = wxT("failed to detect chiptyp!");
		}
		else if( chip_init()!=true )
		{
			strErrorMsg = wxT("failed to init chip!");
		}
		else
		{
			m_fIsConnected = true;
		}

		if( m_fIsConnected!=true )
		{
			m_ptUartDev->Close();
			wxLogError(m_strMe + strErrorMsg);
			m_ptLuaState->wxlua_Error(strErrorMsg);
		}
	}
}


void romloader_uart::disconnect(void)
{
	wxLogMessage(m_strMe + wxT("disconnect"));

	if( m_ptUartDev!=NULL )
	{
		m_ptUartDev->Close();
	}

	m_fIsConnected = false;
}


bool romloader_uart::is_connected(void) const
{
	wxLogMessage(m_strMe + wxT("is_connected: %s"), m_fIsConnected?wxT("true"):wxT("false"));

	return m_fIsConnected;
}


double romloader_uart::read_data08(double dNetxAddress)
{
	unsigned long ulValue;
	wxString strCmd;
	wxString strResponse;
	wxString strHexValue;
	wxString strErrorMsg;
	wxRegEx reDumpResponse(wxT("([0-9A-Fa-f]{8}): ([0-9A-Fa-f]{2}) "));
	unsigned long ulNetxAddress;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;

	// expect failure
	fOk = false;

	// set default value
	ulValue = 0;

	// construct command
	strCmd.Printf(wxT("DUMP %lX BYTE"), ulNetxAddress);
	// send command
	if( m_ptUartDev->SendCommand(strCmd, 1000)!=true )
	{
		strErrorMsg = wxT("failed to send dump command to device");
	}
	// get the response line
	else if( m_ptUartDev->GetLine(strResponse, "\r\n", 1000)!=true )
	{
		strErrorMsg = wxT("failed to get dump response from device");
	}
	else if( reDumpResponse.Matches(strResponse)!=true )
	{
		strErrorMsg = wxT("strange response from device:") + strResponse;
	}
	else
	{
		strHexValue = reDumpResponse.GetMatch(strResponse, 1);
		if( strHexValue.ToULong(&ulValue, 16)!=true )
		{
			strErrorMsg = wxT("failed to extract address from response:") + strResponse;
		}
		else if( ulValue!=ulNetxAddress )
		{
			strErrorMsg = wxT("address does not match request:") + strResponse;
		}
		else
		{
			strHexValue = reDumpResponse.GetMatch(strResponse, 2);
			if( strHexValue.ToULong(&ulValue, 16)!=true )
			{
				strErrorMsg = wxT("failed to extract value from response:") + strResponse;
			}
			else if( m_ptUartDev->WaitForResponse(strResponse, 65536, 1024)==false )
			{
				strErrorMsg = wxT("failed to get a command prompt!");
			}
			else
			{
				wxLogMessage(wxT("read_data08 from %08lx: %08lx"), ulNetxAddress, ulValue);
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return (unsigned char)ulValue;
}


double romloader_uart::read_data16(double dNetxAddress)
{
	unsigned long ulValue;
	wxString strCmd;
	wxString strResponse;
	wxString strHexValue;
	wxString strErrorMsg;
	wxRegEx reDumpResponse(wxT("([0-9A-Fa-f]{8}): ([0-9A-Fa-f]{4}) "));
	unsigned long ulNetxAddress;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;

	// expect failure
	fOk = false;

	// set default value
	ulValue = 0;

	// construct command
	strCmd.Printf(wxT("DUMP %lX WORD"), ulNetxAddress);
	// send command
	if( m_ptUartDev->SendCommand(strCmd, 1000)!=true )
	{
		strErrorMsg = wxT("failed to send dump command to device");
	}
	// get the response line
	else if( m_ptUartDev->GetLine(strResponse, "\r\n", 1000)!=true )
	{
		strErrorMsg = wxT("failed to get dump response from device");
	}
	else if( reDumpResponse.Matches(strResponse)!=true )
	{
		strErrorMsg = wxT("strange response from device:") + strResponse;
	}
	else
	{
		strHexValue = reDumpResponse.GetMatch(strResponse, 1);
		if( strHexValue.ToULong(&ulValue, 16)!=true )
		{
			strErrorMsg = wxT("failed to extract address from response:") + strResponse;
		}
		else if( ulValue!=ulNetxAddress )
		{
			strErrorMsg = wxT("address does not match request:") + strResponse;
		}
		else
		{
			strHexValue = reDumpResponse.GetMatch(strResponse, 2);
			if( strHexValue.ToULong(&ulValue, 16)!=true )
			{
				strErrorMsg = wxT("failed to extract value from response:") + strResponse;
			}
			else if( m_ptUartDev->WaitForResponse(strResponse, 65536, 1024)==false )
			{
				strErrorMsg = wxT("failed to get a command prompt!");
			}
			else
			{
				wxLogMessage(wxT("read_data16 from %08lx: %08lx"), ulNetxAddress, ulValue);
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return (unsigned short)ulValue;
}


double romloader_uart::read_data32(double dNetxAddress)
{
	unsigned long ulValue;
	wxString strCmd;
	wxString strResponse;
	wxString strHexValue;
	wxString strErrorMsg;
	wxRegEx reDumpResponse(wxT("([0-9A-Fa-f]{8}): ([0-9A-Fa-f]{8}) "));
	unsigned long ulNetxAddress;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;

	// expect failure
	fOk = false;

	// set default value
	ulValue = 0;

	// construct command
	strCmd.Printf(wxT("DUMP %lX"), ulNetxAddress);
	// send command
	if( m_ptUartDev->SendCommand(strCmd, 1000)!=true )
	{
		strErrorMsg = wxT("failed to send dump command to device");
	}
	// get the response line
	else if( m_ptUartDev->GetLine(strResponse, "\r\n", 1000)!=true )
	{
		strErrorMsg = wxT("failed to get dump response from device");
	}
	else if( reDumpResponse.Matches(strResponse)!=true )
	{
		strErrorMsg = wxT("strange response from device:") + strResponse;
	}
	else
	{
		strHexValue = reDumpResponse.GetMatch(strResponse, 1);
		if( strHexValue.ToULong(&ulValue, 16)!=true )
		{
			strErrorMsg = wxT("failed to extract address from response:") + strResponse;
		}
		else if( ulValue!=ulNetxAddress )
		{
			strErrorMsg = wxT("address does not match request:") + strResponse;
		}
		else
		{
			strHexValue = reDumpResponse.GetMatch(strResponse, 2);
			if( strHexValue.ToULong(&ulValue, 16)!=true )
			{
				strErrorMsg = wxT("failed to extract value from response:") + strResponse;
			}
			else if( m_ptUartDev->WaitForResponse(strResponse, 65536, 1024)==false )
			{
				strErrorMsg = wxT("failed to get a command prompt!");
			}
			else
			{
				wxLogMessage(wxT("read_data32 from %08lx: %08lx"), ulNetxAddress, ulValue);
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return ulValue;
}


bool romloader_uart::parseDumpLine(const char *pcLine, size_t sizLineLen, unsigned long ulAddress, unsigned long ulElements, unsigned char *pucBuffer, wxString &strErrorMsg)
{
	bool fResult;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned long ulChunkCnt;
	unsigned int uiByte;


	// expect success
	fResult = true;

	// is enough input data left?
	if( sizLineLen<(10+ulElements*3) )
	{
		strErrorMsg = wxT("strange response from netx!");
		fResult = false;
	}
	else
	{
		// get the address
		iMatches = sscanf(pcLine, "%8lX: ", &ulResultAddress);
		if( iMatches!=1 )
		{
			strErrorMsg = wxT("strange response from netx!");
			fResult = false;
		}
		else if( ulResultAddress!=ulAddress )
		{
			strErrorMsg = wxT("response does not match request!");
			fResult = false;
		}
		else
		{
			// advance parse ptr to data part of the line
			pcLine += 10;
			// get all bytes
			ulChunkCnt = ulElements;
			while( ulChunkCnt!=0 )
			{
				// get one hex digit
				iMatches = sscanf(pcLine, "%2X ", &uiByte);
				if( iMatches!=1 )
				{
					strErrorMsg = wxT("strange response from netx!");
					fResult = false;
					break;
				}
				// advance parse ptr to data part of the line
				pcLine += 3;
				*(pucBuffer++) = (char)uiByte;
				// one number processed
				--ulChunkCnt;
			}
		}
	}

	return fResult;
}


wxString romloader_uart::read_image(double dNetxAddress, double dSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned long ulNetxAddress;
	unsigned long ulSize;
	wxString strCommand;
	wxString strData;
	wxString strResponse;
	wxString strErrorMsg;
	unsigned char *pucData;
	unsigned char *pucDataCnt;
	unsigned long ulExpectedAddress;
	unsigned long ulBytesLeft;
	unsigned long ulChunkSize;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulSize = (unsigned long)dSize;

	fOk = false;

	wxLogMessage(m_strMe + wxT("read_image from 0x%08lx, len=0x%08lx"), ulNetxAddress, ulSize);

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX %08lX BYTE"), ulNetxAddress, ulSize);

	// send command
	if( m_ptUartDev->SendCommand(strCommand, 1000)!=true )
	{
		strErrorMsg = wxT("failed to send dump command to device");
	}
	else
	{
		pucData = (unsigned char*)malloc(ulSize);
		if( pucData==NULL )
		{
			strErrorMsg.Printf(wxT("failed to alloc %d bytes of input buffer!"), ulSize);
		}
		else
		{
			pucDataCnt = pucData;
			// parse the result
			ulBytesLeft = ulSize;
			ulExpectedAddress = ulNetxAddress;
			while( ulBytesLeft>0 )
			{
				fOk = callback_long(L, iLuaCallbackTag, ulSize-ulBytesLeft, pvCallbackUserData);
				if( fOk!=true )
				{
					strErrorMsg = wxT("operation canceled!");
					break;
				}

				// get the response line
				if( m_ptUartDev->GetLine(strResponse, "\r\n", 1000)!=true )
				{
					fOk = false;
					strErrorMsg = wxT("failed to get dump response from device");
					break;
				}
				else
				{
					// get the number of expected bytes in the next row
					ulChunkSize = 16;
					if( ulChunkSize>ulBytesLeft )
					{
						ulChunkSize = ulBytesLeft;
					}
					fOk = parseDumpLine(strResponse.To8BitData(), strResponse.Len(), ulExpectedAddress, ulChunkSize, pucDataCnt, strErrorMsg);
					if( fOk!=true )
					{
						break;
					}
					else
					{
						ulBytesLeft -= ulChunkSize;
						// inc address
						ulExpectedAddress += ulChunkSize;
						// inc buffer ptr
						pucDataCnt += ulChunkSize;
					}
				}
			}

			if( fOk==true )
			{
				// get data
				strData = wxString::From8BitData((const char*)pucData, ulSize);

				// wait for prompt
				fOk = m_ptUartDev->WaitForResponse(strResponse, 65536, 1024);
				if( fOk!=true )
				{
					strErrorMsg = wxT("failed to get a command prompt!");
				}
			}
		}

		// free buffer
		free(pucData);
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return strData;
}


void romloader_uart::write_data08(double dNetxAddress, double dData)
{
	wxString strCmd;
	wxString strResponse;
	wxString strErrorMsg;
	unsigned long ulNetxAddress;
	unsigned char ucData;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ucData = (unsigned char)dData;

	// expect failure
	fOk = false;

	// construct the command
	strCmd.Printf(wxT("FILL %08lX %02X BYTE"), ulNetxAddress, ucData);
	// send command
	if( m_ptUartDev->SendCommand(strCmd, 1000)!=true )
	{
		strErrorMsg = wxT("failed to send fill command to device");
	}
	else if( m_ptUartDev->WaitForResponse(strResponse, 65536, 1024)==false )
	{
		strErrorMsg = wxT("failed to get a command prompt!");
	}
	else
	{
		wxLogMessage(wxT("write_data08 %08lx=%02x"), ulNetxAddress, ucData);
		fOk = true;
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


void romloader_uart::write_data16(double dNetxAddress, double dData)
{
	wxString strCmd;
	wxString strResponse;
	wxString strErrorMsg;
	unsigned long ulNetxAddress;
	unsigned short usData;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	usData = (unsigned short)dData;

	// expect failure
	fOk = false;

	// construct the command
	strCmd.Printf(wxT("FILL %08lX %04X WORD"), ulNetxAddress, usData);
	// send command
	if( m_ptUartDev->SendCommand(strCmd, 1000)!=true )
	{
		strErrorMsg = wxT("failed to send fill command to device");
	}
	else if( m_ptUartDev->WaitForResponse(strResponse, 65536, 1024)==false )
	{
		strErrorMsg = wxT("failed to get a command prompt!");
	}
	else
	{
		wxLogMessage(wxT("write_data16 %08lx=%04x"), ulNetxAddress, usData);
		fOk = true;
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


void romloader_uart::write_data32(double dNetxAddress, double dData)
{
	wxString strCmd;
	wxString strResponse;
	wxString strErrorMsg;
	unsigned long ulNetxAddress;
	unsigned long ulData;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulData = (unsigned long)dData;

	// expect failure
	fOk = false;

	// construct the command
	strCmd.Printf(wxT("FILL %08lX %08lX"), ulNetxAddress, ulData);
	// send command
	if( m_ptUartDev->SendCommand(strCmd, 1000)!=true )
	{
		strErrorMsg = wxT("failed to send fill command to device");
	}
	else if( m_ptUartDev->WaitForResponse(strResponse, 65536, 1024)==false )
	{
		strErrorMsg = wxT("failed to get a command prompt!");
	}
	else
	{
		wxLogMessage(wxT("write_data32 %08lx=%08lx"), ulNetxAddress, ulData);
		fOk = true;
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


unsigned int romloader_uart::buildCrc(const char *pcData, size_t sizDataLen)
{
  const unsigned char *pucCnt;
  const unsigned char *pucEnd;
  unsigned int uiCrc;


  pucCnt = (const unsigned char*)pcData;
  pucEnd = pucCnt + sizDataLen;

  uiCrc = 0xffff;
  while(pucCnt<pucEnd)
  {
    uiCrc = crc16(uiCrc, *(pucCnt++));
  }

  return uiCrc;
}


int romloader_uart::write_data(wxString &strData, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData, wxString &strErrorMsg)
{
	size_t sizDataCnt, sizDataLen;
	unsigned int uiCrc;
	wxString strCommand;
	size_t sizChunkSize;
	bool fIsRunning;
	size_t sizMaxChunkSize;
	int iResult;
	wxString strResponse;
	unsigned long ulSent;
	unsigned long ulPeek;


	// assume success
	iResult = 0;

	sizMaxChunkSize = m_ptUartDev->GetMaxBlockSize();

	// get the data length
	sizDataLen = strData.Length();

	// generate crc checksum
	uiCrc = buildCrc(strData.fn_str(), strData.Len());

	// generate load command
	strCommand.Printf(wxT("LOAD %08lX %08X %04X"), ulLoadAdr, sizDataLen, uiCrc);
	wxLogMessage(m_strMe + wxT("Command: ") + strCommand);

	// send command
	if( m_ptUartDev->SendCommand(strCommand, 1000)!=true )
	{
		strErrorMsg = wxT("failed to send load command to device");
		iResult = -1;
	}
	else
	{
		// now send the data part
		sizDataCnt = 0;
		while( sizDataCnt<sizDataLen )
		{
			// check for response
			ulPeek = m_ptUartDev->Peek();
			if( ulPeek!=0 )
			{
				// error message
				strErrorMsg.Printf(wxT("received error during transmission, peek reported %d"), ulPeek);
				iResult = -3;
				// get netx message
				if( m_ptUartDev->WaitForResponse(strResponse, 65536, 1024)==false )
				{
					strErrorMsg.Append(wxT(" failed to get netx response!"));
				}
				else
				{
				  wxLogMessage(wxT("netx response: ") + strResponse);
				}
			}

			// get the size of the next data chunk
			sizChunkSize = sizDataLen - sizDataCnt;
			if( sizChunkSize>sizMaxChunkSize )
			{
				sizChunkSize = sizMaxChunkSize;
			}

			fIsRunning = callback_long(L, iLuaCallbackTag, sizDataCnt, pvCallbackUserData);
			if( fIsRunning!=true )
			{
				strErrorMsg = wxT("operation canceled!");
				iResult = -2;
				break;
			}

			// send data chunk
			ulSent = m_ptUartDev->SendRaw((const unsigned char*)strData.Mid(sizDataCnt, sizChunkSize).To8BitData(), sizChunkSize, 1000);
			if( ulSent!=sizChunkSize )
			{
				strErrorMsg.Printf(wxT("failed to send %d bytes: %d"), sizChunkSize, ulSent);
				iResult = -1;
				break;
			}
	    // flush the port to get all data out
	    if( m_ptUartDev->Flush()!=true )
	    {
				strErrorMsg = wxT("failed to flush the port");
				iResult = -1;
				break;
	    }

			sizDataCnt += sizChunkSize;
		}
	}

	return iResult;
}


void romloader_uart::write_image(double dNetxAddress, wxString strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned long ulNetxAddress;
	wxString strResponse;
	wxString strErrorMsg;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;

	// expect failure
	fOk = false;

	wxLogMessage(m_strMe + wxT("write_image %08lx"), ulNetxAddress);

	// send the command
	if( write_data(strData, ulNetxAddress, L, iLuaCallbackTag, pvCallbackUserData, strErrorMsg)==0 )
	{
		// get the response
		if( m_ptUartDev->WaitForResponse(strResponse, 65536, 1024)==false )
		{
			strErrorMsg = wxT("failed to get a command prompt!");
		}
		else
		{
			fOk = true;
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


void romloader_uart::call(double dNetxAddress, double dParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned long ulNetxAddress;
	unsigned long ulParameterR0;
	wxString strCommand;
	wxString strErrorMsg;
	wxString strCallbackData;
	bool fIsRunning;
	unsigned long ulChunkRead;
	unsigned char *pucBuf;
	unsigned char *pucCnt, *pucEnd;
	unsigned char aucSbuf[3] = { 0, 0, 0 };
	const size_t sizBufLen = 1024;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulParameterR0 = (unsigned long)dParameterR0;

	// expect failure
	fOk = false;

	wxLogMessage(m_strMe + wxT("call %08x(%08x)"), ulNetxAddress, ulParameterR0);

	// construct the "call" command
	strCommand.Printf(wxT("CALL %08lX %08X"), ulNetxAddress, ulParameterR0);
	// send command
	if( m_ptUartDev->SendCommand(strCommand, 1000)!=true )
	{
		strErrorMsg = wxT("failed to send call command to device");
	}
	else
	{
		pucBuf = (unsigned char*)malloc(sizBufLen);
		if( pucBuf==NULL )
		{
			strErrorMsg.Printf(wxT("failed to alloc %d bytes for receive buffer"), sizBufLen);
		}
		else
		{
			// wait for the call to finish
			do
			{
				// execute callback
				fIsRunning = callback_string(L, iLuaCallbackTag, strCallbackData, pvCallbackUserData);
				strCallbackData.Empty();
				if( fIsRunning==true )
				{
					// look for data from netx
					ulChunkRead = m_ptUartDev->RecvRaw(pucBuf, sizBufLen, 200);
					if( ulChunkRead>0 )
					{
						// get data
						// NOTE: use Append to make a real copy
						strCallbackData.Append( wxString::From8BitData((const char*)pucBuf, ulChunkRead) );

						// scan data for command prompt
						pucCnt = pucBuf;
						pucEnd = pucCnt + ulChunkRead;
						while( pucCnt<pucEnd )
						{
							aucSbuf[0] = aucSbuf[1];
							aucSbuf[1] = aucSbuf[2];
							aucSbuf[2] = *(pucCnt++);
							if( aucSbuf[0]=='\r' && aucSbuf[1]=='\n' && aucSbuf[2]=='>' )
							{
								// send the rest of the data
								callback_string(L, iLuaCallbackTag, strCallbackData, pvCallbackUserData);
								// call has finished
								fIsRunning = false;
								fOk = true;
								break;
							}
						}
					}
				}
				else
				{
					strErrorMsg = wxT("user pressed cancel");
				}
			} while( fIsRunning==true );

			// free the buffer
			free(pucBuf);
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}

