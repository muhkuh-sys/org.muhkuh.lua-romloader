
#include "muhkuh.h"

#include <wx/fs_inet.h>
#include <wx/fs_zip.h>

#include "growbuffer.h"
#include "readFsFile.h"


typedef struct
{
	wxURLError tUrlError;
	const char *pcMessage;
} WXURL_ERROR_TO_MESSAGE_T;

typedef struct
{
	int iCode;
	const char *pcMessage;
} INT_CODE_TO_MESSAGE_T;



static const WXURL_ERROR_TO_MESSAGE_T atUrlErrorCodes[] =
{
	{ wxURL_SNTXERR,        "Syntax error in the URL string." },
	{ wxURL_NOPROTO,        "Found no protocol which can get this URL." },
	{ wxURL_NOHOST,         "A host name is required for this protocol." },
	{ wxURL_NOPATH,         "A path is required for this protocol." },
	{ wxURL_CONNERR,        "Connection error." },
	{ wxURL_PROTOERR,       "An error occurred during negotiation. (should never happen!)" }
};


const INT_CODE_TO_MESSAGE_T atLuaErrorToString[] =
{
	{ 0,                    "" },
	{ LUA_YIELD,            "Thread is suspended" },
	{ LUA_ERRRUN,           "Error while running chunk" },
	{ LUA_ERRSYNTAX,        "Syntax error during pre-compilation" },
	{ LUA_ERRMEM,           "Memory allocation error" },
	{ LUA_ERRERR,           "Generic error or an error occurred while running the error handler" },
	{ LUA_ERRFILE,          "Error occurred while opening file" }
};


const INT_CODE_TO_MESSAGE_T atLuaTypeToString[] =
{
	{ LUA_TNIL,             "nil" },
	{ LUA_TBOOLEAN,         "boolean" },
	{ LUA_TLIGHTUSERDATA,   "light userdata" },
	{ LUA_TNUMBER,          "number" },
	{ LUA_TSTRING,          "string" },
	{ LUA_TTABLE,           "table" },
	{ LUA_TFUNCTION,        "function" },
	{ LUA_TUSERDATA,        "userdata" },
	{ LUA_TTHREAD,          "thread" }
};


// DEBUG
#include <stdio.h>



void muhkuh_internal_init(void)
{
	wxFileSystem::AddHandler(new wxLocalFSHandler);
	wxFileSystem::AddHandler(new wxZipFSHandler);
	wxFileSystem::AddHandler(new wxInternetFSHandler);
}


wxString get_xml_url(lua_State *ptLua_State)
{
	wxString strXmlUrl;
	wxFileName tFileName;


	lua_getglobal(ptLua_State, "__MUHKUH_TEST_XML");
	if( lua_isstring(ptLua_State, -1)==1 )
	{
		strXmlUrl = wxString::FromAscii(lua_tostring(ptLua_State, -1));
	}
	else
	{
		tFileName.AssignCwd();
		strXmlUrl = wxFileSystem::FileNameToURL(tFileName);
	}

	return strXmlUrl;
}


wxString get_base_url(wxString strXmlUrl)
{
	int iLastColon;
	int iLastSlash;
	int iLastValid;


	iLastColon = strXmlUrl.Find(wxT(':'), true);
	iLastSlash = strXmlUrl.Find(wxT('/'), true);
	if( iLastColon==wxNOT_FOUND )
	{
		iLastColon = 0;
	}
	else
	{
		++iLastColon;
	}
	if( iLastSlash==wxNOT_FOUND )
	{
		iLastSlash = 0;
	}
	else
	{
		++iLastSlash;
	}
	iLastValid = (iLastColon>iLastSlash) ? iLastColon : iLastSlash;
	return strXmlUrl.Left(iLastValid);
}


const char *muhkuh_lua_GetUrlErrorString(wxURLError tUrlError)
{
	const char *pcMessage;
	const WXURL_ERROR_TO_MESSAGE_T *ptCnt;
	const WXURL_ERROR_TO_MESSAGE_T *ptEnd;


	/* set the default message */
	pcMessage = _("unknown errorcode");

	ptCnt = atUrlErrorCodes;
	ptEnd = atUrlErrorCodes + sizeof(atUrlErrorCodes)/sizeof(atUrlErrorCodes[0]);
	while(ptCnt<ptEnd)
	{
		if( ptCnt->tUrlError==tUrlError )
		{
			pcMessage = ptCnt->pcMessage;
			break;
		}
		++ptCnt;
	}

	return pcMessage;
}


const char *muhkuh_lua_GetLuaErrorString(int iLuaError)
{
	const char *pcMessage;
	const INT_CODE_TO_MESSAGE_T *ptCnt;
	const INT_CODE_TO_MESSAGE_T *ptEnd;


	/* set the default message */
	pcMessage = _("unknown errorcode");

	ptCnt = atLuaErrorToString;
	ptEnd = atLuaErrorToString + sizeof(atLuaErrorToString)/sizeof(atLuaErrorToString[0]);
	while(ptCnt<ptEnd)
	{
		if( ptCnt->iCode==iLuaError )
		{
			pcMessage = ptCnt->pcMessage;
			break;
		}
		++ptCnt;
	}

	return pcMessage;
}


const char *muhkuh_lua_GetLuaTypeString(int iLuaType)
{
	const char *pcMessage;
	const INT_CODE_TO_MESSAGE_T *ptCnt;
	const INT_CODE_TO_MESSAGE_T *ptEnd;


	/* set the default message */
	pcMessage = _("unknown type");

	ptCnt = atLuaTypeToString;
	ptEnd = atLuaTypeToString + sizeof(atLuaTypeToString)/sizeof(atLuaTypeToString[0]);
	while(ptCnt<ptEnd)
	{
		if( ptCnt->iCode==iLuaType )
		{
			pcMessage = ptCnt->pcMessage;
			break;
		}
		++ptCnt;
	}

	return pcMessage;
}


void muhkuh_lua_GetErrorInfo(lua_State *L, int iStatus, int iTop, wxString *pstrErrorMsg, int *piLineNum)
{
	int iNewTop;
	wxString strErrorMsg;


	if( iStatus!=0 )
	{
		iNewTop = lua_gettop(L);

		strErrorMsg = muhkuh_lua_GetLuaErrorString(iStatus);

		switch(iStatus)
		{
		case LUA_ERRMEM:
		case LUA_ERRERR:
			if( iNewTop>iTop )
			{
				strErrorMsg += wxT("\n");
			}
			break;

		case LUA_ERRRUN:
		case LUA_ERRFILE:
		case LUA_ERRSYNTAX:
		default:
			if( iNewTop>iTop )
			{
				strErrorMsg += wxT("\n") + wxString::FromAscii(lua_tostring(L, -1));
			}
			break;
		}
	}

	strErrorMsg += wxT("\n");

	// Why can't I fill a lua_Debug here? Try to get the line number
	// by parsing the error message that looks like this, 3 is linenumber
	// [string "a = 1("]:3: unexpected symbol near `<eof>'
	wxString strLine = strErrorMsg;
	long lLineNum = -1;
	while( strLine.IsEmpty()==false )
	{
		// search through the str to find ']:LONG:' pattern
		strLine = strLine.AfterFirst(wxT(']'));
		if ((strLine.Length() > 0) && (strLine.GetChar(0) == wxT(':')))
		{
			strLine = strLine.AfterFirst(wxT(':'));
			if (strLine.IsEmpty() || strLine.BeforeFirst(wxT(':')).ToLong(&lLineNum))
			{
				break;
			}
		}
	}

	/* pops the message if any */
	lua_settop(L, iTop);

	if(pstrErrorMsg!=NULL)
	{
		*pstrErrorMsg = strErrorMsg;
	}
	if(piLineNum!=NULL)
	{
		*piLineNum = (int)lLineNum;
	}
}


void load(lua_State *ptLuaState, char *pcUrl, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT)
{
	bool fResult;
	wxString strXmlUrl;
	wxString strBaseUrl;
	wxString strFileName;
	wxString strFileUrl;
	wxURL filelistUrl;
	wxURLError tUrlError;
	growbuffer *ptGrowBuffer;
	wxString strMessage;


	strXmlUrl = get_xml_url(ptLuaState);

	if( strXmlUrl.IsEmpty()==true )
	{
		lua_pushfstring(ptLuaState, "lua load: failed to get global __MUHKUH_TEST_XML");
		fResult = false;
	}
	else
	{
		strBaseUrl = get_base_url(strXmlUrl);

		strFileName = wxString::FromAscii(pcUrl);
		if( strFileName.IsEmpty()==true )
		{
			// the filename parameter is invalid
			lua_pushfstring(ptLuaState, "lua load failed: empty filename");
			fResult = false;
		}
		else
		{
			/* Use the URI class to get rid of backsteps ("..") in the path. */
			wxURI tURI(strBaseUrl + strFileName);
			strFileUrl = tURI.BuildURI();

			tUrlError = filelistUrl.SetURL(strFileUrl);
			if( tUrlError!=wxURL_NOERR )
			{
				// show the error message
				strMessage.Printf(wxT("lua load: invalid URL '%s': %s"), strFileUrl.c_str(), muhkuh_lua_GetUrlErrorString(tUrlError));
				wxLogMessage(strMessage);
				lua_pushfstring(ptLuaState, strMessage.c_str());
				fResult = false;
			}
			else
			{
				ptGrowBuffer = new growbuffer(65536);
				fResult = readFsFile(ptGrowBuffer, strFileUrl);
				if( fResult!=true )
				{
					wxLogMessage(wxT("lua load: failed to read file"));
					lua_pushfstring(ptLuaState, "lua load: failed to read file");
				}
				else
				{
					*psizBUFFER_OUT = ptGrowBuffer->getSize();
					*ppcBUFFER_OUT = (char*)ptGrowBuffer->adoptData();
					fResult = true;
				}
				delete ptGrowBuffer;
			}
		}
	}

	if( fResult!=true )
	{
		wxLogMessage(wxT("Error!"));
		lua_error(ptLuaState);
	}
}


void include(lua_State *ptLuaState, char *pcUrl, char *pcChunkName)
{
	bool fResult;
	wxString strXmlUrl;
	wxString strBaseUrl;
	wxString strFileName;
	wxString strChunkName;
	wxString strErrorMsg;
	wxString strFileUrl;
	wxString strMessage;
	wxURL filelistUrl;
	wxURLError tUrlError;
	wxString strFileContents;
	growbuffer *ptGrowBuffer;
	unsigned char *pucData;
	size_t sizDataSize;
	int iResult;
	int iGetTop;
	int iLineNr;


	/* expect failure */
	fResult = false;

	strXmlUrl = get_xml_url(ptLuaState);
	if( strXmlUrl.IsEmpty()==true )
	{
		lua_pushfstring(ptLuaState, "lua load: failed to get global __MUHKUH_TEST_XML");
	}
	else
	{
		strBaseUrl = get_base_url(strXmlUrl);

		strFileName = wxString::FromAscii(pcUrl);
		strChunkName = wxString::FromAscii(pcChunkName);

		if( strFileName.IsEmpty()==true )
		{
			// the filename parameter is invalid
			lua_pushfstring(ptLuaState, "lua include failed: empty filename");
		}
		else
		{
//			strFileUrl = m_strTestBaseUrl + wxFileName::GetPathSeparator() + strFileName;
			strFileUrl = strBaseUrl + strFileName;
//			wxLogMessage(_("lua include: searching '%s'"), strFileUrl.fn_str());
			tUrlError = filelistUrl.SetURL(strFileUrl);
			if( tUrlError!=wxURL_NOERR )
			{
				strMessage.Printf(wxT("lua include: invalid URL '%s': %s"), strFileUrl.fn_str(), muhkuh_lua_GetUrlErrorString(tUrlError));
				lua_pushfstring(ptLuaState, strMessage.c_str());
			}
			else
			{
				ptGrowBuffer = new growbuffer(65536);
				fResult = readFsFile(ptGrowBuffer, strFileUrl);
				if( fResult!=true )
				{
					lua_pushfstring(ptLuaState, "lua include: failed to read file");
				}
				else
				{
					sizDataSize = ptGrowBuffer->getSize();
					pucData = ptGrowBuffer->getData();

					iGetTop = lua_gettop(ptLuaState);
					iResult = luaL_loadbuffer(ptLuaState, (const char*)pucData, sizDataSize, strFileUrl.ToAscii());
					switch( iResult )
					{
					case 0:
						// ok, the function is on the stack -> execute the new code with no arguments and no return values
//						wxLogMessage(_("lua_include: file loaded, executing code"));
						lua_call(ptLuaState, 0,0);
						fResult = true;
						break;

					case LUA_ERRSYNTAX:
						muhkuh_lua_GetErrorInfo(ptLuaState, iResult, iGetTop, &strErrorMsg, &iLineNr);
						strMessage.Printf(wxT("error %d in line %d: %s"), iResult, iLineNr, strErrorMsg.fn_str());
						lua_pushfstring(ptLuaState, strMessage.c_str());
						break;

					case LUA_ERRMEM:
						lua_pushfstring(ptLuaState, "memory allocation error");
						break;

					default:
						lua_pushfstring(ptLuaState, "Unknown error message from luaL_LoadBuffer: 0x%x", iResult);
						break;
					}
				}
				delete ptGrowBuffer;
			}
		}
	}

	if( fResult!=true )
	{
		lua_error(ptLuaState);
	}
}
