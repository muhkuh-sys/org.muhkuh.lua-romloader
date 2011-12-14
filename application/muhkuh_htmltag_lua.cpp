

#include <wx/wx.h>
#include <wx/html/forcelnk.h>
#include <wx/html/m_templ.h>

#include <wx/html/htmlcell.h>

#if USE_LUA!=0
#       include "muhkuh_lua.h"
#endif


FORCE_LINK_ME(muhkuh_htmltag_lua)


TAG_HANDLER_BEGIN(LUA, "LUA")
	TAG_HANDLER_CONSTR(LUA) { }

	TAG_HANDLER_PROC(tag)
	{
		wxString strLuaCode;
		wxString strHtmlCode;
		char *pcResult;


#if USE_LUA!=0
		/* Get the lua script. */
		strLuaCode = m_WParser->GetInnerSource(tag);

		/* Run the lua script. */
		lua_muhkuh_generate_text(NULL, strLuaCode.c_str(), &pcResult);
		if( pcResult!=NULL )
		{
			strHtmlCode = wxString::FromAscii(pcResult);
			free(pcResult);
		}
		else
		{
			strHtmlCode = wxEmptyString;
		}
#else
		strHtmlCode = "ERROR: can not parse lua tag. No lua support built in.";
#endif

		ParseInnerSource(strHtmlCode);
		return true;
	}
TAG_HANDLER_END(LUA)


TAGS_MODULE_BEGIN(Lua)
    TAGS_MODULE_ADD(LUA)
TAGS_MODULE_END(Lua)
