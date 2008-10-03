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


#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/url.h>

#include "application/growbuffer.h"
#include "application/muhkuh_version.h"
#include "application/muhkuh_lua_interface.h"
#include "application/readFsFile.h"
#include "application/muhkuh_brokenPluginDialog.h"
#include "application/muhkuh_debug_messages.h"

#include "serverkuh_icons.h"
#include "serverkuh_id.h"
#include "serverkuh_version.h"
#include "serverkuh_mainFrame.h"

//-------------------------------------
// the wxLua entries

#include <wxlua/include/wxlstate.h>
#include <wxbind/include/wxbinddefs.h>

WXIMPORT int wxluatype_wxXmlDocument;
WXIMPORT int wxluatype_wxPanel;

WXLUA_DECLARE_BIND_WXLUA
WXLUA_DECLARE_BIND_WXBASE
WXLUA_DECLARE_BIND_WXCORE
WXLUA_DECLARE_BIND_WXADV
WXLUA_DECLARE_BIND_WXNET
WXLUA_DECLARE_BIND_WXXML
WXLUA_DECLARE_BIND_WXXRC
WXLUA_DECLARE_BIND_WXHTML
WXLUA_DECLARE_BIND_WXAUI
WXLUA_DECLARE_BIND_WXSTC

serverkuh_mainFrame *g_ptMainFrame;

//-------------------------------------

BEGIN_EVENT_TABLE(serverkuh_mainFrame, wxFrame)
	EVT_IDLE(serverkuh_mainFrame::OnIdle)

	EVT_MENU(wxID_EXIT,						serverkuh_mainFrame::OnQuit)

	EVT_LUA_DEBUG_HOOK(wxID_ANY,					serverkuh_mainFrame::OnLuaDebug)
	EVT_LUA_PRINT(wxID_ANY,						serverkuh_mainFrame::OnLuaPrint)
	EVT_LUA_ERROR(wxID_ANY,						serverkuh_mainFrame::OnLuaError)

	EVT_MOVE(serverkuh_mainFrame::OnMove)
	EVT_SIZE(serverkuh_mainFrame::OnSize)

	EVT_SOCKET(muhkuh_debugClientSocket_event,			serverkuh_mainFrame::OnDebugSocket)
END_EVENT_TABLE()

serverkuh_mainFrame::serverkuh_mainFrame(wxCmdLineParser *ptParser)
 : wxFrame(NULL, wxID_ANY, wxEmptyString)
 , m_ptParser(ptParser)
 , m_ptLuaState(NULL)
 , m_ptPluginManager(NULL)
 , m_ptWrapXml(NULL)
 , m_eInitState(MAINFRAME_INIT_STATE_UNCONFIGURED)
 , m_ptDebugClientSocket(NULL)
{
	wxString strConfigFileName;
	wxFileName cfgName;
	wxFileConfig *ptConfig;
	int iLanguage;
	int iLastColon;
	int iLastSlash;
	int iLastValid;
	bool fOk;
	int iColonPos;
	wxString strDebugServerAndPort;
	wxString strMsg;


	// get the application path
	cfgName.Assign(wxStandardPaths::Get().GetExecutablePath());
	m_strApplicationPath = cfgName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	// get the config
	if( m_ptParser->Found(wxT("c"), &strConfigFileName)!=true )
	{
		strConfigFileName = wxTheApp->GetAppName()+wxT(".cfg");
	}
	cfgName.SetFullName(strConfigFileName);
	strConfigFileName = cfgName.GetFullPath();
	ptConfig = new wxFileConfig(wxTheApp->GetAppName(), wxTheApp->GetVendorName(), strConfigFileName, strConfigFileName, wxCONFIG_USE_LOCAL_FILE);
	wxConfigBase::Set(ptConfig);
	ptConfig->SetRecordDefaults();

	// TODO: init the locale to a value from the config file
//	iLanguage = wxLANGUAGE_GERMAN;
//	iLanguage = wxLANGUAGE_ENGLISH_US;
	iLanguage = wxLocale::GetSystemLanguage();
//	wxLogMessage(_("Using language '%s'."), wxLocale::GetLanguageName(iLanguage).fn_str());
	if( m_locale.Init(iLanguage, wxLOCALE_CONV_ENCODING)==false )
	{
		// NOTE: translate this so the default system language may be used at least.
		wxLogError(_("The language '%s' is not supported by the system."), wxLocale::GetLanguageName(iLanguage).fn_str());
	}
	else
	{
		// search locales in the 'locale' directory
		// TODO: use some systempath for this
		wxLocale::AddCatalogLookupPathPrefix(wxT("locale"));
		m_locale.AddCatalog(wxTheApp->GetAppName());
	}

	// create the plugin manager
	m_ptPluginManager = new muhkuh_plugin_manager();

	// build version string
	m_strVersion.Printf(wxT(SERVERKUH_APPLICATION_NAME) wxT(" v%d.%d.%d"), SERVERKUH_VERSION_MAJ, SERVERKUH_VERSION_MIN, SERVERKUH_VERSION_SUB);

	// use aui manager for this frame
	m_auiMgr.SetManagedWindow(this);

	// create a status bar with text on the left side and a gauge on the right
	CreateStatusBar(2);
	// use pane 1 for menu and toolbar help
	SetStatusBarPane(0);

	// create the controls
	createControls();

	// init the log target and the log level
	initLogging(ptParser);

	wxLogMessage(_("application started"));

	// read the config
	read_config();

	// set the window title
	if( m_strApplicationTitle.IsEmpty()==true )
	{
		// no custom title -> use the version string
		SetTitle(m_strVersion);
	}
	else
	{
		// show the custom title
		SetTitle(m_strApplicationTitle);
	}

	// set the application icon
	if( m_strApplicationIcon.IsEmpty()==true )
	{
		// set the muhkuh icon
		m_frameIcons.AddIcon(muhkuh_016_xpm);
		m_frameIcons.AddIcon(muhkuh_032_xpm);
		m_frameIcons.AddIcon(muhkuh_064_xpm);
		m_frameIcons.AddIcon(muhkuh_128_xpm);
	}
	else
	{
		// load a custom icon
		m_frameIcons.AddIcon(m_strApplicationIcon, wxBITMAP_TYPE_ICO);
	}
	SetIcons(m_frameIcons);

	InitDialog();

	g_ptMainFrame = this;

	// get the debug server and port
	m_strDebugServerName = wxEmptyString;
	m_lDebugServerPort = -1;
	if( ptParser->Found(wxT("d"), &strDebugServerAndPort)==true )
	{
		iColonPos = strDebugServerAndPort.Find(wxT(':'), false);
		if( iColonPos!=wxNOT_FOUND && iColonPos==strDebugServerAndPort.Find(wxT(':'), true) &&  strDebugServerAndPort.AfterFirst(wxT(':')).ToLong(&m_lDebugServerPort)==true )
		{
			m_strDebugServerName = strDebugServerAndPort.BeforeFirst(wxT(':'));
		}
		else
		{
			wxMessageBox(_("Failed to parse debug server and port from the comand line! The connection to the debugger will not be esablished."), _("Command line error"), wxICON_ERROR, this);
		}
	}

	// get the test index
	if( ptParser->Found(wxT("i"), &m_lTestIndex)!=true )
	{
		m_lTestIndex = 0;
	}

	// get the base url of the test
	m_strTestXmlUrl = ptParser->GetParam(0);
	iLastColon = m_strTestXmlUrl.Find(wxT(':'), true);
	iLastSlash = m_strTestXmlUrl.Find(wxT('/'), true);
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
	m_strTestBaseUrl = m_strTestXmlUrl.Left(iLastValid);
	wxLogMessage(wxT("base url:") + m_strTestBaseUrl);

	m_eInitState = MAINFRAME_INIT_STATE_CONFIGURED;
}


serverkuh_mainFrame::~serverkuh_mainFrame(void)
{
	write_config();

	finishTest();
	clearLuaState();

	// delete the plugin manager
	if( m_ptPluginManager!=NULL )
	{
		delete m_ptPluginManager;
	}

	m_auiMgr.UnInit();
}


void serverkuh_mainFrame::createControls(void)
{
	wxAuiPaneInfo paneInfo;
	long style;
	wxSize tSize;
	int iSize;
	wxListItem listItem;


	// TODO: is this really necessary?
	SetMinSize(wxSize(400,300));

	// create the test panel
	m_testPanel = new wxPanel(this);
	paneInfo.Name(wxT("testpanel")).CaptionVisible(false).Center().Position(0);
	m_auiMgr.AddPane(m_testPanel, paneInfo);

	style = wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY;
	m_textCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, style);
	paneInfo.Name(wxT("message_log")).CaptionVisible(true).Caption(_("Message Log")).Bottom().Position(0);
	m_auiMgr.AddPane(m_textCtrl, paneInfo);

	m_auiMgr.Update();

	// save this layout as the default perspective
	m_strDefaultPerspective = m_auiMgr.SavePerspective();
}


void serverkuh_mainFrame::initLogging(wxCmdLineParser *ptParser)
{
	wxLog *pOldLogTarget;
	long lLogLevel;
	wxLogLevel tLogLevel;
	bool fBeVerbose;


	// set new log target
	m_pLogTarget = new wxLogTextCtrl(m_textCtrl);
	pOldLogTarget = wxLog::SetActiveTarget(m_pLogTarget);
	if( pOldLogTarget!=NULL )
	{
		delete pOldLogTarget;
	}

	// get verbose level
	if( ptParser->Found(wxT("v"), &lLogLevel)!=true )
	{
		// default log level is 'message'
		lLogLevel = 2;
	}
	// limit log level to valid range
	if( lLogLevel<0 )
	{
		lLogLevel = 0;
	}
	else if( lLogLevel>6 )
	{
		lLogLevel = 6;
	}
	// get the settings for the log level
	switch(lLogLevel)
	{
	case 0:
		tLogLevel = wxLOG_Error;
		fBeVerbose = false;
		break;
	case 1:
		tLogLevel = wxLOG_Warning;
		fBeVerbose = false;
		break;
	case 2:
		tLogLevel = wxLOG_Message;
		fBeVerbose = false;
		break;
	case 3:
		tLogLevel = wxLOG_Status;
		fBeVerbose = false;
		break;
	case 4:
		// NOTE: that's the same as wxLogVerbose
		tLogLevel = wxLOG_Info;
		fBeVerbose = true;
		break;
	case 5:
		tLogLevel = wxLOG_Debug;
		fBeVerbose = true;
		break;
	case 6:
		tLogLevel = wxLOG_Trace;
		fBeVerbose = true;
		break;
	}
	wxLog::SetLogLevel(tLogLevel);
	wxLog::SetVerbose(fBeVerbose);
}


void serverkuh_mainFrame::read_config(void)
{
	wxConfigBase *pConfig;
	int iMainFrameX, iMainFrameY, iMainFrameW, iMainFrameH;
	bool fWinMaximized;
	wxString strPerspective;


	// get the config
	pConfig = wxConfigBase::Get();
	if( pConfig==NULL )
	{
		return;
	}

	// get frame position and size
	pConfig->SetPath(wxT("/ServerFrame"));
	m_framePosition.x = pConfig->Read(wxT("x"), 50);
	m_framePosition.y = pConfig->Read(wxT("y"), 50);
	m_frameSize.SetWidth( pConfig->Read(wxT("w"), 640) );
	m_frameSize.SetHeight( pConfig->Read(wxT("h"), 480) );
	pConfig->Read(wxT("maximized"), &fWinMaximized, false);
	strPerspective = pConfig->Read(wxT("perspective"), wxEmptyString);
	m_strApplicationTitle = pConfig->Read(wxT("customtitle"), wxEmptyString);
	m_strApplicationIcon = pConfig->Read(wxT("customicon"), wxEmptyString);

	// get lua settings
	pConfig->SetPath(wxT("/Lua"));
	m_strLuaIncludePath = pConfig->Read(wxT("includepaths"), wxT("lua/?.lua"));
	m_strLuaStartupCode = pConfig->Read(wxT("startupcode"), wxT("require(\"muhkuh_system\")\nmuhkuh_system.boot_xml()\n"));

	// get all plugins
	m_ptPluginManager->read_config(pConfig);

	// set window properties
	SetSize(m_framePosition.x, m_framePosition.y, m_frameSize.GetWidth(), m_frameSize.GetHeight());
	// set fullscreen mode
	Maximize(fWinMaximized);
	if( strPerspective.IsEmpty()==false )
	{
		m_auiMgr.LoadPerspective(strPerspective,true);
	}
}


void serverkuh_mainFrame::write_config(void)
{
	wxConfigBase *pConfig;
	bool fWinMaximized;
	wxString strPerspective;


	pConfig = wxConfigBase::Get();
	if( pConfig == NULL )
	{
		return;
	}

	fWinMaximized = IsMaximized();

	// save the frame position
	strPerspective = m_auiMgr.SavePerspective();

	pConfig->SetPath(wxT("/ServerFrame"));
	pConfig->Write(wxT("x"),		(long)m_framePosition.x);
	pConfig->Write(wxT("y"),		(long)m_framePosition.y);
	pConfig->Write(wxT("w"),		(long)m_frameSize.GetWidth());
	pConfig->Write(wxT("h"),		(long)m_frameSize.GetHeight());
	pConfig->Write(wxT("maximized"),	fWinMaximized);
	pConfig->Write(wxT("perspective"),	strPerspective);
	pConfig->Write(wxT("customtitle"),	m_strApplicationTitle);
	pConfig->Write(wxT("customicon"),	m_strApplicationIcon);

	// save all plugins
	m_ptPluginManager->write_config(pConfig);
}


bool serverkuh_mainFrame::scanFileXml(wxString &strXmlUrl)
{
	wxFileSystem fileSystem;
	wxFSFile *ptFsFile;
	wxInputStream *ptInputStream;
	wxString strXmlData;
	muhkuh_wrap_xml *ptWrapXml;
	testTreeItemData *ptItemData;
	wxString strTestName;
	wxString strHelpFile;
	wxString strMsg;
	bool fResult;


	fResult = false;

	// test if file exists
	wxLogVerbose(_("Reading testdescription '%s'"), strXmlUrl.fn_str());
	ptFsFile = fileSystem.OpenFile(strXmlUrl);
	if( ptFsFile==NULL )
	{
		wxLogError(_("could not read xml file '%s'"), strXmlUrl.fn_str());
	}
	else
	{
		// ok, file exists. read all data into a string
		ptInputStream = ptFsFile->GetStream();
		// stream read, construct xml tree
		ptWrapXml = new muhkuh_wrap_xml();
		if( ptWrapXml!=NULL )
		{
			if( ptWrapXml->initialize(ptInputStream, 0, 0)==false )
			{
				wxLogError(_("Failed to read the xml file for testdescription '%s'"), strXmlUrl.fn_str());
			}
			else
			{
				// get the main test name
				m_strTestName = ptWrapXml->testDescription_getName();
				// TODO: get the help file name
				m_strHelpFile.Empty();
				// set the xml object
				m_ptWrapXml = ptWrapXml;
				// ok!
				fResult = true;
			}
		}
		delete ptFsFile;
	}

	return fResult;
}


bool serverkuh_mainFrame::dbg_enable(void)
{
	bool fResult;
	wxIPV4address tIpAdr;


	// expect failure
	fResult = false;

	m_ptDebugClientSocket = new wxSocketClient(wxSOCKET_WAITALL);
	m_ptDebugClientSocket->SetEventHandler(*this, muhkuh_debugClientSocket_event);
	m_ptDebugClientSocket->SetNotify(wxSOCKET_CONNECTION_FLAG|wxSOCKET_INPUT_FLAG|wxSOCKET_LOST_FLAG);
	m_ptDebugClientSocket->Notify(true);

	if( tIpAdr.Hostname(m_strDebugServerName)==false )
	{
		wxLogError(_("failed to set debug server's hostname!"));
	}
	else if( tIpAdr.Service(m_lDebugServerPort)==false )
	{
		wxLogError(_("failed to set debug server's port!"));
	}
	else
	{
		m_ptDebugClientSocket->Connect(tIpAdr, false);
		m_ptDebugClientSocket->WaitOnConnect(100);

		if( m_ptDebugClientSocket->IsConnected()!=true )
		{
			m_ptDebugClientSocket->Close();
			wxLogMessage(_("failed to connect to the debug server!"));
		}
		else
		{
			wxLogMessage(_("connected to the debug server!"));

			m_ptLuaState->SetLuaDebugHook(LUA_MASKCALL|LUA_MASKRET|LUA_MASKLINE, 0, 0, true);

			m_dbg_mode = DBGMODE_StepInto;

			fResult = true;
		}
	}

	return fResult;
}


bool serverkuh_mainFrame::initLuaState(void)
{
	bool fResult;
	wxLuaBindingList *ptBindings;


	// expect success
	fResult = true;

	clearLuaState();

	// create a new lua state
	m_ptLuaState = new wxLuaState(false);
	if( m_ptLuaState==NULL )
	{
		wxLogError(_("Failed to allocate a new lua state"));
		fResult = false;
	}
	else
	{
		// init the standard lua bindings
		WXLUA_IMPLEMENT_BIND_WXLUA
		WXLUA_IMPLEMENT_BIND_WXBASE
		WXLUA_IMPLEMENT_BIND_WXCORE
		WXLUA_IMPLEMENT_BIND_WXADV
		WXLUA_IMPLEMENT_BIND_WXNET
		WXLUA_IMPLEMENT_BIND_WXXML
		WXLUA_IMPLEMENT_BIND_WXXRC
		WXLUA_IMPLEMENT_BIND_WXHTML
		WXLUA_IMPLEMENT_BIND_WXAUI
		WXLUA_IMPLEMENT_BIND_WXSTC

		// init the muhkuh lua bindings
		fResult = wxLuaBinding_serverkuh_lua_init();
		if( fResult!=true )
		{
			// failed to init the muhkuh lua bindings
			wxLogError(_("Failed to init the muhkuh_lua bindings"));
		}
		else
		{
			// init the muhkuh bit lua bindings
			fResult = wxLuaBinding_bit_lua_init();
			if( fResult!=true )
			{
				// failed to init the muhkuh bit lua bindings
				wxLogError(_("Failed to init the muhkuh_bit_lua bindings"));
			}
			else
			{
				// init the muhkuh mhash lua bindings
				fResult = wxLuaBinding_mhash_lua_init();
				if( fResult!=true )
				{
					// failed to init the muhkuh mhash lua bindings
					wxLogError(_("Failed to init the muhkuh_mhash_lua bindings"));
				}
				else
				{
					// init the lua bindings for all plugins
					fResult = m_ptPluginManager->initLuaBindings(m_ptLuaState);
					if( fResult!=true )
					{
						wxLogError(_("Failed to init plugin bindings"));
					}
					else
					{
						// create the lua state
						fResult = m_ptLuaState->Create(this, wxID_ANY);
						if( fResult!=true )
						{
							wxLogError(_("Failed to create a new lua state"));
						}
						else
						{
							// is the state valid?
							fResult = m_ptLuaState->Ok();
							if( fResult!=true )
							{
								wxLogError(_("Strange lua state"));
							}
							else
							{
								// set the package path
								wxLogMessage(wxT("Lua path:") + m_strLuaIncludePath);
	
								m_ptLuaState->lua_GetGlobal(wxT("package"));
								if( m_ptLuaState->lua_IsNoneOrNil(-1)==true )
								{
									wxLogError(_("Failed to get the global 'package'"));
								}
								m_ptLuaState->lua_PushString(m_strLuaIncludePath);
								m_ptLuaState->lua_SetField(-2, wxT("path"));
	
								// set the lua version
								m_ptLuaState->lua_PushString(m_strVersion.ToAscii());
								m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_VERSION"));
	
								// only create debug hooks and connection if debug server is set
								if( m_strDebugServerName.IsEmpty()==false )
								{
									dbg_enable();
								}
							}
						}
					}
				}
			}
		}
	}

	return fResult;
}


void serverkuh_mainFrame::clearLuaState(void)
{
	wxLuaState *ptLuaState;
	wxLuaBindingList *ptBindings;


	// clear any plugin scans
	m_ptPluginManager->ClearAllMatches();

	// NOTE: dependencies must be cleared the here, they might come from
	// plugins which are already gone after a config change
	ptBindings = wxLuaBinding::GetBindingList();
	ptBindings->Clear();

	// disconnect from any debug server
	if( m_ptDebugClientSocket!=NULL )
	{
		m_ptDebugClientSocket->Destroy();
		m_ptDebugClientSocket = NULL;
	}

	if( m_ptLuaState!=NULL )
	{
		ptLuaState = m_ptLuaState;
		m_ptLuaState = NULL;

		if( ptLuaState->Ok()==true )
		{
			ptLuaState->CloseLuaState(true);
			ptLuaState->Destroy();
		}
		delete ptLuaState;
	}

	wxSafeYield();
}


bool serverkuh_mainFrame::check_plugins(void)
{
	std::vector<unsigned long> v_plugins;
	std::vector<unsigned long>::const_iterator v_iter;
	muhkuh_brokenPluginDialog *ptBrokenPluginDlg;
	unsigned long ulCnt;
	unsigned long ulMax;
	bool fPluginOk;
	int iResult;
	bool fContinueOperation;


	// default is to continue a pending operation
	fContinueOperation = true;

	if( m_ptPluginManager!=NULL )
	{
		// loop over all plugins
		ulCnt = 0;
		ulMax = m_ptPluginManager->getPluginCount();
		while( ulCnt<ulMax )
		{
			if( m_ptPluginManager->GetEnable(ulCnt) )
			{
				// get the plugin status
				fPluginOk = m_ptPluginManager->IsOk(ulCnt);
				if( fPluginOk!=true )
				{
					v_plugins.push_back(ulCnt);
				}
			}

			// next plugin
			++ulCnt;
		}
	}

	// found any broken plugins?
	if( v_plugins.size()!=0 )
	{
		// yes -> prompt the user what to do
		ptBrokenPluginDlg = new muhkuh_brokenPluginDialog(this, &v_plugins, m_ptPluginManager);
		iResult = ptBrokenPluginDlg->ShowModal();
		if( iResult==muhkuh_brokenPluginDialog_ButtonConfig )
		{
			// show config menu
			wxCommandEvent exec_event(wxEVT_COMMAND_MENU_SELECTED, wxID_PREFERENCES);
			wxPostEvent( this, exec_event );

			// the config menu is shown, don't continue an operation like "start test"
			fContinueOperation = false;
		}
	}

	return fContinueOperation;
}


void serverkuh_mainFrame::executeTest(void)
{
	bool fResult;
	int iResult;
	wxString strMsg;
	wxString strErrorMsg;
	wxString strDebug;
	int iGetTop;
	int iLineNr;


	// check all plugins for state ok before executing the test
	fResult = check_plugins();
	if( fResult==true )
	{
		wxLogMessage(wxT("execute test '") + m_strTestName + wxT("', index %ld"), m_lTestIndex);

		if( m_ptLuaState!=NULL && m_ptLuaState->Ok()==true )
		{
			// set some global vars

			// set the xml document
			m_ptLuaState->wxluaT_PushUserDataType(m_ptWrapXml->getXmlDocument(), wxluatype_wxXmlDocument, false);
			m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_TEST_XML"));
			// set the selected test index
			m_ptLuaState->lua_PushNumber(m_lTestIndex);
			m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_TEST_INDEX"));
			// set the test panel
			m_ptLuaState->wxluaT_PushUserDataType(m_testPanel, wxluatype_wxPanel, false);
			m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_PANEL"));

			// set the log marker
			luaSetLogMarker();

			iGetTop = m_ptLuaState->lua_GetTop();
			iResult = m_ptLuaState->RunString(m_strLuaStartupCode, m_strLuaStartupCode);
			if( iResult!=0 )
			{
				wxlua_errorinfo(m_ptLuaState->GetLuaState(), iResult, iGetTop, &strErrorMsg, &iLineNr);
				strMsg.Printf(_("error %d in line %d: ") + strErrorMsg, iResult, iLineNr);
				wxLogError(strMsg);
				finishTest();
			}
		}
	}
}


void serverkuh_mainFrame::finishTest(void)
{
	wxLogMessage(_("Test '%s' finished, cleaning up..."), m_strTestName.fn_str());
}


void serverkuh_mainFrame::OnIdle(wxIdleEvent &event)
{
	wxString strStatus;
	wxString strMemStatus;
	int iLuaMemKb;
	bool fOk;


	if( m_eInitState==MAINFRAME_INIT_STATE_CONFIGURED )
	{
		// NOTE: this must be the first statement in this case, or it will be executed with every idle event
		m_eInitState = MAINFRAME_INIT_STATE_RUNNING;

//		// TODO: add all help books
//		m_ptHelp->AddBook(wxFileName("muhkuh.htb"), true);

		fOk = scanFileXml(m_strTestXmlUrl);
		if( fOk!=true )
		{
			wxLogError(_("failed to read test description!"));
		}
		else
		{
			fOk = initLuaState();
			if( fOk!=true )
			{
				wxLogError(_("failed to init the lua state"));
			}
			else
			{
				executeTest();
			}
		}
	}

	strStatus.Printf(_("Test '%s' in progress..."), m_strTestName.fn_str());

	// get the Lua Memory in kilobytes
	if( m_ptLuaState!=NULL && m_ptLuaState->Ok()==true )
	{
		iLuaMemKb = m_ptLuaState->lua_GetGCCount();
		strMemStatus.Printf(_("Lua uses %d kilobytes"), iLuaMemKb);
		strStatus += strMemStatus;
	}

	// set the status text
	SetStatusText(strStatus, 1);

	event.Skip();
}


void serverkuh_mainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}


void serverkuh_mainFrame::OnLuaDebug(wxLuaEvent &event)
{
	bool fPauseExec;


	// assume to continue
	fPauseExec = false;

	switch( event.m_lua_Debug->event )
	{
	case LUA_HOOKCALL:
		// count frames if in step over mode
		if( m_dbg_mode==DBGMODE_StepOver || m_dbg_mode==DBGMODE_StepOut )
		{
			++m_uiDbgFrameLevel;
		}
		break;

	case LUA_HOOKRET:
	case LUA_HOOKTAILRET:
		// dec frames in step over mode
		if( m_dbg_mode==DBGMODE_StepOver || m_dbg_mode==DBGMODE_StepOut )
		{
			if( m_uiDbgFrameLevel>0 )
			{
				--m_uiDbgFrameLevel;
			}
		}
		break;

	case LUA_HOOKLINE:
		switch( m_dbg_mode )
		{
		case DBGMODE_Run:
			// do not stop
			break;

		case DBGMODE_StepInto:
			// always stop
			fPauseExec = true;
			break;

		case DBGMODE_StepOver:
		case DBGMODE_StepOut:
			// stop if execution if the requested level is reached
			if( m_uiDbgFrameLevel==0 )
			{
				fPauseExec = true;
			}
			break;
		}
	}


	if( fPauseExec==true )
	{
		// write the command
		dbg_write_u08(MUHDBG_InterpreterHalted);
		dbg_get_stack(0);
		dbg_get_step_command();
	}
}


void serverkuh_mainFrame::dbg_get_step_command(void)
{
	bool fContinueExecution;


	// wait until a step, run or exit command was received
	do
	{
		fContinueExecution = dbg_get_command();
	} while( fContinueExecution==false );
}


bool serverkuh_mainFrame::dbg_get_command(void)
{
	bool fContinueExecution;
	unsigned char ucPacketTyp;
	bool fOk;
	int iLevel;
	int iIndex;


	fContinueExecution = false;

	// wait for a command
	m_ptDebugClientSocket->Read(&ucPacketTyp, 1);
	if( m_ptDebugClientSocket->LastCount()==1 )
	{
		switch( ucPacketTyp )
		{
		case MUHDBG_CmdStepInto:
			m_dbg_mode = DBGMODE_StepInto;
			fContinueExecution = true;
			break;

		case MUHDBG_CmdStepOver:
			// new mode is 'step over the next line'
			m_dbg_mode = DBGMODE_StepOver;
			// init frame level to 0 (stay on this level)
			m_uiDbgFrameLevel = 0;
			fContinueExecution = true;
			break;

		case MUHDBG_CmdStepOut:
			m_dbg_mode = DBGMODE_StepOut;
			// init frame level to 1 (go one level up)
			m_uiDbgFrameLevel = 1;
			fContinueExecution = true;
			break;

		case MUHDBG_CmdContinue:
			m_dbg_mode = DBGMODE_Run;
			fContinueExecution = true;
			break;

		case MUHDBG_CmdBreak:
			m_dbg_mode = DBGMODE_StepInto;
			break;

		case MUHDBG_CmdGetStack:
			fOk = dbg_read_int(&iLevel);
			if( fOk==true )
			{
				dbg_get_stack(iLevel);
			}
			else
			{
				wxLogError(_("failed to receive int parameter for GetFrame command!"));
			}
			break;

		case MUHDBG_CmdGetLocal:
			fOk = dbg_read_int(&iLevel);
			if( fOk==true )
			{
				dbg_get_locals(iLevel);
			}
			else
			{
				wxLogError(_("failed to receive int parameter for GetLocal command!"));
			}
			break;

		case MUHDBG_CmdGetUpValue:
			fOk = dbg_read_int(&iLevel);
			if( fOk==true )
			{
				fOk = dbg_read_int(&iIndex);
				if( fOk==true )
				{
					dbg_get_upvalue(iLevel, iIndex);
				}
			}
			else
			{
				wxLogError(_("failed to receive int parameter for GetUpValue command!"));
			}
			break;

		default:
			wxLogError(_("received strange packet: 0x%02x"), ucPacketTyp);
			break;
		}
	}

	return fContinueExecution;
}


void serverkuh_mainFrame::dbg_get_stack(int iLevel)
{
	int iResult;
	unsigned char ucStatus;
	lua_Debug tDbg = {0};


	if( m_ptDebugClientSocket->IsConnected()==true )
	{
		ucStatus = 1;
		iResult = m_ptLuaState->lua_GetStack(iLevel, &tDbg);
		if( iResult==1 )
		{
			iResult = m_ptLuaState->lua_GetInfo("Slnu", &tDbg);
			if( iResult!=0 )
			{
				ucStatus = 0;

				// write status
				dbg_write_u08(ucStatus);

				// write all debug info
				dbg_write_achar(tDbg.name);
				dbg_write_achar(tDbg.namewhat);
				dbg_write_achar(tDbg.what);
				dbg_write_achar(tDbg.source);
				dbg_write_int(tDbg.currentline);
				dbg_write_int(tDbg.nups);
				dbg_write_int(tDbg.linedefined);
				dbg_write_int(tDbg.lastlinedefined);
			}
		}

		if( ucStatus!=0 )
		{
			// write status
			dbg_write_u08(ucStatus);
		}
	}
}


void serverkuh_mainFrame::dbg_get_locals(int iLevel)
{
	int iResult;
	wxString strValue;
	unsigned char ucStatus;
	lua_Debug tDbg = {0};
	int iLuaType;
	int iIndex;


	if( m_ptDebugClientSocket->IsConnected()==true )
	{
		// TODO: create a temp state here to prevent stack modifications in the original
//		L = wxlState.GetLuaState();
		iResult = m_ptLuaState->lua_GetStack(iLevel, &tDbg);
		if( iResult==1 )
		{
			iIndex = 1;
			do
			{
				strValue = m_ptLuaState->lua_GetLocal(&tDbg, iIndex);
				dbg_write_achar(strValue);

				if( strValue.IsEmpty()==false )
				{
					// get the value from the stack
					iLuaType = m_ptLuaState->lua_Type(-1);
					strValue = m_ptLuaState->lua_TypeName(iLuaType);
					dbg_write_achar(strValue);

					// get the typ
					strValue = dbg_getStackValue(-1);
					dbg_write_achar(strValue);

					// remove the value from the stack
					m_ptLuaState->lua_Pop(1);
				}
				else
				{
					break;
				}
				// next local
				++iIndex;
			} while( 1 );
		}
		else
		{
			dbg_write_achar(wxEmptyString);
		}
	}
}


wxString serverkuh_mainFrame::dbg_getStackValue(int iIndex)
{
	int iTyp;
	int iVal;
	double dVal;
	wxString strValue;
	const void *pvVal;


	iTyp = m_ptLuaState->lua_Type(iIndex);
	switch( iTyp )
	{
	case LUA_TNONE:
		strValue = wxEmptyString;
		break;

	case LUA_TNIL:
		strValue = wxT("nil");
		break;

	case LUA_TBOOLEAN:
		// NOTE: ToBoolean returns int
		iVal = m_ptLuaState->lua_ToBoolean(iIndex);
		if( iVal==0 )
		{
			strValue = wxT("false");
		}
		else
		{
			strValue = wxT("true");
		}
		break;

	case LUA_TLIGHTUSERDATA:
		pvVal = m_ptLuaState->lua_ToUserdata(iIndex);
		strValue.Printf("%p", pvVal);
		break;

	case LUA_TNUMBER:
		dVal = m_ptLuaState->lua_ToNumber(iIndex);
		strValue.Printf("%f", dVal);
		break;

	case LUA_TSTRING:
		strValue = m_ptLuaState->lua_TowxString(iIndex);
		break;

	case LUA_TTABLE:
		// don't recurse here, just return a summary
		strValue = dbg_dumpTable(iIndex);
		//pvVal = m_ptLuaState->lua_ToPointer(iIndex);
		//strValue.Printf("%p", pvVal);
		break;

	case LUA_TFUNCTION:
		strValue = "some function...";
		break;

	case LUA_TUSERDATA:
		pvVal = m_ptLuaState->lua_ToUserdata(iIndex);
		strValue.Printf("%p", pvVal);
		break;

	case LUA_TTHREAD:
		pvVal = m_ptLuaState->lua_ToPointer(iIndex);
		strValue.Printf("%p", pvVal);
		break;

	default:
		strValue.Printf("unknown type: %d", iTyp);
		break;
	}

	return strValue;
}


wxString serverkuh_mainFrame::dbg_dumpTable(int iIndex)
{
	bool fResult;
	wxString strKey;
	wxString strValue;
	wxString strDump;


	// is the index really a table?
	fResult = m_ptLuaState->lua_IsTable(iIndex);
	if( fResult!=true )
	{
		// no table
		strDump = wxT("not a table");
		// TODO: return some errormsg
	}
	else
	{
		// ok, it's a table, dump it

		// push a nil, this is the first value
		m_ptLuaState->lua_PushNil();
		// loop over all entries
		while( m_ptLuaState->lua_Next(iIndex-1)!=0 )
		{
			// NOTE: key is now at stack -2, value at stack -1

			strKey = dbg_getStackValue(-2);
			strValue = dbg_getStackValue(-1);
			strDump += strKey + wxT(" : ") + strValue + wxT("\n");

			// remove the value from the stack
			// NOTE: do not remove the key, it is necessary to get the next table line
			m_ptLuaState->lua_Pop(1);
		}
	}

	return strDump;
}

void serverkuh_mainFrame::dbg_get_upvalue(int iLevel, int iIndex)
{

}


bool serverkuh_mainFrame::dbg_read_string(wxString &strData)
{
	wxUint32 u32LastRead;
	size_t sizLen;
	char *pcBuf;
	bool fOk;


	fOk = false;

	strData.Empty();

	m_ptDebugClientSocket->Read(&sizLen, sizeof(sizLen));
	u32LastRead = m_ptDebugClientSocket->LastCount();
	if( u32LastRead==sizeof(sizLen) )
	{
		if( sizLen==0 )
		{
			fOk = true;
		}
		else
		{
			pcBuf = new char[sizLen];
			m_ptDebugClientSocket->Read(pcBuf, sizLen);
			u32LastRead = m_ptDebugClientSocket->LastCount();
			if( u32LastRead==sizLen )
			{
				strData = wxString::From8BitData(pcBuf, sizLen);
				fOk = true;
			}
			delete[] pcBuf;
		}
	}

	return fOk;
}


bool serverkuh_mainFrame::dbg_read_int(int *piData)
{
	wxUint32 u32LastRead;
	bool fOk;


	fOk = false;

	m_ptDebugClientSocket->Read(piData, sizeof(int));
	u32LastRead = m_ptDebugClientSocket->LastCount();
	if( u32LastRead==sizeof(int) )
	{
		fOk = true;
	}

	return fOk;
}


void serverkuh_mainFrame::dbg_write_u08(unsigned char ucData)
{
	m_ptDebugClientSocket->Write(&ucData, 1);
}


void serverkuh_mainFrame::dbg_write_int(int iData)
{
	m_ptDebugClientSocket->Write(&iData, sizeof(int));
}


void serverkuh_mainFrame::dbg_write_achar(const char *pcData)
{
	size_t sizStrLen;


	if( pcData==NULL )
	{
		sizStrLen = 0;
	}
	else
	{
		sizStrLen = strlen(pcData);
	}
	m_ptDebugClientSocket->Write(&sizStrLen, sizeof(size_t));
	if( sizStrLen!=0 )
	{
		m_ptDebugClientSocket->Write(pcData, sizStrLen);
	}
}

void serverkuh_mainFrame::OnLuaPrint(wxLuaEvent &event)
{
	wxLogMessage( event.GetString() );
}


void serverkuh_mainFrame::OnLuaError(wxLuaEvent &event)
{
	wxLogError( event.GetString() );
}


void serverkuh_mainFrame::OnMove(wxMoveEvent &event)
{
	// is the frame in normal state?
	if( IsIconized()==false && IsFullScreen()==false && IsMaximized()==false )
	{
		// frame is in normal state -> remember the position
		m_framePosition = event.GetPosition();
	}
}


void serverkuh_mainFrame::OnSize(wxSizeEvent &event)
{
	// is the frame in normal state?
	if( IsIconized()==false && IsFullScreen()==false && IsMaximized()==false )
	{
		// frame is in normal state -> remember size
		m_frameSize = event.GetSize();
	}
}


void serverkuh_mainFrame::OnDebugSocket(wxSocketEvent &event)
{
	wxSocketNotify tEventTyp;


	tEventTyp = event.GetSocketEvent();
	switch( tEventTyp )
	{
	case wxSOCKET_INPUT:
		wxLogMessage("wxSOCKET_INPUT");
		break;

	case wxSOCKET_LOST:
		wxLogMessage("wxSOCKET_LOST");
		break;

	case wxSOCKET_CONNECTION:
		wxLogMessage("wxSOCKET_CONNECTION");
	}
}


void serverkuh_mainFrame::luaTestHasFinished(void)
{
	// autoexit -> send quit event
	wxCommandEvent tCommandEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_EXIT);
	tCommandEvent.SetEventObject( this );
	GetEventHandler()->ProcessEvent(tCommandEvent);
}


wxString serverkuh_mainFrame::luaLoad(wxString strFileName)
{
	wxString strMsg;
	wxString strFileUrl;
	wxURL filelistUrl;
	wxURLError urlError;
	wxString strFileContents;
	growbuffer *ptGrowBuffer;
	unsigned char *pucData;
	size_t sizDataSize;
	bool fResult;
	wxString strData;


	if( strFileName.IsEmpty()==true )
	{
		// the filename parameter is invalid
		strMsg = _("lua load failed: empty filename");
		m_ptLuaState->wxlua_Error(strMsg);
	}
	else
	{
//		strFileUrl = m_strTestBaseUrl + wxFileName::GetPathSeparator() + strFileName;
		strFileUrl = m_strTestBaseUrl + strFileName;
		wxLogMessage(_("lua load: searching '%s'"), strFileUrl.fn_str());
		urlError = filelistUrl.SetURL(strFileUrl);
		if( urlError!=wxURL_NOERR )
		{
			// this was no valid url
			strMsg.Printf(_("lua load: invalid URL '%s': "), strFileUrl.fn_str());
			// try to show some details
			switch( urlError )
			{
			case wxURL_SNTXERR:
				strMsg += _("Syntax error in the URL string.");
				break;
			case wxURL_NOPROTO:
				strMsg += _("Found no protocol which can get this URL.");
				break;
			case wxURL_NOHOST:
				strMsg += _("A host name is required for this protocol.");
				break;
			case wxURL_NOPATH:
				strMsg += _("A path is required for this protocol.");
				break;
			case wxURL_CONNERR:
				strMsg += _("Connection error.");
				break;
			case wxURL_PROTOERR:
				strMsg += _("An error occurred during negotiation. (should never happen!)");
				break;
			default:
				strMsg += _("unknown errorcode");
				break;
			}

			// show the error message
			m_ptLuaState->wxlua_Error(strMsg);
		}
		else
		{
			ptGrowBuffer = new growbuffer(65536);
			fResult = readFsFile(ptGrowBuffer, strFileUrl);
			if( fResult==true )
			{
				sizDataSize = ptGrowBuffer->getSize();
				pucData = ptGrowBuffer->getData();
				strData = wxString::From8BitData((const char*)pucData, sizDataSize);
				strMsg.Printf(_("lua load: Read 0x%08X bytes"), strData.Len());
				wxLogMessage(strMsg);
			}
			else
			{
				m_ptLuaState->wxlua_Error(_("lua load: failed to read file"));
			}
			delete ptGrowBuffer;
		}
	}

	return strData;
}


void serverkuh_mainFrame::luaInclude(wxString strFileName)
{
	wxString strMsg;
	wxString strErrorMsg;
	wxString strFileUrl;
	wxURL filelistUrl;
	wxURLError urlError;
	wxString strFileContents;
	growbuffer *ptGrowBuffer;
	unsigned char *pucData;
	size_t sizDataSize;
	bool fResult;
	int iResult;
	int iGetTop;
	int iLineNr;


	if( m_ptLuaState!=NULL )
	{
		if( strFileName.IsEmpty()==true )
		{
			// the filename parameter is invalid
			strMsg = _("lua include failed: empty filename");
			m_ptLuaState->wxlua_Error(strMsg);
		}
		else
		{
//			strFileUrl = m_strTestBaseUrl + wxFileName::GetPathSeparator() + strFileName;
			strFileUrl = m_strTestBaseUrl + strFileName;
			wxLogMessage(_("lua include: searching '%s'"), strFileUrl.fn_str());
			urlError = filelistUrl.SetURL(strFileUrl);
			if( urlError!=wxURL_NOERR )
			{
				// this was no valid url
				strMsg.Printf(_("lua include: invalid URL '%s': "), strFileUrl.fn_str());
				// try to show some details
				switch( urlError )
				{
				case wxURL_SNTXERR:
					strMsg += _("Syntax error in the URL string.");
					break;
				case wxURL_NOPROTO:
					strMsg += _("Found no protocol which can get this URL.");
					break;
				case wxURL_NOHOST:
					strMsg += _("An host name is required for this protocol.");
					break;
				case wxURL_NOPATH:
					strMsg += _("A path is required for this protocol.");
					break;
				case wxURL_CONNERR:
					strMsg += _("Connection error.");
					break;
				case wxURL_PROTOERR:
					strMsg += _("An error occurred during negotiation. (should never happen!)");
					break;
				default:
					strMsg += _("unknown errorcode");
					break;
				}

				// show the error message
				m_ptLuaState->wxlua_Error(strMsg);
			}
			else
			{
				ptGrowBuffer = new growbuffer(65536);
				fResult = readFsFile(ptGrowBuffer, strFileUrl);
				if( fResult==true )
				{
					sizDataSize = ptGrowBuffer->getSize();
					pucData = ptGrowBuffer->getData();

					iGetTop = m_ptLuaState->lua_GetTop();
					iResult = m_ptLuaState->luaL_LoadBuffer((const char*)pucData, sizDataSize, strFileUrl.ToAscii());
					switch( iResult )
					{
					case 0:
						// ok, the function is on the stack -> execute the new code with no arguments and no return values
						wxLogMessage(_("lua_include: file loaded, executing code"));
						m_ptLuaState->lua_Call(0,0);
						break;

					case LUA_ERRSYNTAX:
						wxlua_errorinfo(m_ptLuaState->GetLuaState(), iResult, iGetTop, &strErrorMsg, &iLineNr);
						strMsg.Printf(_("error %d in line %d: ") + strErrorMsg, iResult, iLineNr);
						wxLogError(strMsg);
						strMsg = _("syntax error during pre-compilation");
						m_ptLuaState->wxlua_Error(strMsg);
						break;

					case LUA_ERRMEM:
						strMsg = _("memory allocation error");
						m_ptLuaState->wxlua_Error(strMsg);
						break;

					default:
						strMsg.Printf(_("Unknown error message from luaL_LoadBuffer: 0x%x"), iResult);
						m_ptLuaState->wxlua_Error(strMsg);
						break;
					}
				}
				else
				{
					m_ptLuaState->wxlua_Error(_("lua include: failed to read file"));
				}
				delete ptGrowBuffer;
			}
		}
	}
}


void serverkuh_mainFrame::luaSetLogMarker(void)
{
	m_tLogMarker = m_textCtrl->GetLastPosition();
}


wxString serverkuh_mainFrame::luaGetMarkedLog(void)
{
	wxTextPos tEndPos;
	wxString strLogData;


	tEndPos = m_textCtrl->GetLastPosition();
	strLogData = m_textCtrl->GetRange(m_tLogMarker, tEndPos);
	m_tLogMarker = tEndPos;

	return strLogData;
}


void serverkuh_mainFrame::luaScanPlugins(wxString strPattern)
{
	wxString strMsg;


	// does a plugin manager exist?
	if( m_ptPluginManager!=NULL )
	{
		// search
		m_ptPluginManager->ScanPlugins(strPattern, m_ptLuaState);
	}
	else
	{
		m_ptLuaState->wxlua_Error(_("no plugin manager exists in main application"));
	}
}


muhkuh_plugin_instance *serverkuh_mainFrame::luaGetNextPlugin(void)
{
	muhkuh_plugin_instance *ptInstance = NULL;
	muhkuh_plugin_instance *ptInstanceTmp;


	// does a plugin manager exist?
	if( m_ptPluginManager!=NULL )
	{
		// search
		ptInstanceTmp = m_ptPluginManager->GetNextPlugin();
		if( ptInstanceTmp!=NULL )
		{
			ptInstance = new muhkuh_plugin_instance(ptInstanceTmp);
		}
		else
		{
			ptInstance = new muhkuh_plugin_instance();
		}
	}

	return ptInstance;
}


muhkuh_wrap_xml *serverkuh_mainFrame::luaGetSelectedTest(void)
{
	wxTreeItemId itemId;
	const testTreeItemData *ptItemData;
	muhkuh_wrap_xml *ptWrapXml;
	wxListItem listItem;


	// get item data structure
	return m_ptWrapXml;
}


void TestHasFinished(void)
{
	// does the mainframe exist?
	if( g_ptMainFrame!=NULL )
	{
		// yes, the mainframe exists -> call the luaTestHasFinished function there
		g_ptMainFrame->luaTestHasFinished();
	}
}


wxString load(wxString strFileName)
{
	wxString strData;


	// does the mainframe exist?
	if( g_ptMainFrame!=NULL )
	{
		// yes, the mainframe exists -> call the luaLoadFile function there
		strData = g_ptMainFrame->luaLoad(strFileName);
	}
	return strData;
}


void include(wxString strFileName)
{
	// does the mainframe exist?
	if( g_ptMainFrame!=NULL )
	{
		// yes, the mainframe exists -> call the luaLoadFile function there
		g_ptMainFrame->luaInclude(strFileName);
	}
}


void setLogMarker(void)
{
	// does the mainframe exist?
	if( g_ptMainFrame!=NULL )
	{
		// yes, the mainframe exists -> call the luaLoadFile function there
		g_ptMainFrame->luaSetLogMarker();
	}
}


wxString getMarkedLog(void)
{
	wxString strData;


	// does the mainframe exist?
	if( g_ptMainFrame!=NULL )
	{
		// yes, the mainframe exists -> call the luaLoadFile function there
		strData = g_ptMainFrame->luaGetMarkedLog();
	}
	return strData;
}


void ScanPlugins(wxString strPattern)
{
	if( g_ptMainFrame!=NULL )
	{
		// yes, the mainframe exists -> call the strPattern function there
		g_ptMainFrame->luaScanPlugins(strPattern);
	}
}


muhkuh_plugin_instance *GetNextPlugin(void)
{
	muhkuh_plugin_instance *ptInstance = NULL;


	if( g_ptMainFrame!=NULL )
	{
		// yes, the mainframe exists -> call the GetNextPlugin function there
		ptInstance = g_ptMainFrame->luaGetNextPlugin();
	}

	return ptInstance;
}


muhkuh_wrap_xml *GetSelectedTest(void)
{
	muhkuh_wrap_xml *ptTest = NULL;


	if( g_ptMainFrame!=NULL )
	{
		// yes, the mainframe exists -> call the GetNextPlugin function there
		ptTest = g_ptMainFrame->luaGetSelectedTest();
	}

	return ptTest;
}

