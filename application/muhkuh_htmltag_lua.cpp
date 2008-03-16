

#include <wx/wx.h>
#include <wx/html/forcelnk.h>
#include <wx/html/m_templ.h>

#include <wx/html/htmlcell.h>

#include "muhkuh_lua_interface.h"
#include "muhkuh_mainFrame.h"


FORCE_LINK_ME(muhkuh_htmltag_lua)


TAG_HANDLER_BEGIN(LUA, "LUA")
	TAG_HANDLER_CONSTR(LUA) { }

	TAG_HANDLER_PROC(tag)
	{
		wxString strLuaCode;
		wxString strHtmlCode;


		strLuaCode = m_WParser->GetInnerSource(tag);
		strHtmlCode = muhkuh_mainFrame::htmlTag_lua(strLuaCode);

		ParseInnerSource(strHtmlCode);
		return true;
	}

TAG_HANDLER_END(LUA)


TAGS_MODULE_BEGIN(Lua)

    TAGS_MODULE_ADD(LUA)

TAGS_MODULE_END(Lua)
