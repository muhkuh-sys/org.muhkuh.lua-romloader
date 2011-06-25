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


#include <wx/longlong.h>
#include <wx/stdpaths.h>
#include <wx/txtstrm.h>
#include <wx/url.h>


#include "growbuffer.h"
#include "muhkuh_aboutDialog.h"
#include "muhkuh_app.h"
#include "muhkuh_brokenPluginDialog.h"
#include "muhkuh_icons.h"
#include "muhkuh_id.h"
#include "muhkuh_mainFrame.h"
#include "muhkuh_split_testdescription.h"
#include "muhkuh_version.h"
#include "readFsFile.h"


//-------------------------------------

BEGIN_EVENT_TABLE(muhkuh_mainFrame, wxFrame)
	EVT_IDLE(muhkuh_mainFrame::OnIdle)

	EVT_MENU(wxID_EXIT,                                             muhkuh_mainFrame::OnQuit)
	EVT_MENU(wxID_ABOUT,                                            muhkuh_mainFrame::OnAbout)
	EVT_MENU(wxID_PREFERENCES,                                      muhkuh_mainFrame::OnConfigDialog)
	EVT_MENU(wxID_HELP,                                             muhkuh_mainFrame::OnHelp)

	EVT_MENU(muhkuh_mainFrame_TestTree_Execute,                     muhkuh_mainFrame::OnTestExecute)
	EVT_MENU(muhkuh_mainFrame_TestTree_ShowHelp,                    muhkuh_mainFrame::OnTestHelp)
	EVT_MENU(muhkuh_mainFrame_TestTree_Rescan,                      muhkuh_mainFrame::OnTestRescan)
	EVT_MENU(muhkuh_mainFrame_menuShowTip,                          muhkuh_mainFrame::OnShowTip)
	EVT_MENU(muhkuh_mainFrame_menuRestoreDefaultPerspective,        muhkuh_mainFrame::OnRestoreDefaultPerspective)
	EVT_MENU(muhkuh_mainFrame_menuTestCancel,                       muhkuh_mainFrame::OnTestCancel)
	EVT_MENU(muhkuh_mainFrame_menuTestRescan,                       muhkuh_mainFrame::OnTestRescan)
	EVT_MENU(muhkuh_mainFrame_menuViewRepositoryPane,               muhkuh_mainFrame::OnViewRepositoryPane)
	EVT_MENU(muhkuh_mainFrame_menuViewPanicButton,                  muhkuh_mainFrame::OnViewPanicButton)
	EVT_MENU(muhkuh_mainFrame_menuViewTestTree,                     muhkuh_mainFrame::OnViewTestTree)
	EVT_MENU(muhkuh_mainFrame_menuViewMessageLog,                   muhkuh_mainFrame::OnViewMessageLog)
	EVT_MENU(muhkuh_mainFrame_menuViewWelcomePage,                  muhkuh_mainFrame::OnViewWelcomePage)
	EVT_MENU(muhkuh_mainFrame_menuViewTestDetails,                  muhkuh_mainFrame::OnViewTestDetails)

	EVT_COMBOBOX(muhkuh_mainFrame_RepositoryCombo_id,               muhkuh_mainFrame::OnRepositoryCombo)

	EVT_TREE_ITEM_ACTIVATED(muhkuh_mainFrame_TestTree_Ctrl,         muhkuh_mainFrame::OnTestTreeItemActivated)
	EVT_TREE_ITEM_MENU(muhkuh_mainFrame_TestTree_Ctrl,              muhkuh_mainFrame::OnTestTreeContextMenu)
	EVT_TREE_SEL_CHANGED(muhkuh_mainFrame_TestTree_Ctrl,            muhkuh_mainFrame::OnTestTreeItemSelected)

	EVT_HTML_LINK_CLICKED(muhkuh_mainFrame_Welcome_id,              muhkuh_mainFrame::OnMtdLinkClicked)
	EVT_HTML_LINK_CLICKED(muhkuh_mainFrame_TestDetails_id,          muhkuh_mainFrame::OnMtdLinkClicked)

	EVT_BUTTON(muhkuh_mainFrame_cancelTestButton_id,                muhkuh_mainFrame::OnTestCancel)
	EVT_AUINOTEBOOK_PAGE_CLOSE(muhkuh_mainFrame_Notebook_id,        muhkuh_mainFrame::OnNotebookPageClose)
	EVT_AUI_PANE_CLOSE(muhkuh_mainFrame::OnPaneClose)

	EVT_END_PROCESS(muhkuh_serverProcess_terminate,                 muhkuh_mainFrame::OnServerProcessTerminate)

	EVT_MUHKUH_COPY_PROGRESS(wxID_ANY,                              muhkuh_mainFrame::OnCopyProgress)

	EVT_MOVE(muhkuh_mainFrame::OnMove)
	EVT_SIZE(muhkuh_mainFrame::OnSize)
END_EVENT_TABLE()


muhkuh_mainFrame::muhkuh_mainFrame(void)
 : wxFrame(NULL, wxID_ANY, wxEmptyString)
 , m_treeCtrl(NULL)
 , m_textCtrl(NULL)
 , m_eInitState(MAINFRAME_INIT_STATE_UNCONFIGURED)
 , m_state(muhkuh_mainFrame_state_idle)
 , m_sizTestCnt(0)
 , m_ptHelp(NULL)
 , m_lServerPid(0)
 , m_ptServerProcess(NULL)
 , m_tipProvider(NULL)
 , m_welcomeHtml(NULL)
 , m_testDetailsHtml(NULL)
 , m_timerIdleWakeUp(this)
 , m_ptTextCtrl_TestOutput(NULL)
 , m_ptConfigData(NULL)
 , m_ptCopyProgress(NULL)
{
	wxLog *pOldLogTarget;
	wxFileName cfgName;
	wxFileConfig *ptConfig;
	int iLanguage;


	/* Create a new config data object. */
	m_ptConfigData = new muhkuh_config_data();

	// get the application path
	cfgName.Assign(wxStandardPaths::Get().GetExecutablePath());
	m_strApplicationPath = cfgName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	// get the config
	cfgName.SetFullName(wxTheApp->GetAppName()+wxT(".cfg"));
	ptConfig = new wxFileConfig(wxTheApp->GetAppName(), wxTheApp->GetVendorName(), cfgName.GetFullPath(), cfgName.GetFullPath(), wxCONFIG_USE_LOCAL_FILE);
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
		wxLogError(_("The language '%s' is not supported by the system."), wxLocale::GetLanguageName(iLanguage).c_str());
	}
	else
	{
		// search locales in the 'locale' directory
		// TODO: use some systempath for this
		wxLocale::AddCatalogLookupPathPrefix(wxT("locale"));
		m_locale.AddCatalog(wxTheApp->GetAppName());
	}

	// create the server process notifier
	m_ptServerProcess = new muhkuh_server_process(this, muhkuh_serverProcess_terminate);

	// build version string
	m_strVersion.Printf(wxT(MUHKUH_APPLICATION_NAME) wxT(" v%d.%d.%d"), MUHKUH_VERSION_MAJ, MUHKUH_VERSION_MIN, MUHKUH_VERSION_SUB);

	// use aui manager for this frame
	m_auiMgr.SetManagedWindow(this);

	// create a status bar with text on the left side and a gauge on the right
	CreateStatusBar(2);
	// use pane 1 for menu and toolbar help
	SetStatusBarPane(0);

	// create the menu bar
	createMenu();

	// create the controls
	createControls();

	// set new log target
	m_pLogTarget = new wxLogTextCtrl(m_textCtrl);
	pOldLogTarget = wxLog::SetActiveTarget(m_pLogTarget);
	if( pOldLogTarget!=NULL )
	{
		delete pOldLogTarget;
	}
	wxLog::SetVerbose(true);
	// this is for release
//	wxLog::SetLogLevel(wxLOG_Info);
	// this is for debug, it's a lot more noisy
	wxLog::SetLogLevel(wxLOG_Debug);

	wxLogMessage(_("application started"));

	// read the config
	read_config();

	// set the window title
	if( m_ptConfigData->m_strApplicationTitle.IsEmpty()==true )
	{
		// no custom title -> use the version string
		SetTitle(m_strVersion);
	}
	else
	{
		// show the custom title
		SetTitle(m_ptConfigData->m_strApplicationTitle);
	}

	// set the application icon
	if( m_ptConfigData->m_strApplicationIcon.IsEmpty()==true )
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
		m_frameIcons.AddIcon(m_ptConfigData->m_strApplicationIcon, wxBITMAP_TYPE_ICO);
	}
	SetIcons(m_frameIcons);

	// create the tip provider
	createTipProvider();

	// NOTE: do not scan the tests here, this freezes the win version
	m_eInitState  = MAINFRAME_INIT_STATE_CONFIGURED;

	InitDialog();

#if USE_LUA!=0
	/* Open a new lua state. */
	lua_muhkuh_create_default_state();
	lua_muhkuh_register_config_data(m_ptConfigData);
#endif
}


muhkuh_mainFrame::~muhkuh_mainFrame(void)
{
	write_config();

#if USE_LUA!=0
	/* Close any open lua state. */
	lua_muhkuh_close_default_state();
#endif

	// delete the help controller
	if( m_ptHelp!=NULL )
	{
		delete m_ptHelp;
		m_ptHelp = NULL;
	}

	// delete the tip provider
	if( m_tipProvider!=NULL )
	{
		delete m_tipProvider;
	}

	// force idle state
	setState(muhkuh_mainFrame_state_idle);

	finishTest();

	// delete the server notifier
	if( m_ptServerProcess!=NULL )
	{
		delete m_ptServerProcess;
	}

	if( m_ptConfigData!=NULL )
	{
		delete m_ptConfigData;
		lua_muhkuh_register_config_data(NULL);
	}

	m_auiMgr.UnInit();
}


void muhkuh_mainFrame::createMenu(void)
{
	wxMenu *file_menu;
	wxMenu *test_menu;
	wxMenu *view_menu;
	wxMenu *help_menu;
	wxMenuItem *ptMenuItem;


	file_menu = new wxMenu;
	ptMenuItem = new wxMenuItem(file_menu, wxID_PREFERENCES, _("Preferences"), _("Open the preferences dialog"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_wrench);
	file_menu->Append(ptMenuItem);
	ptMenuItem = new wxMenuItem(file_menu, wxID_EXIT, wxString(_("Quit"))+wxT("\tCtrl+Q"), _("Quit the program"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_door_out);
	file_menu->Append(ptMenuItem);

	test_menu = new wxMenu;
	ptMenuItem = new wxMenuItem(test_menu, muhkuh_mainFrame_menuTestCancel, _("Cancel"), _("Cancel the running test"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_cancel);
	test_menu->Append(ptMenuItem);
	ptMenuItem = new wxMenuItem(test_menu, muhkuh_mainFrame_menuTestRescan, wxString(_("Rescan"))+wxT("\tCtrl-R"), _("Rescan the repository"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_database_refresh);
	test_menu->Append(ptMenuItem);

	view_menu = new wxMenu;
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewRepositoryPane,	_("View Repository Selector"),		_("Toggle the visibility of the repository selector"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewPanicButton,	_("View Panic Button"),			_("Toggle the visibility of the panic button"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewTestTree,		_("View Test Tree"),			_("Toggle the visibility of the test tree"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewMessageLog,		_("View Message Log"),			_("Toggle the visibility of the message log"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewWelcomePage,	_("View Welcome Page"),			_("Toggle the visibility of the Welcome page"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewTestDetails,	_("View Test Details"),			_("Toggle the visibility of the Test Details page"));
	view_menu->Append(muhkuh_mainFrame_menuRestoreDefaultPerspective,	_("Restore Default Layout"),		_("Restore the default window layout"));

	help_menu = new wxMenu;
	ptMenuItem = new wxMenuItem(help_menu, wxID_HELP, wxString(_("Help"))+wxT("\tCtrl+H"), _("Show the documentation"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_help);
	help_menu->Append(ptMenuItem);
	help_menu->AppendSeparator();
	ptMenuItem = new wxMenuItem(help_menu, wxID_ABOUT, _("About"), _("Show details about Muhkuh"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_information);
	help_menu->Append(ptMenuItem);
	help_menu->AppendSeparator();
	ptMenuItem = new wxMenuItem(help_menu, muhkuh_mainFrame_menuShowTip, _("Show Tip"), _("Show some tips about Muhkuh"));
	ptMenuItem->SetBitmap(icon_famfamfam_silk_lightbulb);
	help_menu->Append(ptMenuItem);

	m_menuBar = new wxMenuBar;
	m_menuBar->Append(file_menu, _("&File"));
	m_menuBar->Append(test_menu, _("&Test"));
	m_menuBar->Append(view_menu, _("&View"));
	m_menuBar->Append(help_menu, _("&Help"));

	SetMenuBar(m_menuBar);
}


void muhkuh_mainFrame::createControls(void)
{
	wxAuiPaneInfo paneInfo;
	long style;
	wxSize tSize;
	wxListItem listItem;


	// TODO: is this really necessary?
	SetMinSize(wxSize(400,300));

	// add the repository combo box
	m_repositoryCombo = new wxBitmapComboBox(this, muhkuh_mainFrame_RepositoryCombo_id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	paneInfo.Name(wxT("repository_selector")).CaptionVisible(true).Caption(_("Repository Selector")).Top().Position(0);
	m_auiMgr.AddPane(m_repositoryCombo, paneInfo);

	// add the cancel button
	m_buttonCancelTest = new wxButton(this, muhkuh_mainFrame_cancelTestButton_id, _("Cancel Test"));
	paneInfo.Name(wxT("panic_button")).CaptionVisible(true).Caption(_("Panic Button")).Top().Position(1);
	m_auiMgr.AddPane(m_buttonCancelTest, paneInfo);

	// create the tree
	style = wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxSUNKEN_BORDER;
	m_treeCtrl = new wxTreeCtrl(this, muhkuh_mainFrame_TestTree_Ctrl, wxDefaultPosition, wxDefaultSize, style);
	paneInfo.Name(wxT("test_tree")).CaptionVisible(true).Caption(_("Test Tree")).Left().Position(0);
	m_auiMgr.AddPane(m_treeCtrl, paneInfo);

	// create the auinotebook
	m_notebook = new wxAuiNotebook(this, muhkuh_mainFrame_Notebook_id);
	paneInfo.Name(wxT("notebook")).CaptionVisible(false).Center().Position(0);
	m_auiMgr.AddPane(m_notebook, paneInfo);

	// create the "welcome" html
	createWelcomeWindow();
	// create the test details list
	createTestDetailsWindow();

	style = wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY;
	m_textCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, style);
	paneInfo.Name(wxT("message_log")).CaptionVisible(true).Caption(_("Message Log")).Bottom().Position(0);
	m_auiMgr.AddPane(m_textCtrl, paneInfo);

	m_auiMgr.Update();

	// save this layout as the default perspective
	strDefaultPerspective = m_auiMgr.SavePerspective();

	// create help controller
	m_ptHelp = new wxHtmlHelpController(wxHF_DEFAULT_STYLE);
}


void muhkuh_mainFrame::createTipProvider(void)
{
	// create the tip provider
	// TODO: check if the tips.txt file exists
	m_tipProvider = wxCreateFileTipProvider(wxT("muhkuh_tips.txt"), m_sizStartupTipsIdx);
}


void muhkuh_mainFrame::createWelcomeWindow(void)
{
	m_welcomeHtml = new wxHtmlWindow(this, muhkuh_mainFrame_Welcome_id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	m_welcomeHtml->SetPage(m_strWelcomePage);
	m_welcomeHtml->SetRelatedFrame(this, m_strVersion + wxT(" - %s"));
	m_welcomeHtml->SetRelatedStatusBar(0);
	m_notebook->AddPage(m_welcomeHtml, _("Welcome"), false, m_frameIcons.GetIcon(16));
}


void muhkuh_mainFrame::createTestDetailsWindow(void)
{
	m_testDetailsHtml = new wxHtmlWindow(this, muhkuh_mainFrame_TestDetails_id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	m_testDetailsHtml->SetPage(m_strTestDetails);
	m_testDetailsHtml->SetRelatedFrame(this, m_strVersion + wxT(" - %s"));
	m_testDetailsHtml->SetRelatedStatusBar(0);
	m_notebook->AddPage(m_testDetailsHtml, _("Test Details"), false, m_frameIcons.GetIcon(16));
}


wxString muhkuh_mainFrame::loadHtmlString(wxString strFileUrl)
{
	growbuffer *ptGrowBuffer;
	bool fResult;
	size_t sizDataSize;
	unsigned char *pucData;
	wxString strData;


	ptGrowBuffer = new growbuffer(65536);
	fResult = readFsFile(ptGrowBuffer, strFileUrl);
	if( fResult==true )
	{
		sizDataSize = ptGrowBuffer->getSize();
		pucData = ptGrowBuffer->getData();
		strData = wxString::From8BitData((const char*)pucData, sizDataSize);
	}
	else
	{
		strData.Printf(_("<html><body><h1>Error!</h1>Failed to load file '%s'!</body></html>"), strFileUrl.c_str());
	}
	delete ptGrowBuffer;

	return strData;
}


void muhkuh_mainFrame::reloadWelcomePage(void)
{
	wxString strPage;


	if( m_ptConfigData->m_strWelcomeFile.IsEmpty()==true )
	{
		// use the default welcome message
		strPage = _("<html><head><title>Welcome</title></head><body><h1>Welcome to <lua>return muhkuh_components_lua.get_version()</lua></h1></body></html>");
	}
	else
	{
		// load from file
		strPage = loadHtmlString(m_ptConfigData->m_strWelcomeFile);
	}

	m_strWelcomePage = strPage;

	if( m_welcomeHtml!=NULL )
	{
		m_welcomeHtml->SetPage(m_strWelcomePage);
	}
}


void muhkuh_mainFrame::reloadDetailsPage(void)
{
	wxString strTestName;
	wxString strSubTestName;
	wxString strPage;


	if( m_ptConfigData->m_strDetailsFile.IsEmpty()==true )
	{
		/* FIXME: take this from a config option. */
		strPage = wxT("<html><lua>\n")
			  wxT("	local strPage\n")
			  wxT("	local testDesc\n")
			  wxT("	local testName\n")
			  wxT("	local pageTitle\n")
			  wxT("	local subTestName\n")
			  wxT("\n")
			  wxT("\n")
			  wxT("	testDesc = muhkuh.GetSelectedTest()\n")
			  wxT("	if testDesc then\n")
			  wxT("		testName = testDesc:testDescription_getName()\n")
			  wxT("		pageTitle = \"Test Details for \" .. testName\n")
			  wxT("	else\n")
			  wxT("		pageTitle = \"Test Details\"\n")
			  wxT("	end\n")
			  wxT("\n")
			  wxT("	strPage =            \"<head>\\n\"\n")
			  wxT("	strPage = strPage .. \"<meta name=\\\"Keywords\\\" content=\\\"Muhkuh\\\">\\n\"\n")
			  wxT("	strPage = strPage .. \"<meta name=\\\"Author\\\" content=\\\"\" .. _G.__MUHKUH_VERSION .. \"\\\">\\n\"\n")
			  wxT("	strPage = strPage .. \"<meta name=\\\"Description\\\" content=\\\"\" .. pageTitle .. \"\\\">\\n\"\n")
			  wxT("	strPage = strPage .. \"<title>\" .. pageTitle .. \"</title>\\n\"\n")
			  wxT("	strPage = strPage .. \"</head><body>\\n\"\n")
			  wxT("\n")
			  wxT("\n")
			  wxT("	if testDesc then\n")
			  wxT("		strPage = strPage .. \"<h1><a href=\\\"mtd://\" .. testName .. \"\\\">\" .. testName .. \"</a></h1>\\n\"\n")
			  wxT("		strPage = strPage .. \"Version \" .. testDesc:testDescription_getVersion()\n")
			  wxT("		strPage = strPage .. \"<p>\\n\"\n")
			  wxT("\n")
			  wxT("		strPage = strPage .. \"<table border=1><tr><th>Subtest</th><th>Version</th></tr><tbody>\\n\"\n")
			  wxT("\n")
			  wxT("		for iCnt = 0 , testDesc:testDescription_getTestCnt()-1 do\n")
			  wxT("			if testDesc:testDescription_setTest(iCnt)~=true then\n")
			  wxT("				strPage = strPage .. \"<tr><td>Error:</td><td>Failed to query subtest!</td><tr>\\n\"\n")
			  wxT("				break\n")
			  wxT("			else\n")
			  wxT("				subTestName = testDesc:test_getName()\n")
			  wxT("\n")
			  wxT("				strPage = strPage .. \"<tr><td>\"\n")
			  wxT("				strPage = strPage .. \"<a href=\\\"mtd://\" .. testName .. \"#\" .. subTestName .. \"\\\">\" .. subTestName .. \"</a>\"\n")
			  wxT("				strPage = strPage .. \"</td><td>\"\n")
			  wxT("				strPage = strPage .. testDesc:test_getVersion()\n")
			  wxT("				strPage = strPage .. \"</td></tr>\\n\"\n")
			  wxT("			end\n")
			  wxT("		end\n")
			  wxT("\n")
			  wxT("		strPage = strPage .. \"</tbody></table>\\n\"\n")
			  wxT("	else\n")
			  wxT("		strPage = strPage .. \"<h1>\" .. pageTitle .. \"</h1>\\n\"\n")
			  wxT("		strPage = strPage .. \"No test selected.<p>Please select a test in the test tree.\"\n")
			  wxT("	end\n")
			  wxT("\n")
			  wxT("	strPage = strPage .. \"</body>\\n\"\n")
			  wxT("\n")
			  wxT("\n")
			  wxT("	return strPage\n")
			  wxT("</lua></html>\n")
		;
	}
	else
	{
		// load from file
		strPage = loadHtmlString(m_ptConfigData->m_strDetailsFile);
	}

	m_strTestDetails = strPage;

	if( m_testDetailsHtml!=NULL )
	{
		m_testDetailsHtml->SetPage(m_strTestDetails);
	}
}


void muhkuh_mainFrame::read_config(void)
{
	wxConfigBase *pConfig;
	wxString strPerspective;
	bool fPaneIsVisible;
	bool fWelcomePageIsVisible;
	bool fTestDetailsPageIsVisible;
	int iPageIdx;
	bool fWinMaximized;


	// get the config
	pConfig = wxConfigBase::Get();
	if( pConfig==NULL )
	{
		return;
	}

	// get mainframe position and size
	pConfig->SetPath(wxT("/MainFrame"));
	m_framePosition.x = pConfig->Read(wxT("x"), 50);
	m_framePosition.y = pConfig->Read(wxT("y"), 50);
	m_frameSize.SetWidth( pConfig->Read(wxT("w"), 640) );
	m_frameSize.SetHeight( pConfig->Read(wxT("h"), 480) );
	pConfig->Read(wxT("maximized"), &fWinMaximized, false);
	strPerspective = pConfig->Read(wxT("perspective"), wxEmptyString);
	pConfig->Read(wxT("showwelcome"), &fWelcomePageIsVisible, true);
	pConfig->Read(wxT("showtestdetails"), &fTestDetailsPageIsVisible, true);
	pConfig->Read(wxT("showtips"), &m_fShowStartupTips, true);
	m_sizStartupTipsIdx = pConfig->Read(wxT("tipidx"), (long)0);
	m_ptConfigData->m_strWelcomeFile = pConfig->Read(wxT("welcomepage"), wxEmptyString);
	m_ptConfigData->m_strDetailsFile = pConfig->Read(wxT("detailspage"), wxEmptyString);
	m_ptConfigData->m_strApplicationTitle = pConfig->Read(wxT("customtitle"), wxEmptyString);
	m_ptConfigData->m_strApplicationIcon = pConfig->Read(wxT("customicon"), wxEmptyString);

	// get lua settings
	pConfig->SetPath(wxT("/Lua"));
	m_ptConfigData->m_strLuaIncludePath = pConfig->Read(wxT("includepaths"), wxT("lua/?.lua"));
	m_ptConfigData->m_strLuaStartupCode = pConfig->Read(wxT("startupcode"), wxT("require(\"muhkuh_system\")\nmuhkuh_system.boot_xml()\n"));

	// get all plugins
	m_ptConfigData->m_ptPluginManager->read_config(pConfig);

	// get all repositories
	m_ptConfigData->m_ptRepositoryManager->read_config(pConfig);
	// show all repositories in the combo box
	updateRepositoryCombo();
	// set the active repository
	m_repositoryCombo->Select(m_ptConfigData->m_ptRepositoryManager->GetActiveRepository());

	// set window properties
	SetSize(m_framePosition.x, m_framePosition.y, m_frameSize.GetWidth(), m_frameSize.GetHeight());
	// set fullscreen mode
	Maximize(fWinMaximized);
	// set perspective
	if( strPerspective.IsEmpty()==false )
	{
		m_auiMgr.LoadPerspective(strPerspective,true);
		// set the "view" buttons according to the new perspective
		fPaneIsVisible = m_auiMgr.GetPane(m_repositoryCombo).IsShown();
		m_menuBar->Check(muhkuh_mainFrame_menuViewRepositoryPane, fPaneIsVisible);
		fPaneIsVisible = m_auiMgr.GetPane(m_buttonCancelTest).IsShown();
		m_menuBar->Check(muhkuh_mainFrame_menuViewPanicButton, fPaneIsVisible);
		fPaneIsVisible = m_auiMgr.GetPane(m_treeCtrl).IsShown();
		m_menuBar->Check(muhkuh_mainFrame_menuViewTestTree, fPaneIsVisible);
		fPaneIsVisible = m_auiMgr.GetPane(m_textCtrl).IsShown();
		m_menuBar->Check(muhkuh_mainFrame_menuViewMessageLog, fPaneIsVisible);
	}
	m_menuBar->Check(muhkuh_mainFrame_menuViewWelcomePage, fWelcomePageIsVisible);
	if( m_welcomeHtml!=NULL && fWelcomePageIsVisible==false )
	{
		// close welcome page
		iPageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
		if( iPageIdx!=wxNOT_FOUND )
		{
			m_notebook->DeletePage(iPageIdx);
		}
		// forget the pointer
		m_welcomeHtml = NULL;
	}
	m_menuBar->Check(muhkuh_mainFrame_menuViewTestDetails, fTestDetailsPageIsVisible);
	if( m_testDetailsHtml!=NULL && fTestDetailsPageIsVisible==false )
	{
		// close test details page
		iPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);
		if( iPageIdx!=wxNOT_FOUND )
		{
			m_notebook->DeletePage(iPageIdx);
		}
		// forget the pointer
		m_testDetailsHtml = NULL;	
	}
}


void muhkuh_mainFrame::write_config(void)
{
	wxConfigBase *pConfig;
	wxString strPerspective;
	int iPageIdx;
	bool fWelcomePageIsVisible;
	bool fTestDetailsPageIsVisible;
	bool fWinMaximized;


	pConfig = wxConfigBase::Get();
	if( pConfig == NULL )
	{
		return;
	}

	fWinMaximized = IsMaximized();

	// save the frame position
	strPerspective = m_auiMgr.SavePerspective();

	// save tip provider data
	if( m_tipProvider!=NULL )
	{
		m_sizStartupTipsIdx = m_tipProvider->GetCurrentTip();
	}

	// save visibility of welcome and test details page
	if( m_welcomeHtml!=NULL )
	{
		// close welcome page
		iPageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
		fWelcomePageIsVisible =  (iPageIdx!=wxNOT_FOUND);
	}
	else
	{
		fWelcomePageIsVisible = false;
	}
	if( m_testDetailsHtml!=NULL )
	{
		// close test details page
		iPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);
		fTestDetailsPageIsVisible = (iPageIdx!=wxNOT_FOUND);
	}
	else
	{
		fTestDetailsPageIsVisible = false;
	}

	pConfig->SetPath(wxT("/MainFrame"));
	pConfig->Write(wxT("x"),                (long)m_framePosition.x);
	pConfig->Write(wxT("y"),                (long)m_framePosition.y);
	pConfig->Write(wxT("w"),                (long)m_frameSize.GetWidth());
	pConfig->Write(wxT("h"),                (long)m_frameSize.GetHeight());
	pConfig->Write(wxT("maximized"),        fWinMaximized);
	pConfig->Write(wxT("perspective"),      strPerspective);
	pConfig->Write(wxT("showwelcome"),      fWelcomePageIsVisible);
	pConfig->Write(wxT("showtestdetails"),  fTestDetailsPageIsVisible);
	pConfig->Write(wxT("showtips"),         m_fShowStartupTips);
	pConfig->Write(wxT("tipidx"),           (long)m_sizStartupTipsIdx);
	pConfig->Write(wxT("welcomepage"),      m_ptConfigData->m_strWelcomeFile);
	pConfig->Write(wxT("detailspage"),      m_ptConfigData->m_strDetailsFile);
	pConfig->Write(wxT("customtitle"),      m_ptConfigData->m_strApplicationTitle);
	pConfig->Write(wxT("customicon"),       m_ptConfigData->m_strApplicationIcon);

	pConfig->SetPath(wxT("/"));

	// get lua settings
	pConfig->SetPath(wxT("/Lua"));
	pConfig->Write(wxT("includepaths"), m_ptConfigData->m_strLuaIncludePath);
	pConfig->Write(wxT("startupcode"), m_ptConfigData->m_strLuaStartupCode);
	pConfig->SetPath(wxT("/"));

	/* Save all plugins. */
	m_ptConfigData->m_ptPluginManager->write_config(pConfig);

	/* Save repositories. */
	m_ptConfigData->m_ptRepositoryManager->write_config(pConfig);
}


void muhkuh_mainFrame::setState(muhkuh_mainFrame_state tNewState)
{
	int iWelcomePageIdx;
	int iTestDetailsPageIdx;
	bool fRepositorySelected;


	// leave old state
	switch(m_state)
	{
	case muhkuh_mainFrame_state_scanning:
		break;

	case muhkuh_mainFrame_state_idle:
		break;

	case muhkuh_mainFrame_state_testing:
		break;
	}

	iWelcomePageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
	iTestDetailsPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);

	// enter new state
	switch(tNewState)
	{
	case muhkuh_mainFrame_state_scanning:
		m_buttonCancelTest->Enable(false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestCancel, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewWelcomePage, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewTestDetails, false);
		m_repositoryCombo->Enable(false);
		m_treeCtrl->Enable(false);
		m_menuBar->Enable(wxID_PREFERENCES, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestRescan, false);
		if( iWelcomePageIdx!=wxNOT_FOUND )
		{
			m_welcomeHtml->Enable(false);
		}
		if( iTestDetailsPageIdx!=wxNOT_FOUND )
		{
			m_testDetailsHtml->Enable(false);
		}
		break;

	case muhkuh_mainFrame_state_idle:
		m_buttonCancelTest->Enable(false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestCancel, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewWelcomePage, true);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewTestDetails, true);
		m_repositoryCombo->Enable(true);
		m_treeCtrl->Enable(true);
		m_menuBar->Enable(wxID_PREFERENCES, true);
		fRepositorySelected = (m_repositoryCombo->GetSelection() != wxNOT_FOUND);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestRescan, fRepositorySelected);
		if( iWelcomePageIdx!=wxNOT_FOUND )
		{
			m_welcomeHtml->Enable(true);
		}
		if( iTestDetailsPageIdx!=wxNOT_FOUND )
		{
			m_testDetailsHtml->Enable(true);
		}
		break;

	case muhkuh_mainFrame_state_testing:
		m_buttonCancelTest->Enable(true);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestCancel, true);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewWelcomePage, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewTestDetails, false);
		m_repositoryCombo->Enable(false);
		m_treeCtrl->Enable(false);
		m_menuBar->Enable(wxID_PREFERENCES, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuViewTestDetails, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestRescan, false);
		if( iWelcomePageIdx!=wxNOT_FOUND )
		{
			m_welcomeHtml->Enable(false);
		}
		if( iTestDetailsPageIdx!=wxNOT_FOUND )
		{
			m_testDetailsHtml->Enable(false);
		}
		break;
	}

	// set new state
	m_state = tNewState;
}


void muhkuh_mainFrame::OnIdle(wxIdleEvent& event)
{
	wxString strStatus;
	wxString strMemStatus;
	int iRepositoryIndex;
	int iLuaMemKb;
	wxULongLong tLuaMemBytes;
	bool fHasMoreInput;


	switch(m_state)
	{
	case muhkuh_mainFrame_state_scanning:
		strStatus = _("Scanning test descriptions...");
		break;

	case muhkuh_mainFrame_state_idle:
		if( m_eInitState==MAINFRAME_INIT_STATE_CONFIGURED )
		{
			// NOTE: this must be the first statement in this case, or it will be executed with every idle event
			m_eInitState = MAINFRAME_INIT_STATE_SCANNED;

			// show welcome and details page
			reloadWelcomePage();
			reloadDetailsPage();

			// add all help books
			m_ptHelp->AddBook(wxFileName(wxT("muhkuh.htb")), true);

			if( m_fShowStartupTips==true && m_tipProvider!=NULL )
			{
				m_fShowStartupTips = wxShowTip(this, m_tipProvider);
			}

			iRepositoryIndex = m_ptConfigData->m_ptRepositoryManager->GetActiveRepository();
			scanTests(iRepositoryIndex);

			check_plugins();
		}

		// show the number of loaded tests
		if( m_sizTestCnt==1 )
		{
			strStatus = _("1 test loaded...");
		}
		else
		{
			strStatus.Printf(_("%d tests loaded..."), m_sizTestCnt);
		}
		break;

	case muhkuh_mainFrame_state_testing:
		strStatus.Printf(_("Test '%s' in progress..."), m_strRunningTestName.c_str());

		fHasMoreInput = process_server_output();
		if( fHasMoreInput==true )
		{
			/* send this idle event again */
			event.RequestMore();
		}
		break;
	}

#if USE_LUA!=0
	/* Get the lua memory consumption in kilobytes. */
	iLuaMemKb = lua_muhkuh_get_memory_usage(NULL);
	tLuaMemBytes = (unsigned long)(iLuaMemKb*1024);
	strMemStatus.Printf(_("Lua uses %s ."), wxFileName::GetHumanReadableSize(tLuaMemBytes, _("an unknown amount of bytes"), 1, wxSIZE_CONV_TRADITIONAL));
	strStatus += strMemStatus;
#endif

	// set the status text
	SetStatusText(strStatus, 1);

	event.Skip();
}


void muhkuh_mainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	int iResult;
	bool fCloseApp = false;


	// is a test running? -> first ask to terminate it
	switch(m_state)
	{
	case muhkuh_mainFrame_state_scanning:
		// The progress dialog is modal, if a close request comes in now, it's something serious.
		fCloseApp = true;
		break;

        case muhkuh_mainFrame_state_testing:
		// ask for confirmation
		iResult = wxMessageBox(_("A test is still running. Are you sure you want to cancel it?"), m_strRunningTestName, wxYES_NO, this);
		if( iResult==wxYES )
		{
			wxLogMessage(_("Script canceled on user request!"));

			// test done -> go back to idle state
			setState(muhkuh_mainFrame_state_idle);

			finishTest();

			// close the application
			fCloseApp = true;
		}
		break;

	case muhkuh_mainFrame_state_idle:
	default:
		// ok, nothing running -> quit
		fCloseApp = true;
		break;
	}

	if( fCloseApp==true )
	{
		Close(true);
	}
}


void muhkuh_mainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	muhkuh_aboutDialog *pAboutDialog;


	pAboutDialog = new muhkuh_aboutDialog(this, m_strVersion, m_frameIcons);
	pAboutDialog->ShowModal();
	pAboutDialog->Destroy();
}


void muhkuh_mainFrame::OnConfigDialog(wxCommandEvent& WXUNUSED(event))
{
	muhkuh_config_data *ptConfigData;
	muhkuh_configDialog *cfgDlg;
	int iNewSelection;
	wxString strMessage;
	wxBitmap tRepoBitmap;
	wxSize tRepoBitmapSize;


	/* Clone the config data. */
	ptConfigData = new muhkuh_config_data(m_ptConfigData);

	/* Show the config dialog. */
	cfgDlg = new muhkuh_configDialog(this, m_strApplicationPath, ptConfigData);
	if( cfgDlg->ShowModal()==wxID_OK )
	{
		/* Copy the temp configuration data over the current one. */
		delete m_ptConfigData;
		m_ptConfigData = ptConfigData;

		/* Write the new config to disc. */
		write_config();

		/* Replace the default lua state. */
		lua_muhkuh_create_default_state();
		lua_muhkuh_register_config_data(m_ptConfigData);

		reloadWelcomePage();
		reloadDetailsPage();

		/* Show all repositories in the combo box. */
		updateRepositoryCombo();

		/* Get new test list. */
		iNewSelection = m_ptConfigData->m_ptRepositoryManager->GetActiveRepository();
		m_repositoryCombo->Select(iNewSelection);
		scanTests(iNewSelection);
	}
	else
	{
		/* The changes were canceled -> delete the temp configuration data. */
		delete ptConfigData;
	}

	/* Delete the config dialog. */
	cfgDlg->Destroy();
}


void muhkuh_mainFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
	wxLogMessage(wxT("OnHelp"));

	m_ptHelp->DisplayContents();
}


void muhkuh_mainFrame::OnTestExecute(wxCommandEvent& WXUNUSED(event))
{
	wxTreeItemId tItemId;
	const testTreeItemData *ptItemData;
	muhkuh_wrap_xml *ptWrapXml;


	// get the selected item from the tree
	tItemId = m_treeCtrl->GetSelection();
	if ( tItemId.IsOk()==true )
	{
		ptItemData = (const testTreeItemData*)m_treeCtrl->GetItemData(tItemId);
		if( ptItemData!=NULL )
		{
			ptWrapXml = ptItemData->getXmlDescription();
			if( ptWrapXml!=NULL )
			{
				// execute the main test
				executeTest(ptWrapXml, 0);
			}
		}
	}
}


bool muhkuh_mainFrame::check_plugins(void)
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

        if( m_ptConfigData->m_ptPluginManager!=NULL )
        {
                // loop over all plugins
                ulCnt = 0;
                ulMax = m_ptConfigData->m_ptPluginManager->getPluginCount();
                while( ulCnt<ulMax )
        {
                        if( m_ptConfigData->m_ptPluginManager->GetEnable(ulCnt) )
                {
                                // get the plugin status
                                fPluginOk = m_ptConfigData->m_ptPluginManager->IsOk(ulCnt);
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
                ptBrokenPluginDlg = new muhkuh_brokenPluginDialog(this, &v_plugins, m_ptConfigData->m_ptPluginManager);
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


bool muhkuh_mainFrame::executeTest_prepare_working_folder(void)
{
	bool fResult;
	wxString strTempFolder;         /* This is the folder where the application can create a new subfolder. */
	wxString strMsgFormat;
	wxString strMsg;
	wxString strSubFolder;
	wxFileName tWorkingDir;
	wxString strTempWorkingFolder;  /* This is the folder where the mtd is depacked and the startup file is generated. */
	int iResult;


	/* Expect success. */
	fResult = true;

	/* Get the working temp folder. */
	//strTempWorkingFolder = wxT("/home/cthelen/Compile/muhkuh_experimental/build/build/demo_layout/");
	strTempFolder = wxFileName::GetTempDir();

	/* Check if the temp working folder exists. */
	if( wxFileName::DirExists(strTempFolder)==false )
	{
		strMsg.Printf(_("The temporary working folder %s does not exist! Please create it or choose another folder."), strTempFolder.c_str());
		wxMessageBox(strMsg, _("Invalid working folder"), wxICON_ERROR, this);
		fResult = false;
	}
	else if( wxFileName::IsDirReadable(strTempFolder)==false )
	{
		strMsg.Printf(_("The application has no read access to the temporary working folder %s!"), strTempFolder.c_str());
		wxMessageBox(strMsg, _("Invalid working folder"), wxICON_ERROR, this);
		fResult = false;
	}
	else if( wxFileName::IsDirWritable(strTempFolder)==false )
	{
		strMsg.Printf(_("The application has no write access to the temporary working folder %s!"), strTempFolder.c_str());
		wxMessageBox(strMsg, _("Invalid working folder"), wxICON_ERROR, this);
		fResult = false;
	}
	else
	{
		/* Create a new subfolder with the process id. */
		strSubFolder.Printf(wxT("%s_%08x"), wxGetApp().GetAppName().c_str(), wxGetProcessId());
		tWorkingDir.AssignDir(strTempFolder);
		tWorkingDir.AppendDir(strSubFolder);
		strTempWorkingFolder = tWorkingDir.GetFullPath();
		wxLogMessage(_("Temp working folder: %s"), strTempWorkingFolder.c_str());

		/* Check the working path for spaces. */
		if( strTempWorkingFolder.Find(wxT(' '))!=wxNOT_FOUND )
		{
			strMsgFormat =      _("The path to the working folder is\n\n%s\n\nThis path contains spaces, which can cause a lot of problems. It is not recommended to continue!\n\n");
			strMsgFormat.Append(_("Press 'Ok' to ignore this warning and continue anyway.\n"));
			strMsgFormat.Append(_("Press 'Cancel' to stop the test execution."));
			strMsg.Printf(strMsgFormat, strTempWorkingFolder.c_str());
			iResult = wxMessageBox(strMsg, _("Problematic working folder"), wxCANCEL|wxOK|wxICON_EXCLAMATION, this);
			if( iResult==wxCANCEL )
			{
				fResult = false;
			}
		}

		if( fResult==true )
		{
			/* Does the temp folder already exist? */
			if( tWorkingDir.DirExists()==true )
			{
				strMsgFormat =      _("The working folder %s already exists. If you continue, all files in this folder will be erased!\n\n");
				strMsgFormat.Append(_("Press 'Ok' to continue and erase all files in the working folder.\n"));
				strMsgFormat.Append(_("Press 'Cancel' to stop the test execution."));
				strMsg.Printf(strMsgFormat, strTempWorkingFolder.c_str());
				iResult = wxMessageBox(strMsg, _("The working folder already exists"), wxCANCEL|wxOK|wxICON_EXCLAMATION, this);
				if( iResult==wxOK )
				{
					/* Ok, remove the complete folder! */
					/* FIXME: Rmdir only removes an empty folder in wx2.8 . Either move to 2.9 or write something recursive. */
					fResult = tWorkingDir.Rmdir();
					if( fResult!=true )
					{
						strMsg.Printf(_("Failed to remove the existing working folder %s!"), strTempWorkingFolder.c_str());
						wxMessageBox(strMsg, _("The working folder already exists"), wxICON_ERROR, this);
					}
				}
				else if( iResult==wxCANCEL )
				{
					fResult = false;
				}
			}

			if( fResult==true )
			{
				fResult = tWorkingDir.Mkdir();
				if( fResult!=true )
				{
					strMsg.Printf(_("Failed to create the working folder %s!"), strTempWorkingFolder.c_str());
					wxMessageBox(strMsg, _("Invalid working folder"), wxICON_ERROR, this);
				}
				else
				{
					/* Ok, now we have a fresh working folder. */
					m_strWorkingFolder = strTempWorkingFolder;
				}
			}
		}
	}

	return fResult;
}


void muhkuh_mainFrame::executeTest_extract_mtd(muhkuh_wrap_xml *ptTestData)
{
	size_t sizRepositoryIdx;
	size_t sizTestIdx;
	wxString strBaseUrl;
	wxThreadError tThreadError;


	/* Get the base url of the test. */
	sizRepositoryIdx = ptTestData->getRepositoryIndex();
	sizTestIdx = ptTestData->getTestIndex();
	strBaseUrl = m_ptConfigData->m_ptRepositoryManager->getTestlistBaseUrl(sizRepositoryIdx, sizTestIdx);

	/* Create the progress dialog. */
	m_ptCopyProgress = new wxProgressDialog(_("Copy files to a local working folder..."), wxT(" "), 100000, this, wxPD_AUTO_HIDE|wxPD_SMOOTH|wxPD_CAN_ABORT);

	/* Start the copy thread. */
	m_ptCopyProcess = new muhkuh_copy_process(strBaseUrl, m_strWorkingFolder, GetEventHandler());
	tThreadError = m_ptCopyProcess->CreateThread(wxTHREAD_JOINABLE);
	if( tThreadError==wxTHREAD_NO_ERROR )
	{
		tThreadError = m_ptCopyProcess->GetThread()->Run();
	}

	if( tThreadError!=wxTHREAD_NO_ERROR )
	{
		wxLogError("Failed to create the copy thread!");
		delete m_ptCopyProcess;
		m_ptCopyProcess = NULL;
		m_ptCopyProgress->Destroy();
		m_ptCopyProgress = NULL;
	}
}


bool muhkuh_mainFrame::executeTest_generate_start_code(wxString strStartLuaFile)
{
	bool fResult;
	int iResult;
	char *pcStartupCode;
	wxString strStartupCode;
	wxFileName tFileName;
	wxFile tFile;
	wxString strMsg;
	wxString strErrorMsg;


	/* Expect error. */
	fResult = false;

	/* Run the startup code generator. */
	iResult = lua_muhkuh_generate_text(NULL, m_ptConfigData->m_strLuaStartupCode.fn_str(), &pcStartupCode);
	if( iResult!=0 )
	{
		strErrorMsg = wxString::FromAscii(lua_muhkuh_error_to_string(iResult));
		strStartupCode = wxString::FromAscii(pcStartupCode);
		strMsg.Printf(_("Failed to execute the startup code generator: %d: %s : %s"), iResult, strErrorMsg, strStartupCode);
		strMsg.Append(_("The startup code generator can be defined in the lua section of the configuration dialog."));
		wxMessageBox(strMsg, _("Server startup error"), wxOK|wxICON_ERROR, this);
	}
	else
	{
		wxLogDebug("Lua startup code: %s", pcStartupCode);
		
		/* Create the filename. */
		tFileName.AssignDir(m_strWorkingFolder);
		tFileName.SetFullName(strStartLuaFile);
		
		/* Create the lua file. */
		fResult = tFile.Create(tFileName.GetFullPath(), true);
		if( fResult!=true )
		{
			strMsg.Printf(_("Failed to create temporary file '%s'!"), strStartLuaFile);
			wxMessageBox(strMsg, _("Server startup error"), wxICON_ERROR, this);
		}
		else
		{
			fResult = tFile.Write(wxString::FromAscii(pcStartupCode));
			if( fResult!=true )
			{
				strMsg.Printf(_("Failed to write to temporary file '%s'!"), strStartLuaFile);
				wxMessageBox(strMsg, _("Server startup error"), wxICON_ERROR, this);
			}
			else
			{
				wxLogMessage(wxT("Startup code written to %s."), strStartLuaFile);
			}

			tFile.Close();
		}
	}

	return fResult;
}


void muhkuh_mainFrame::executeTest(muhkuh_wrap_xml *ptTestData, unsigned int uiIndex)
{
	bool fResult;
	wxString strTempWorkingFolder;    /* This is the folder where the mtd is depacked and the startup file is generated. */
	muhkuh_split_testdescription *ptSlitter;
	wxString strOldWorkingDirectory;  /* This is the current working folder before this function changes it. */
	wxString strStartLuaFile;	  /* This is the complete path to the generated startup file. */
	wxString strStartCmd;


	m_strRunningTestName = ptTestData->testDescription_getName();
	m_sizRunningTest_RepositoryIdx = ptTestData->getRepositoryIndex();
	m_sizRunningTest_TestIdx = ptTestData->getTestIndex();
	m_sizRunningTest_SubTestIdx = uiIndex;
	wxLogDebug("m_strRunningTestName = %s", m_strRunningTestName);
	wxLogDebug("m_sizRunningTest_RepositoryIdx = %d", m_sizRunningTest_RepositoryIdx);
	wxLogDebug("m_sizRunningTest_TestIdx = %d", m_sizRunningTest_TestIdx);
	wxLogDebug("m_sizRunningTest_SubTestIdx = %d", m_sizRunningTest_SubTestIdx);

	/* Create the temp working folder and return the complete path to it. */
	fResult = executeTest_prepare_working_folder();
	if( fResult==true )
	{
		/* Extract the complete Test description. */
		executeTest_extract_mtd(ptTestData);
	}
}


void muhkuh_mainFrame::executeTestPart2(void)
{
	bool fResult;
	wxString strMsg;
	wxString strNow;


	muhkuh_split_testdescription *ptSplitter;
	wxString strOldWorkingDirectory;  /* This is the current working folder before this function changes it. */
	wxString strStartLuaFile;	  /* This is the complete path to the generated startup file. */
	wxString strStartCmd;


	/* Generate the code chunks. */
	ptSplitter = new muhkuh_split_testdescription();
	fResult = ptSplitter->split(m_strWorkingFolder);
	if( fResult==true )
	{
		/* Create the startup code. */
		strStartLuaFile = "start_gui.lua";
		fResult = executeTest_generate_start_code(strStartLuaFile);
		if( fResult==true )
		{
			/* Save the old working directory. It will be restored at the end of this function. */
			strOldWorkingDirectory = wxFileName::GetCwd();

			/* Change the working directory to the temp folder. */
			wxLogMessage("Changing to folder %s", m_strWorkingFolder);
			wxFileName::SetCwd(m_strWorkingFolder);

			wxLogMessage(_("execute test '%s', index %d"), m_strRunningTestName, m_sizRunningTest_SubTestIdx);


			/* Set state to 'testing'.
			 * NOTE: this must be done before the call to 'RunString', or
			 * the state will not change before the first idle event.
			 */
			setState(muhkuh_mainFrame_state_testing);

			/* Construct the start command. */
			strStartCmd.Printf("lua %s", strStartLuaFile);
			wxLogMessage("start command: %s", strStartCmd);

			m_lServerPid = wxExecute(strStartCmd, wxEXEC_ASYNC|wxEXEC_MAKE_GROUP_LEADER, m_ptServerProcess);
			if( m_lServerPid==0 )
			{
				strMsg.Printf(_("Failed to start the server with command: %s"), strStartCmd);
				wxMessageBox(strMsg, _("Server startup error"), wxICON_ERROR, this);
			}
			else
			{
				strNow = wxDateTime::Now().Format("%F %T");
				// create start message for the report tab
				strMsg.Printf(_("%s: started test '%s'.\n"), strNow, m_strRunningTestName);
				// create a new notebook tab
				m_ptTextCtrl_TestOutput = new wxTextCtrl(this, wxID_ANY, strMsg, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY);
				strMsg = m_strRunningTestName + " - " + strNow;
				m_notebook->AddPage(m_ptTextCtrl_TestOutput, strMsg, true, m_frameIcons.GetIcon(16));

				m_ptTextCtrl_TestOutput->AppendText("lalala\n");

				// start the timer to poll the server for input
				m_timerIdleWakeUp.Start(100);
			}

			/* Restore old working directory. */
			wxFileName::SetCwd(strOldWorkingDirectory);
		}
	}
}


bool muhkuh_mainFrame::process_server_output(void)
{
	bool fProcessedInput;
	wxInputStream *ptIs;
	wxString strNow;
	wxString strMsg;


	fProcessedInput = false;

	if( m_ptServerProcess!=NULL && m_ptTextCtrl_TestOutput!=NULL )
	{
		strNow = wxDateTime::Now().Format(wxT("%F %T"));

		if( m_ptServerProcess->IsInputAvailable()==true )
		{
			ptIs = m_ptServerProcess->GetInputStream();
			wxTextInputStream tis(*ptIs);
			m_ptTextCtrl_TestOutput->AppendText(strNow);
			m_ptTextCtrl_TestOutput->AppendText(wxT(":MSG:"));
			m_ptTextCtrl_TestOutput->AppendText(tis.ReadLine());
			m_ptTextCtrl_TestOutput->AppendText(wxT("\n"));
			fProcessedInput = true;
		}

		if( m_ptServerProcess->IsErrorAvailable()==true )
		{
			ptIs = m_ptServerProcess->GetErrorStream();
			wxTextInputStream tis(*ptIs);
			m_ptTextCtrl_TestOutput->AppendText(strNow);
			m_ptTextCtrl_TestOutput->AppendText(wxT(":ERR:"));
			m_ptTextCtrl_TestOutput->AppendText(tis.ReadLine());
			m_ptTextCtrl_TestOutput->AppendText(wxT("\n"));
			fProcessedInput = true;
		}
	}

	return fProcessedInput;
}


void muhkuh_mainFrame::finishTest(void)
{
	bool fResult;


	wxLogMessage(_("Test '%s' finished, cleaning up..."), m_strRunningTestName.c_str());

	m_timerIdleWakeUp.Stop();

	/* show all server output */
	do
	{
		fResult = process_server_output();
	} while( fResult==true );

	if( m_ptTextCtrl_TestOutput!=NULL )
	{
		m_ptTextCtrl_TestOutput->AppendText(wxDateTime::Now().Format(wxT("%F %T")).c_str());
		m_ptTextCtrl_TestOutput->AppendText(wxT("Test '"));
		m_ptTextCtrl_TestOutput->AppendText(m_strRunningTestName);
		m_ptTextCtrl_TestOutput->AppendText(wxT("' finished.\n"));
	}

	/* forget current server output tab */
	m_ptTextCtrl_TestOutput = NULL;

	// remove the temp file
	if( m_strRunningTestTempFileName.IsEmpty()==false && wxFileExists(m_strRunningTestTempFileName)==true )
	{
		fResult = wxRemoveFile(m_strRunningTestTempFileName);
		if( fResult!=true )
		{
			wxLogWarning(_("Failed to remove the temp file '%s'."), m_strRunningTestTempFileName.c_str());
		}
	}
}


#if 0
void muhkuh_mainFrame::executeTest(muhkuh_wrap_xml *ptTestData, unsigned int uiIndex)
{
        bool fResult;
        wxString strMsg;
        wxString strXmlUrl;
        wxString strServerCmd;


        // check all plugins for state ok before executing the test
        fResult = check_plugins();
        if( fResult==true )
        {
                m_strRunningTestName = ptTestData->testDescription_getName();
                m_sizRunningTest_RepositoryIdx = ptTestData->getRepositoryIndex();
                m_sizRunningTest_TestIdx = ptTestData->getTestIndex();

                strMsg.Printf(wxT("execute test '") + m_strRunningTestName + wxT("', index %d"), uiIndex);
                wxLogMessage(strMsg);

                if( m_ptLuaState!=NULL && m_ptLuaState->Ok()==true )
                {
#ifdef __ENABLE_DEBUGGER__
                        // create a new panel for the debugger
                        m_debuggerPanel = new muhkuh_debugger(this, m_strApplicationPath, m_usDebugServerPort, ptTestData);
                        m_notebook->AddPage(m_debuggerPanel, m_strRunningTestName, true);
#endif
                        // set state to 'testing'
                        // NOTE: this must be done before the call to 'RunString', or the state will not change before the first idle event
                        setState(muhkuh_mainFrame_state_testing);

                        strXmlUrl = m_ptRepositoryManager->getTestlistXmlUrl(m_sizRunningTest_RepositoryIdx, m_sizRunningTest_TestIdx);
#ifdef __ENABLE_DEBUGGER__
                        strServerCmd.Printf(wxT("./serverkuh -c Muhkuh.cfg -i %d -dlocalhost:%d %s"), uiIndex, m_usDebugServerPort, strXmlUrl.fn_str());
#else
                        strServerCmd.Printf(wxT("./serverkuh -c Muhkuh.cfg -i %d %s"), uiIndex, strXmlUrl.fn_str());
#endif
                        wxLogMessage(wxT("starting server: ") + strServerCmd);

                        m_lServerPid = wxExecute(strServerCmd, wxEXEC_ASYNC|wxEXEC_MAKE_GROUP_LEADER, m_ptServerProcess);
                        if( m_lServerPid==0 )
                {
                                strMsg.Printf(_("Failed to start the server with command: %s"), strServerCmd.fn_str());
                                wxMessageBox(strMsg, _("Server startup error"), wxICON_ERROR, this);
                        }
                }
        }
}


void muhkuh_mainFrame::finishTest(void)
{
        int iPanelIdx;


        wxLogMessage(_("Test '%s' finished, cleaning up..."), m_strRunningTestName.fn_str());

        if( m_debuggerPanel!=NULL )
        {
                // does the pannel still exist?
                iPanelIdx = m_notebook->GetPageIndex(m_debuggerPanel);
                if( iPanelIdx!=wxNOT_FOUND )
        {
                        m_notebook->RemovePage(iPanelIdx);
                }
                // delete the panel
                delete m_debuggerPanel;
                m_debuggerPanel = NULL;
        }

        // was this an autostart test?
        if( m_fRunningTestIsAutostart==true )
        {
                // autostart test has finished
                m_fRunningTestIsAutostart = false;

                // yes -> check for auto exit
                if( m_fAutoExit==true )
                {
                        // autoexit -> send quit event
                        wxCommandEvent tCommandEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_EXIT);
                        tCommandEvent.SetEventObject( this );
                        GetEventHandler()->ProcessEvent(tCommandEvent);
                }
        }
}
#endif


void muhkuh_mainFrame::updateRepositoryCombo(void)
{
        muhkuh_repository::REPOSITORY_TYP_E eTyp;
	const char **ppcXpm;
	wxString strReposEntry;
	wxBitmap tRepoBitmap;
	size_t sizCnt, sizMax;


	m_repositoryCombo->Clear();
	sizMax = m_ptConfigData->m_ptRepositoryManager->GetRepositoryCount();
	for(sizCnt=0; sizCnt<sizMax; ++sizCnt)
	{
		// get string representation of the new entry
		strReposEntry = m_ptConfigData->m_ptRepositoryManager->GetStringRepresentation(sizCnt);
		// get bitmap for the entry
		eTyp = m_ptConfigData->m_ptRepositoryManager->GetTyp(sizCnt);
		switch( eTyp )
		{
		case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
			ppcXpm = icon_famfamfam_silk_folder_table;
			break;
		case muhkuh_repository::REPOSITORY_TYP_FILELIST:
			ppcXpm = icon_famfamfam_silk_database;
			break;
		case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
			ppcXpm = icon_famfamfam_silk_script;
			break;
		case muhkuh_repository::REPOSITORY_TYP_ALLLOCAL:
			ppcXpm = icon_famfamfam_silk_folder_database;
			break;
		default:
			ppcXpm = icon_famfamfam_silk_exclamation;
			break;
		}
		tRepoBitmap = wxBitmap(ppcXpm);
		// add to combo box
		m_repositoryCombo->Append(strReposEntry, tRepoBitmap);
	}
}


void muhkuh_mainFrame::OnTestHelp(wxCommandEvent& WXUNUSED(event))
{
	m_ptHelp->DisplayContents();
}


void muhkuh_mainFrame::OnTestRescan(wxCommandEvent& WXUNUSED(event))
{
	int iRepositoryIdx;


	if( m_ptConfigData->m_ptRepositoryManager!=NULL )
	{
		iRepositoryIdx = m_ptConfigData->m_ptRepositoryManager->GetActiveRepository();
		scanTests(iRepositoryIdx);
	}
}


void muhkuh_mainFrame::OnViewRepositoryPane(wxCommandEvent &event)
{
	m_auiMgr.GetPane(m_repositoryCombo).Show(event.IsChecked());
	m_auiMgr.Update();
}


void muhkuh_mainFrame::OnViewPanicButton(wxCommandEvent &event)
{
	m_auiMgr.GetPane(m_buttonCancelTest).Show(event.IsChecked());
	m_auiMgr.Update();
}


void muhkuh_mainFrame::OnViewTestTree(wxCommandEvent &event)
{
	m_auiMgr.GetPane(m_treeCtrl).Show(event.IsChecked());
	m_auiMgr.Update();
}


void muhkuh_mainFrame::OnViewMessageLog(wxCommandEvent &event)
{
	m_auiMgr.GetPane(m_textCtrl).Show(event.IsChecked());
	m_auiMgr.Update();
}


void muhkuh_mainFrame::OnViewWelcomePage(wxCommandEvent &event)
{
	int iPageIdx;


	if( event.IsChecked()==true )
	{
		// show page
		if( m_welcomeHtml==NULL )
		{
			// page does not exist yet -> create and add
			createWelcomeWindow();
		}
	}
	else
	{
		// hide welcome page
		iPageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
		if( iPageIdx!=wxNOT_FOUND )
		{
			m_notebook->DeletePage(iPageIdx);
			m_welcomeHtml = NULL;
		}
	}
}


void muhkuh_mainFrame::OnViewTestDetails(wxCommandEvent &event)
{
	int iPageIdx;


	if( event.IsChecked()==true )
	{
		// show page
		if( m_testDetailsHtml==NULL )
		{
			// page does not exist yet -> create and add
			createTestDetailsWindow();
		}
	}
	else
	{
		// hide welcome page
		iPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);
		if( iPageIdx!=wxNOT_FOUND )
		{
			m_notebook->DeletePage(iPageIdx);
			m_testDetailsHtml = NULL;
		}
	}
}


void muhkuh_mainFrame::OnShowTip(wxCommandEvent &event)
{
	if( m_tipProvider!=NULL )
	{
		m_fShowStartupTips = wxShowTip(this, m_tipProvider);
	}
}


void muhkuh_mainFrame::OnRestoreDefaultPerspective(wxCommandEvent &event)
{
	int iPageIdx;


	// restore the default perspective
	m_auiMgr.LoadPerspective(strDefaultPerspective, true);

	// create welcome notebook
	if( m_welcomeHtml!=NULL )
	{
		iPageIdx = m_notebook->GetPageIndex(m_welcomeHtml);
	}
	else
	{
		iPageIdx = wxNOT_FOUND;
	}
	if( iPageIdx==wxNOT_FOUND )
	{
		createWelcomeWindow();
		m_menuBar->Check(muhkuh_mainFrame_menuViewWelcomePage, true);
	}

	// create test details notebook
	if( m_testDetailsHtml!=NULL )
	{
		iPageIdx = m_notebook->GetPageIndex(m_testDetailsHtml);
	}
	else
	{
		iPageIdx = wxNOT_FOUND;
	}
	if( iPageIdx==wxNOT_FOUND )
	{
		createTestDetailsWindow();
		m_menuBar->Check(muhkuh_mainFrame_menuViewTestDetails, true);
	}
}


void muhkuh_mainFrame::OnTestCancel(wxCommandEvent& WXUNUSED(event))
{
	int iResult;


	// is a test running?
	if( m_lServerPid!=0 )
	{
		iResult = wxMessageBox(_("Are you sure you want to cancel this test?"), m_strRunningTestName, wxYES_NO, this);
		if( iResult==wxYES )
		{
			wxLogMessage(_("Script canceled on user request!"));

			// try to kill the process
			wxProcess::Kill(m_lServerPid, wxSIGKILL, wxKILL_CHILDREN);
			m_lServerPid = 0;

			finishTest();

			// test done -> go back to idle state
			setState(muhkuh_mainFrame_state_idle);
		}
	}
}


void muhkuh_mainFrame::OnRepositoryCombo(wxCommandEvent &event)
{
	int iActiveRepoIdx;


	// get new selection
	iActiveRepoIdx = event.GetSelection();

	// set new repository
	scanTests(iActiveRepoIdx);
}


void muhkuh_mainFrame::OnTestTreeContextMenu(wxTreeEvent &event)
{
	wxTreeItemId itemId;
	wxPoint pt;
	wxMenu *ptMenu;
	wxString strTitle;


	// get item data structure
	itemId = event.GetItem();
        pt = event.GetPoint();
        if( itemId.IsOk() )
        {
                strTitle.Printf(_("Menu for %s"), m_treeCtrl->GetItemText(itemId).c_str());
                ptMenu = new wxMenu(strTitle);
                ptMenu->Append(muhkuh_mainFrame_TestTree_Execute,       _("&Execute..."));
                ptMenu->Append(muhkuh_mainFrame_TestTree_ShowHelp,      _("&Show Help..."));
		PopupMenu(ptMenu, pt);
		delete ptMenu;
	}
}


void muhkuh_mainFrame::OnTestTreeItemActivated(wxTreeEvent &event)
{
	wxTreeItemId itemId;


	// get item data structure
	itemId = event.GetItem();
	if( itemId.IsOk() )
	{
		wxCommandEvent exec_event(wxEVT_COMMAND_MENU_SELECTED, muhkuh_mainFrame_TestTree_Execute);
		wxPostEvent( this, exec_event );
	}
}


void muhkuh_mainFrame::OnTestTreeItemSelected(wxTreeEvent &event)
{
	wxTreeItemId itemId;
	const testTreeItemData *ptItemData;
	muhkuh_wrap_xml *ptWrapXml;
	wxListItem listItem;


	// get item data structure
	ptWrapXml = NULL;
	itemId = event.GetItem();
	if( itemId.IsOk() )
	{
		ptItemData = (const testTreeItemData*)m_treeCtrl->GetItemData(itemId);
		if( ptItemData!=NULL )
		{
			ptWrapXml = ptItemData->getXmlDescription();
		}
	}

	reloadDetailsPage();
}


void muhkuh_mainFrame::scanTests(int iActiveRepositoryIdx)
{
	bool bRes;
	size_t sizTestCnt;


	// clear all old tests
	m_sizTestCnt = 0;
	// clear the test details
	reloadDetailsPage();
	// clear the complete tree
	m_treeCtrl->DeleteAllItems();
	// add a root item
	m_treeCtrl->AddRoot(wxT("Root"));

	if( iActiveRepositoryIdx!=wxNOT_FOUND )
	{
		// set new state
		setState(muhkuh_mainFrame_state_scanning);

		// show process dialog
		m_scannerProgress = new wxProgressDialog(
			_("Scanning available test descriptions"),
			_("Please wait..."),
			1,
			this,
			wxPD_CAN_ABORT | wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME
		);

		// create the testlist
		bRes = m_ptConfigData->m_ptRepositoryManager->createTestlist(iActiveRepositoryIdx, muhkuh_mainFrame::repositoryScannerCallback, m_scannerProgress);

		// destroy the progress dialog, it's not possible to update the max count
		m_scannerProgress->Destroy();

		if( bRes==true )
		{
			// vaild directory or repository -> accept the index
			m_ptConfigData->m_ptRepositoryManager->SetActiveRepository(iActiveRepositoryIdx);

			// create a new progress dialog with the correct max count
			sizTestCnt = m_ptConfigData->m_ptRepositoryManager->getTestlistCount(iActiveRepositoryIdx);
			if( sizTestCnt>0 )
			{
				// show process dialog
				m_scannerProgress = new wxProgressDialog(
					_("Scanning available test descriptions"),
					_("Please wait..."),
					sizTestCnt,
					this,
					wxPD_CAN_ABORT | wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME
				);

				addAllTests(iActiveRepositoryIdx);

				// destroy the scanner progress dialog
				m_scannerProgress->Destroy();
			}
		}
		else
		{
			// failed to create test list -> this repository is not valid
			m_repositoryCombo->Select(wxNOT_FOUND);
			m_ptConfigData->m_ptRepositoryManager->SetActiveRepository(wxNOT_FOUND);
		}
	}

	setState(muhkuh_mainFrame_state_idle);
}


void muhkuh_mainFrame::addAllTests(int iActiveRepositoryIdx)
{
	size_t sizMax, sizCnt;
	wxString strMessage;
	bool fResult;
	bool fScannerIsRunning;


	// assume that cancel was not pressed yet
	fScannerIsRunning = true;

	// loop over all tests
	sizMax = m_ptConfigData->m_ptRepositoryManager->getTestlistCount(iActiveRepositoryIdx);
	sizCnt = 0;
	while( sizCnt<sizMax && fScannerIsRunning==true )
	{
		// check for cancel button
		strMessage.Printf(_("scanning '%s'..."), m_ptConfigData->m_ptRepositoryManager->getTestlistPrintUrl(iActiveRepositoryIdx, sizCnt).c_str());
		fScannerIsRunning = m_scannerProgress->Update(sizCnt, strMessage, NULL);

		fResult = scanFileXml(iActiveRepositoryIdx, sizCnt);

		// next item
		++sizCnt;

		if( fResult==false )
		{
			continue;
		}

		// one more test loaded
		++m_sizTestCnt;
	}
}


bool muhkuh_mainFrame::cutPath(wxString &strPath, wxArrayString *ptElems)
{
	size_t sizCnt, sizLen, sizPos;
	bool fResult;
	wxURI tUri;


	// cut the name into the path elements
	sizCnt = 0;
	sizLen = strPath.Length();
	// does the string start with a slash?
	if( strPath[0]==wxT('/') )
	{
		// yes -> ignore this slash
		++sizCnt;
	}
	sizPos = sizCnt;
	// loop over all path elements
	while( sizCnt<sizLen )
	{
		// get the next path element
		if( strPath[sizCnt]==wxT('/') )
		{
			if( sizPos<sizCnt )
			{
				// get the path element
				ptElems->Add(tUri.Unescape(strPath.Mid(sizPos, sizCnt-sizPos)));
			}
			// get start position for next path element
			sizPos = sizCnt+1;
		}
		// next char
		++sizCnt;
	}
	// is one element left?
	if( sizPos>=sizLen )
	{
		fResult = false;
	}
	else
	{
		// add the file element
		ptElems->Add(tUri.Unescape(strPath.Mid(sizPos, sizLen-sizPos)));
		fResult = true;
	}

	return fResult;
}


bool muhkuh_mainFrame::addTestTree(testTreeItemData *ptTestTreeItem)
{
	wxString strTestPath;
	wxString strPathElement;
	size_t sizCnt, sizLen;
	wxTreeItemId itemId;
	wxTreeItemId searchId;
	wxTreeItemId prevId;
	wxTreeItemIdValue searchCookie;
	int iCmp;
	wxArrayString aPathElems;
	bool fIsFirst;
	wxString strMsg;
	bool fResult;
	testTreeItemData *ptOldTreeItem;


	// get the full test name
	strTestPath = ptTestTreeItem->getTestLabel();

	fResult = cutPath(strTestPath, &aPathElems);
        if( fResult==false )
        {
                // path not ok
                strMsg.Printf(_("The testname '%s' is no valid path to a test."), strTestPath.c_str());
                wxMessageBox(strMsg, _("Failed to add test"), wxICON_ERROR, this);
        }
        else
	{
		// now create the complete path in the tree

		// start at the root item
		itemId = m_treeCtrl->GetRootItem();

		// loop over all elements except the last one in the path
		sizCnt = 0;
		sizLen = aPathElems.Count() - 1;
		while( fResult==true && sizCnt<sizLen )
		{
			// get the path element
			strPathElement = aPathElems.Item(sizCnt);

			// look for this element in the test tree
			searchId = m_treeCtrl->GetFirstChild(itemId, searchCookie);
			fIsFirst = true;
			do
			{
				if( searchId.IsOk()==false )
				{
					// no items in this branch, just add the new item
					searchId = m_treeCtrl->AppendItem(itemId, strPathElement, -1, -1, NULL);
					break;
				}
				else
				{
					iCmp = m_treeCtrl->GetItemText(searchId).CmpNoCase(strPathElement);
					if( iCmp==0 )
					{
						// a child with the requested name already exists
						// is this a folder or a test?
                                                ptOldTreeItem = (testTreeItemData*)m_treeCtrl->GetItemData(searchId);
                                                if( ptOldTreeItem!=NULL )
                                                {
                                                        strMsg.Printf(_("The path '%s' conflicts with a test in this path!"), strTestPath.c_str());
                                                        wxMessageBox(strMsg, _("Failed to add test"), wxICON_ERROR, this);
                                                        fResult = false;
                                                }
					}
					else if( iCmp<0 )
					{
						// remember previous entry for the insert operation
						prevId = searchId;
						// move to next child
						searchId = m_treeCtrl->GetNextChild(itemId, searchCookie);
						// switch from append to insert operation
						fIsFirst = false;
					}
					else
					{
						// create a new child if the item can't appear anymore because of the sort order
						if( fIsFirst==true )
						{
							// append the new item before the fist one in this branch
							searchId = m_treeCtrl->PrependItem(itemId, strPathElement, -1, -1, NULL);
						}
						else
						{
							// insert the new item before the current search position
							searchId = m_treeCtrl->InsertItem(itemId, prevId, strPathElement, -1, -1, NULL);
						}
						// the child with the requested name was just created
						break;
					}
				}
			} while( iCmp!=0 );

			// the child is the new parent node
			itemId = searchId;

			// next path element
			++sizCnt;
		}

		if( fResult==true )
		{
			// ok, path created, now append the test item
			strPathElement = aPathElems.Last();
			// look for this element in the test tree
			fIsFirst = true;
			searchId = m_treeCtrl->GetFirstChild(itemId, searchCookie);
			do
			{
				// reached end of list?
				if( searchId.IsOk()==false )
				{
					// end of list reached, just add the new item
					searchId = m_treeCtrl->AppendItem(itemId, strPathElement, -1, -1, ptTestTreeItem);
					// set iCmp to something != 0
					iCmp = 1;
					break;
				}
				else
				{
					iCmp = m_treeCtrl->GetItemText(searchId).CmpNoCase(strPathElement);
					if( iCmp<0 )
					{
						// remember previous entry for the insert operation
						prevId = searchId;
						// move to next child
						searchId = m_treeCtrl->GetNextChild(itemId, searchCookie);
						// switch from append to insert operation
						fIsFirst = false;
					}
					else if( iCmp>0 )
					{
						// create a new child if the item can't appear anymore because of the sort order
						if( fIsFirst==true )
						{
							// append the new item before the fist one in this branch
							searchId = m_treeCtrl->PrependItem(itemId, strPathElement, -1, -1, ptTestTreeItem);
						}
						else
						{
							// insert the new item before the current search position
							searchId = m_treeCtrl->InsertItem(itemId, prevId, strPathElement, -1, -1, ptTestTreeItem);
						}
						// the child with the requested name was just created
						break;
					}
				}
			} while( iCmp!=0 );
	
                        // check for error (element already exists)
                        if( iCmp==0 )
                        {
                                strMsg.Printf(_("The test '%s' already exists, skipping new instance!"), strTestPath.c_str());
                                wxMessageBox(strMsg, _("Failed to add test"), wxICON_ERROR, this);
                                fResult = false;
                        }
		}
	}

	return fResult;
}


bool muhkuh_mainFrame::scanFileXml(size_t sizRepositoryIdx, size_t sizTestIdx)
{
	wxString strXmlUrl;
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

	// get the url for the xml file
	strXmlUrl = m_ptConfigData->m_ptRepositoryManager->getTestlistXmlUrl(sizRepositoryIdx, sizTestIdx);

	// test if file exists
	wxLogVerbose(_("Reading testdescription '%s'"), strXmlUrl.c_str());
	ptFsFile = fileSystem.OpenFile(strXmlUrl);
	if( ptFsFile==NULL )
	{
		wxLogError(_("could not read xml file '%s'"), strXmlUrl.c_str());
	}
	else
	{
		// ok, file exists. read all data into a string
		ptInputStream = ptFsFile->GetStream();
		// stream read, construct xml tree
		ptWrapXml = new muhkuh_wrap_xml();
		if( ptWrapXml!=NULL )
		{
			if( ptWrapXml->initialize(ptInputStream, sizRepositoryIdx, sizTestIdx)==false )
			{
				wxLogError(_("Failed to read the xml file for testdescription '%s'"), strXmlUrl.c_str());
			}
			else
			{
				// get the main test name
				strTestName = ptWrapXml->testDescription_getName();
				// TODO: get the help file name
				strHelpFile.Empty();
				// create the tree item object
				ptItemData = new testTreeItemData(strTestName, strHelpFile, ptWrapXml);
				if( addTestTree(ptItemData)!=true )
				{
					delete ptItemData;
				}
				else
				{
					fResult = true;
				}
			}
		}
		delete ptFsFile;
	}

	return fResult;
}


void muhkuh_mainFrame::OnNotebookPageClose(wxAuiNotebookEvent &event)
{
	int iSelection;
	wxWindow *ptWin;


	// get the selected Page
	iSelection = event.GetSelection();
	ptWin = m_notebook->GetPage(iSelection);

	// close the welcome page?
	if( ptWin==m_welcomeHtml )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewWelcomePage, false);
		// forget the pointer
		m_welcomeHtml = NULL;
	}
	// close the test details page?
	else if( ptWin==m_testDetailsHtml )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewTestDetails, false);
		// forget the pointer
		m_testDetailsHtml = NULL;
	}
}


void muhkuh_mainFrame::OnPaneClose(wxAuiManagerEvent &event)
{
	wxWindow *ptWindow;


	ptWindow = event.pane->window;
	if( ptWindow==m_repositoryCombo )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewRepositoryPane, false);
	}
	else if( ptWindow==m_buttonCancelTest )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewPanicButton, false);
	}
	else if( ptWindow==m_treeCtrl )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewTestTree, false);
	}
	else if( ptWindow==m_textCtrl )
	{
		m_menuBar->Check(muhkuh_mainFrame_menuViewMessageLog, false);
	}
}


void muhkuh_mainFrame::OnServerProcessTerminate(wxProcessEvent &event)
{
	int iPid;


	iPid = event.GetPid();
	if( iPid==m_lServerPid )
	{
		wxLogMessage(_("The server terminated with returncode %d."), event.GetExitCode());
		m_lServerPid = 0;
		setState(muhkuh_mainFrame_state_idle);
		finishTest();
	}
	else
	{
		wxLogWarning(_("Ignoring terminate event from unknown process %d!"), iPid);
	}
}


void muhkuh_mainFrame::OnCopyProgress(wxMuhkuhCopyProgressEvent &tEvent)
{
	size_t sizFilesCnt;
	size_t sizFilesEnd;
	int iProgress;
	wxString strMessage;
	MUHKUH_COPY_PROCESS_STATE_T tState;
	bool fKeepRunning;
	bool fProcessFinished;
	wxThread::ExitCode tExitCode;


	if( m_ptCopyProgress!=NULL )
	{
		fProcessFinished = false;

		tState = tEvent.GetState();
		switch( tState )
		{
		case MUHKUH_COPY_PROCESS_STATE_Idle:
			strMessage = _("Initialising copy process...");
			fKeepRunning = m_ptCopyProgress->Pulse(strMessage);
			break;

		case MUHKUH_COPY_PROCESS_STATE_Scanning:
			strMessage.Printf(_("Found %d files..."), tEvent.GetTotalFiles());
			fKeepRunning = m_ptCopyProgress->Pulse(strMessage);
			break;

		case MUHKUH_COPY_PROCESS_STATE_Copy:
			sizFilesCnt = tEvent.GetCurrentFileIdx();
			sizFilesEnd = tEvent.GetTotalFiles();
			if( sizFilesEnd==0 )
			{
				iProgress = 0;
				strMessage = wxEmptyString;
			}
			else
			{
				iProgress = 100000 * sizFilesCnt / sizFilesEnd;
				strMessage.Printf(_("Copying file %s: %d bytes processed."), tEvent.GetCurrentFileName().c_str(), tEvent.GetCurrentFileBytesProcessed());
			}
			fKeepRunning = m_ptCopyProgress->Update(iProgress, strMessage);
			break;

		case MUHKUH_COPY_PROCESS_STATE_Finished:
			iProgress = 100000;
			strMessage.Printf(_("Finished."));
			m_ptCopyProgress->Update(iProgress, strMessage);
			fProcessFinished = true;
			fKeepRunning = false;
			break;
		}

		if(fKeepRunning==false)
		{
			fprintf(stderr, "Close request\n");

			m_ptCopyProgress->Destroy();
			m_ptCopyProgress = NULL;

			if( fProcessFinished==false )
			{
				fprintf(stderr, "Detele thread\n");
				m_ptCopyProcess->GetThread()->Delete();
			}
			tExitCode = m_ptCopyProcess->GetThread()->Wait();
			fprintf(stderr, "Thread returned %p\n", tExitCode);
			if( tExitCode==(wxThread::ExitCode)0 )
			{
				executeTestPart2();
			}
		}
	}
}


void muhkuh_mainFrame::OnMtdLinkClicked(wxHtmlLinkEvent &event)
{
	wxString strHref;
	wxURI tUri;
	wxString strElem;
	wxString strPath;
	wxArrayString aPathElems;
	size_t sizCnt, sizLen;
	wxTreeItemId parentId;
	wxTreeItemId searchId;
	wxTreeItemIdValue searchCookie;
	bool fResult;
	int iCmp;
	const testTreeItemData *ptItemData;
	muhkuh_wrap_xml *ptWrapXml;
	unsigned int uiCnt, uiMax;
	wxString strFragment;


	strHref = event.GetLinkInfo().GetHref();
	tUri.Create(strHref);
	if( tUri.HasScheme()==true && tUri.GetScheme().Cmp(wxT("mtd"))==0 )
	{
		// a mtd link must at least have a server or a path
		if( tUri.HasServer()==false && tUri.HasPath()==false )
		{
			// no server and no path -> no good mtd link
			wxLogError(_("mtd link has no valid path: '%s'"), tUri.Unescape(strHref).c_str());
		}
		else
		{
			if( tUri.HasServer()==true )
			{
				strPath = tUri.GetServer();
			}
			if( tUri.HasPath()==true )
			{
				// separate server and path with '/'
				strPath += wxT("/");
				strElem = tUri.GetPath();
				// cut off leading '/'
				if( strElem[0]==wxT('/') )
				{
					// cut off the first char
					strElem = strElem.Mid(1);
				}
				strPath += strElem;
			}

			// cut the path into elements
			fResult = cutPath(strPath, &aPathElems);
			if( fResult==false )
			{
				// path not ok
				wxLogError(wxT("The testname '%s' is no valid path to a test."), tUri.Unescape(strPath).c_str());
			}
			else
			{
				// path is ok, now search the tree for the test
				sizCnt = 0;
				sizLen = aPathElems.Count();

				// start at the root item
				parentId = m_treeCtrl->GetRootItem();

				iCmp = 1;
				while( sizCnt<sizLen )
				{
					// look for this element in the test tree
					searchId = m_treeCtrl->GetFirstChild(parentId, searchCookie);
					if( searchId.IsOk()==false )
					{
						// no items in this branch
						iCmp = 1;
						break;
					}
					else
					{
						do
						{
							iCmp = m_treeCtrl->GetItemText(searchId).Cmp(aPathElems.Item(sizCnt));
							if( iCmp==0 )
							{
								// found the element
								parentId = searchId;
								break;
							}
							else
							{
								// move to next child
								searchId = m_treeCtrl->GetNextChild(parentId, searchCookie);
							}
						} while( searchId.IsOk()==true );
					}

					// next path element
					++sizCnt;
				}

				// found all elements?
				if( iCmp!=0 || searchId.IsOk()!=true )
				{
					// no
					wxLogError(wxT("The test '%s' could not be found."), tUri.Unescape(strPath).c_str());
				}
				else
				{
					// yes, found all elements -> searchId is the test node!
					ptItemData = (const testTreeItemData*)m_treeCtrl->GetItemData(searchId);
					if( ptItemData!=NULL )
					{
						ptWrapXml = ptItemData->getXmlDescription();
						if( ptWrapXml!=NULL )
						{
							uiCnt = 0;
							if( tUri.HasFragment()==true )
							{
								strFragment = tUri.Unescape(tUri.GetFragment());
								uiMax = ptWrapXml->testDescription_getTestCnt();
								iCmp = 1;
								while( uiCnt<uiMax )
								{
									fResult = ptWrapXml->testDescription_setTest(uiCnt);
									if( fResult!=true )
									{
										wxLogError(_("failed to query subtest '%s' in test '%s'"), strFragment.c_str(), strPath.c_str());
										break;
									}
									else
									{
										++uiCnt;
										iCmp = strFragment.Cmp(ptWrapXml->test_getName());
										if( iCmp==0 )
										{
											// found subtest
											break;
										}
									}
								}
								// found test
								if( iCmp!=0 )
								{
									wxLogError(_("test '%s' has no subtest '%s'"), strPath.c_str(), strFragment.c_str());
								}
							}
							else
							{
								iCmp = 0;
							}

							if( iCmp==0 )
							{
								// execute the main test
								wxLogMessage(_("executing test!"));
								executeTest(ptWrapXml, uiCnt);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		event.Skip();
	}
}


void muhkuh_mainFrame::OnMove(wxMoveEvent &event)
{
	// is the frame in normal state?
	if( IsIconized()==false && IsFullScreen()==false && IsMaximized()==false )
	{
		// frame is in normal state -> remember the position
		m_framePosition = event.GetPosition();
	}
}


void muhkuh_mainFrame::OnSize(wxSizeEvent &event)
{
	// is the frame in normal state?
	if( IsIconized()==false && IsFullScreen()==false && IsMaximized()==false )
	{
		// frame is in normal state -> remember size
		m_frameSize = event.GetSize();
	}
}


bool muhkuh_mainFrame::repositoryScannerCallback(void *pvUser, wxString strMessage, int iProgressPos, int iProgressMax)
{
	bool fScannerIsRunning;
	wxProgressDialog *ptScannerProgress;
	double dProgress;


	if( pvUser!=NULL )
	{
		ptScannerProgress = (wxProgressDialog*)pvUser;

		if( iProgressPos<0 || iProgressMax<0 )
		{
			// mo pos or max -> pulse the dialog
			fScannerIsRunning = ptScannerProgress->Pulse(strMessage, NULL);
		}
		else
		{
			// set new position
			dProgress = ((double)m_iRepositoryProgressMax * (double)iProgressPos) / (double)iProgressMax;
			fScannerIsRunning = ptScannerProgress->Update((int)dProgress, strMessage, NULL);
		}
	}
	else
	{
		// no progress dialog
		fScannerIsRunning = true;
	}

	// update gui for new scanner message
	wxTheApp->Yield();

	return fScannerIsRunning;
}
