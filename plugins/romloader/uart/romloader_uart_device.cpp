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


#include "romloader_uart_device.h"

#include <wx/regex.h>


romloader_uart_device::romloader_uart_device(wxString strPortName)
 : m_strPortName(strPortName)
{ 
}


bool romloader_uart_device::SendString(wxString strData, unsigned long ulTimeout)
{
	unsigned long ulSentBytes;
	unsigned long ulDataLength;
	union
	{
		const char *pc;
		const unsigned char *puc;
	} uData;
	wxCharBuffer buf;


	// get data pointer
	buf = strData.mb_str();
	uData.pc = buf;
	// get data length
	ulDataLength = strData.Len();
	// send the data
	ulSentBytes = SendRaw(uData.puc, ulDataLength, ulTimeout);

	// were all bytes sent?
	return (ulSentBytes==ulDataLength);
}


bool romloader_uart_device::SendCommand(wxString strCmd, unsigned long ulTimeout)
{
	bool fResult;
	wxString strResponse;


	// append eol to command
	strCmd.Append(wxT('\r'));

	// try to send the command
	fResult = SendString(strCmd, ulTimeout);
	if( fResult!=true )
	{
		// failed to send the data
		return false;
	}

	// flush the port to get all data out
	fResult = Flush();
	if( fResult!=true )
	{
		// failed to flush the port
		return false;    
	}

	// receive response with the specified timeout
	fResult = GetLine(strResponse, "\r\n", ulTimeout);
	if( fResult!=true )
	{
		// failed to receive the response
		return false;
	}

	// expect echo
	strCmd.Append(wxT('\n'));
	if( strResponse.Cmp(strCmd)!=0 )
	{
		// error in response
		return false;
	}

	// all ok
	return true;
}


bool romloader_uart_device::Load(const unsigned char* pucData, unsigned long ulDataLen, unsigned long ulLoadAddress, PFN_LOAD_CALLBACK pfnCallback, void* pvUser)
{
	unsigned long ulCrc;
	unsigned long ulMaxBlockLen;
	wxString strCmd;
	wxString strResponse;
	wxString strMsg;
	bool fResult;
	bool fTransferSuccess;
	unsigned long ulIdx;
	unsigned long ulBytesTransferred;
	unsigned long ulToSendCnt;
	unsigned long ulProcessedBytes;


	//calculate CRC16 for image
	ulCrc = 0xffff;
	for(ulIdx = 0; ulIdx < ulDataLen; ulIdx++)
	{
		ulCrc = CalcCrc16(ulCrc, pucData[ulIdx]);
	}

	//build console command for netX
	strCmd.Printf(wxT("LOAD %X %X %X 10000"), ulLoadAddress, ulDataLen, ulCrc);
	// send command
	fResult = SendCommand(strCmd, 1000);
	if( fResult!=true )
	{
		wxLogError(wxT("failed to send load command to device"));
		return false;
	}

	// check for cr lf (only usb sends this)
	fResult = GetLine(strResponse, "\r\n", 200);
	if( fResult==true && strResponse.Cmp(wxT("\r\n"))!=0 )
	{
		// received something and it's not an empty line
		strMsg = wxT("received strange response from netX: ") + strResponse;
		wxLogError(strMsg);
		return false;
	}

	// no bytes transfered yet
	ulBytesTransferred = 0;
	// assume success
	fTransferSuccess = true;
	// get max block size
	ulMaxBlockLen = GetMaxBlockSize();
	while(ulBytesTransferred < ulDataLen)
	{
		ulProcessedBytes = Peek();
		if( ulProcessedBytes!=0 )
		{
			//error, we received an answer. The netX is not sending an answer during binary download
			strMsg = wxT("netX canceled upload operation!");
			wxLogError(strMsg);

			// get line
			fResult = GetLine(strResponse, "\r\n", 1000);
			if( fResult==true )
			{
				wxLogMessage(wxT("netx error: ") + strResponse);
			}

			fTransferSuccess = false;
			break;
		}

		ulToSendCnt = ulDataLen - ulBytesTransferred;
		if( ulToSendCnt>ulMaxBlockLen )
		{
			ulToSendCnt = ulMaxBlockLen;
		}

		// send next bytes to netX
		ulProcessedBytes = SendRaw(pucData + ulBytesTransferred, ulToSendCnt, 500);
		if( ulToSendCnt!=ulProcessedBytes )
		{
			//error, cannot send to device
			fTransferSuccess = false;
			strMsg.Printf(wxT("failed to send %d bytes to the netX!"), ulToSendCnt);
			wxLogError(strMsg);
			break;
		}

		// flush the port to get all data out
		fResult = Flush();
		if( fResult!=true )
		{
			// failed to flush the port
			return false;    
		}

		//block successfully transferred
		ulBytesTransferred += ulToSendCnt;

		if(NULL != pfnCallback)
		{
			if(!pfnCallback(pvUser, ulBytesTransferred, ulDataLen))
			{
				//user wants to cancel
				fTransferSuccess = false;
				break;
			}
		}
	}

	if( fTransferSuccess==true )
	{
		fTransferSuccess = GetPrompt(1000);
	}

	return fTransferSuccess;
}


bool romloader_uart_device::GetPrompt(unsigned long ulTimeout)
{
	bool fResult;
	unsigned char ucData;
	wxString strResponse;


	// get empty line
	fResult = GetLine(strResponse, "\r\n", ulTimeout);
	if( fResult!=true )
	{
		return false;
	}
	if( strResponse.Cmp(wxT("\r\n"))!=0 )
	{
		return false;
	}

	// expect the prompt '>'
	if( RecvRaw(&ucData, 1, ulTimeout)!=1 )
	{
		return false;
	}
	if( ucData!='>' )
	{
		return false;
	}

	// all ok!
	return true;
}


unsigned int romloader_uart_device::CalcCrc16(unsigned int uiCrc, unsigned int uiData)
{
	uiCrc  = (uiCrc >> 8) | ((uiCrc & 0xff) << 8);
	uiCrc ^= uiData;
	uiCrc ^= (uiCrc & 0xff) >> 4;
	uiCrc ^= (uiCrc & 0x0f) << 12;
	uiCrc ^= ((uiCrc & 0xff) << 4) << 1;

	return uiCrc;
}


bool romloader_uart_device::GetLine(wxString &strData, const char *pcEol, unsigned long ulTimeout)
{
	bool fRet = false;
	unsigned char bRecv;

	// state 0 = normal read, 1 = got 0x0a, 2 = got 0x0a and 0x0d
	typedef enum 
	{
		eSerialGetLineWaitFirstEolChar,
		eSerialGetLineWaitSecondEolChar,
		eSerialGetLineFinished,
	} SERIAL_GETLINE_STATE_E;
	SERIAL_GETLINE_STATE_E eState = eSerialGetLineWaitFirstEolChar;


	// clear the string
	strData.Empty();

	do
	{
		if(0 == RecvRaw(&bRecv, 1, ulTimeout))
		{
			/* timeout receiving char */
			break;
		}
		else
		{
			strData += (char)bRecv;

			switch(eState)
			{
			case eSerialGetLineWaitFirstEolChar:
				if(bRecv == pcEol[0])
				{
					// found first eol char -> goto next state
					if(pcEol[1] != 0)
					{
						// expect next eol char
						eState = eSerialGetLineWaitSecondEolChar;
					}
					else
					{
						// no more eol chars -> finish
						eState = eSerialGetLineFinished;
						fRet = true;
					}
				}
				break;

			case eSerialGetLineWaitSecondEolChar:
				// state 1 : already found the first eol char, now the 2nd must follow
				if(bRecv == pcEol[1])
				{
					// found 2nd eol char -> goto next state
					fRet = true;
					eState = eSerialGetLineFinished;
				}
				else
				{
					// not found 2nd eol char -> go back to state 0
					eState = eSerialGetLineWaitFirstEolChar;
				}
				break;

			case eSerialGetLineFinished:
				// state 2 : found both , exit
				fRet = true;
				break;

			default:
				// strange state
				eState = eSerialGetLineWaitFirstEolChar;
				break;
			}
		}
	} while( eState != eSerialGetLineFinished);

	return fRet;
}


bool romloader_uart_device::WaitForResponse(wxString &strData, size_t sizMaxLen, unsigned long ulTimeout)
{
	unsigned long ulReadBytes;
	unsigned char ucData;
	bool fOk;
	size_t sizRead;


	fOk = true;
	sizRead = 0;
	strData.Empty();

	do
	{
		ulReadBytes = RecvRaw(&ucData, 1, ulTimeout);
		if( ulReadBytes!=1 )
		{
			fOk = false;
			break;
		}
		strData += (char)ucData;
		++sizRead;
	} while( ucData!='>' && sizRead<sizMaxLen );

	return fOk;
}


bool romloader_uart_device::IdentifyLoader(void)
{
	bool fResult = false;
	wxRegEx reFirstResponse(wxT("netX.* Step [A-Za-z0-9]"));
	wxRegEx reInvalidCmd(wxT(".*\\?[0-9]{8}[\r\n]*>"));

	const unsigned char abKnock[2]        = { '*', '#' };

    wxString            strMsg;
    wxString            strHexValue;
    unsigned long       ulLength;
    unsigned long       ulValue;
    unsigned long       ulVersionMajor;
    unsigned long       ulVersionMinor;
    unsigned long       ulChipTyp;
    unsigned char       ucData;
    wxString            strResponse;
    unsigned char       abData[3];
    wxString            strNewInterfaceName;


    wxASSERT(reFirstResponse.IsValid());
    wxASSERT(reInvalidCmd.IsValid());


	// send knock sequence with 1 second timeout
	if( SendRaw(abKnock, 2, 1000)!=2 )
	{
		wxLogError(wxT("failed to send knock sequence to device"));
	}
	else if( Flush()!=true )
	{
		wxLogError(wxT("failed to flush the knock sequence"));
	}
	else if( (ulLength=RecvRaw(&ucData, 1, 1000))!=1 )
	{
		wxLogMessage(wxT("strange or no response from device, seems to be no netx"));
	}
	else
	{
	  // this should be '\f', but the first bits might be trashed
		if( ucData<0x20 )
		{
			// this seems to be the welcome message

			// receive the rest of the line
			fResult = GetLine(strResponse, "\r\n", 1000);
			if( fResult!=true )
			{
				wxLogMessage(wxT("strange or no response from device, seems to be no netx"));
			}
			else
			{
				// check for initial bootloader message
				fResult = reFirstResponse.Matches(strResponse);
				if( fResult!=true )
				{
					// seems to be no netX bootloader
					wxLogMessage(wxT("strange response from device, seems to be no netx"));
				}
				else
				{
					// get prompt, the real console eats the first space, the usb console will echo it
					fResult = false;
					ulLength = RecvRaw(abData, 3, 200);
					if( ulLength==0 )
					{
						wxLogMessage(wxT("strange or no response from device, seems to be no netx"));
					}
					else if( abData[0]!='>' )
					{
						wxLogMessage(wxT("strange or no response from device, seems to be no netx"));
					}
					else if( ulLength==1 )
					{
					  fResult = true;
					}
					else if( ulLength==2 && abData[1]=='#' )
					{
						fResult = true;
					}
					else if( ulLength==3 && abData[1]=='*' && abData[2]=='#' )
					{
						fResult = true;
					}
					else
					{
						wxLogMessage(wxT("strange or no response from device, seems to be no netx"));
					}
				}
			}
		}
		// knock echoed -> this is the prompt
		else if( ucData=='*' )
		{
			// get rest of knock sequence
			ulLength = RecvRaw(abData, 1, 100);
			if( ulLength==0 )
			{
				wxLogMessage(wxT("strange or no response from device, seems to be no netx"));
			}
			else if( abData[0]!='#' )
			{
				wxLogMessage(wxT("strange or no response from device, seems to be no netx"));
			}
			else
			{
				fResult = true;
			}
		}
		else if( ucData=='#' )
		{
			fResult = true;
		}
		else
		{
			wxLogMessage(wxT("strange or no response from device, seems to be no netx"));
		}
	}

	if( fResult==true )
	{
		// send enter
		fResult = SendCommand(wxT(""), 100);
		if( fResult!=true )
		{
			wxLogError(wxT("failed to send enter to device"));
		}
		else
		{
			fResult = WaitForResponse(strResponse, 65536, 1024);
			if( fResult!=true )
			{
				wxLogMessage(wxT("strange or no response from device, seems to be no netx"));
			}
			else
			{
				wxLogMessage(wxT("knock response:") + strResponse);
			}
		}
	}


	return fResult;
}

