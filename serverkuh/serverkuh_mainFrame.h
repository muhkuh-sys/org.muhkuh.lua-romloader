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


#include <vector>

#include <wx/wx.h>

#include <wx/aui/aui.h>
#include <wx/cmdline.h>
#include <wx/bmpcbox.h>
#include <wx/dir.h>
#include <wx/html/helpctrl.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>
#include <wx/tipdlg.h>


#ifndef __SERVERKUH_MAINFRAME_H__
#define __SERVERKUH_MAINFRAME_H__

#include "application/muhkuh_plugin_manager.h"
#include "application/muhkuh_testTreeItemData.h"
#include "application/muhkuh_wrap_xml.h"


//-------------------------------------
// the wxLua entries

#include <wxlua/include/wxlua.h>
#include <wxluasocket/include/wxldserv.h>
#include <wxluasocket/include/wxldtarg.h>
#include "plugins/_luaif/muhkuh_wxlua_bindings.h"
#include "plugins/_luaif/bit_wxlua_bindings.h"

extern "C"
{
	#include <lualib.h>
}


//-------------------------------------
// Define the main frame

class serverkuh_mainFrame: public wxFrame
{
public:
	serverkuh_mainFrame(wxCmdLineParser *ptParser);
	~serverkuh_mainFrame(void);

	void OnIdle(wxIdleEvent &event);

	void OnQuit(wxCommandEvent &event);
	void OnTestExecute(wxCommandEvent &event);

	void OnMove(wxMoveEvent &event);
	void OnSize(wxSizeEvent &event);

	// process the lua events
	void OnLuaDebug(wxLuaEvent &event);
	void OnLuaPrint(wxLuaEvent &event);
	void OnLuaError(wxLuaEvent &event);

	void OnDebugSocket(wxSocketEvent &event);

	// the lua functions
	void luaTestHasFinished(void);
	wxString luaLoad(wxString strFileName);
	void luaInclude(wxString strFileName);
	void luaSetLogMarker(void);
	wxString luaGetMarkedLog(void);
	void luaScanPlugins(wxString strPattern);
	muhkuh_plugin_instance *luaGetNextPlugin(void);
	muhkuh_wrap_xml *luaGetSelectedTest(void);

private:
	void createControls(void);
	void initLogging(wxCmdLineParser *ptParser);

	bool scanFileXml(wxString &strXmlUrl);

	void init_lua(void);

	void read_config(void);
	void write_config(void);

	bool initLuaState(void);
	void clearLuaState(void);

	void executeTest(void);
	void finishTest(void);

	bool check_plugins(void);

	void dbg_write_u08(unsigned char ucData);
	void dbg_write_int(int iData);
	void dbg_write_achar(const char *pcData);

	void dbg_get_step_command(void);
	bool dbg_get_command(void);


	// main frame init state
	typedef enum
	{
		MAINFRAME_INIT_STATE_UNCONFIGURED,
		MAINFRAME_INIT_STATE_CONFIGURED,
		MAINFRAME_INIT_STATE_RUNNING
	} MAINFRAME_INIT_STATE_E;

	typedef enum
	{
		DBGMODE_Run			= 0,
		DBGMODE_StepInto		= 1,
		DBGMODE_StepOver		= 2,
		DBGMODE_StepOut			= 3
	} tDBGMODE;

	// state of the init process
	MAINFRAME_INIT_STATE_E m_eInitState;

	// the commandline parser
	wxCmdLineParser *m_ptParser;

	// the version string
	wxString m_strVersion;

	// main frame controls
	wxAuiManager m_auiMgr;
	// the default perspective
	wxString m_strDefaultPerspective;
	// the application icons in different sizes
	wxIconBundle m_frameIcons;

	// the debug server name
	wxString m_strDebugServerName;
	// the debug server port
	long m_lDebugServerPort;
	// the debug client socket
	wxSocketClient *m_ptDebugClientSocket;
	// the debug mode
	tDBGMODE m_dbg_mode;
	// debug frame count
	unsigned int m_uiDbgFrameLevel;
	// the lua state
	wxLuaState *m_ptLuaState;
	// the test panel
	wxPanel *m_testPanel;

	// the plugin manager
	muhkuh_plugin_manager *m_ptPluginManager;

	// xml description of the running test
	muhkuh_wrap_xml *m_ptWrapXml;
	// index of the selected subtest
	long m_lTestIndex;
	// name of the running test
	wxString m_strTestName;
	// name of the helpfile for the running test
	wxString m_strHelpFile;
	// url to the xml file
	wxString m_strTestXmlUrl;
	// base directory of the test
	wxString m_strTestBaseUrl;

	// path settings
	wxString m_strApplicationPath;

	// the lua include path
	wxString m_strLuaIncludePath;
	// lua startup code
	wxString m_strLuaStartupCode;

	// the application title
	wxString m_strApplicationTitle;
	wxString m_strApplicationIcon;

	// log target for all plugins
	wxLog *m_pLogTarget;
	// the log marker for the scripts
	wxTextPos m_tLogMarker;
	// the text control
	wxTextCtrl *m_textCtrl;

	// the locale object
	wxLocale m_locale;

	// frame size and position
	wxPoint m_framePosition;
	wxSize m_frameSize;

    DECLARE_EVENT_TABLE()
};


extern serverkuh_mainFrame *g_ptMainFrame;


#endif	// __SERVERKUH_MAINFRAME_H__

