

#include <wx/wx.h>
#include <wx/html/forcelnk.h>
#include <wx/html/m_templ.h>

#include <wx/html/htmlcell.h>

#include "muhkuh_lua.h"


FORCE_LINK_ME(muhkuh_htmltag_lua)


TAG_HANDLER_BEGIN(LUA, "LUA")
	TAG_HANDLER_CONSTR(LUA) { }

	TAG_HANDLER_PROC(tag)
	{
		wxString strLuaCode;
		wxString strHtmlCode;
		char *pcResult;


		/* Get the lua script. */
		strLuaCode = m_WParser->GetInnerSource(tag);

		/* Run the lua script. */
		lua_muhkuh_execute_html_tag(NULL, strLuaCode.fn_str(), &pcResult);
		if( pcResult!=NULL )
		{
			strHtmlCode = wxString::FromAscii(pcResult);
			free(pcResult);
		}
		else
		{
			strHtmlCode = wxEmptyString;
		}

		ParseInnerSource(strHtmlCode);
		return true;
	}
TAG_HANDLER_END(LUA)


TAGS_MODULE_BEGIN(Lua)
    TAGS_MODULE_ADD(LUA)
TAGS_MODULE_END(Lua)
