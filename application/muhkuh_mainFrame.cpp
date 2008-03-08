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


#include <wx/stdpaths.h>
#include <wx/url.h>

#include "growbuffer.h"
#include "muhkuh_version.h"
#include "muhkuh_aboutDialog.h"
#include "muhkuh_lua_interface.h"
#include "muhkuh_mainFrame.h"
#include "muhkuh_configDialog.h"
#include "muhkuh_icons.h"
#include "readFsFile.h"

//-------------------------------------
// the wxLua entries

#include <wxlua/include/wxlstate.h>
#include <wxbind/include/wxbinddefs.h>

WXIMPORT int wxluatype_wxXmlDocument;
WXIMPORT int wxluatype_wxPanel;

WXLUA_DECLARE_BIND_WXLUA
WXLUA_DECLARE_BIND_WXLUASOCKET
WXLUA_DECLARE_BIND_WXBASE
WXLUA_DECLARE_BIND_WXCORE
WXLUA_DECLARE_BIND_WXADV
WXLUA_DECLARE_BIND_WXNET
WXLUA_DECLARE_BIND_WXXML
WXLUA_DECLARE_BIND_WXXRC
WXLUA_DECLARE_BIND_WXHTML
WXLUA_DECLARE_BIND_WXAUI

muhkuh_mainFrame *g_ptMainFrame;

//-------------------------------------

BEGIN_EVENT_TABLE(muhkuh_mainFrame, wxFrame)
	EVT_IDLE(muhkuh_mainFrame::OnIdle)

	EVT_MENU(wxID_EXIT,						muhkuh_mainFrame::OnQuit)
	EVT_MENU(wxID_ABOUT,						muhkuh_mainFrame::OnAbout)
	EVT_MENU(wxID_PREFERENCES,					muhkuh_mainFrame::OnConfigDialog)
	EVT_MENU(wxID_HELP,						muhkuh_mainFrame::OnHelp)

	EVT_MENU(muhkuh_mainFrame_TestTree_Execute,			muhkuh_mainFrame::OnTestExecute)
	EVT_MENU(muhkuh_mainFrame_TestTree_ShowHelp,			muhkuh_mainFrame::OnTestHelp)
	EVT_MENU(muhkuh_mainFrame_TestTree_Rescan,			muhkuh_mainFrame::OnTestRescan)
	EVT_MENU(muhkuh_mainFrame_menuShowTip,				muhkuh_mainFrame::OnShowTip)
	EVT_MENU(muhkuh_mainFrame_menuRestoreDefaultPerspective,	muhkuh_mainFrame::OnRestoreDefaultPerspective)
	EVT_MENU(muhkuh_mainFrame_menuTestCancel,			muhkuh_mainFrame::OnTestCancel)
	EVT_MENU(muhkuh_mainFrame_menuTestRescan,			muhkuh_mainFrame::OnTestRescan)
	EVT_MENU(muhkuh_mainFrame_menuViewRepositoryPane,		muhkuh_mainFrame::OnViewRepositoryPane)
	EVT_MENU(muhkuh_mainFrame_menuViewPanicButton,			muhkuh_mainFrame::OnViewPanicButton)
	EVT_MENU(muhkuh_mainFrame_menuViewTestTree,			muhkuh_mainFrame::OnViewTestTree)
	EVT_MENU(muhkuh_mainFrame_menuViewMessageLog,			muhkuh_mainFrame::OnViewMessageLog)
	EVT_MENU(muhkuh_mainFrame_menuViewWelcomePage,			muhkuh_mainFrame::OnViewWelcomePage)
	EVT_MENU(muhkuh_mainFrame_menuViewTestDetails,			muhkuh_mainFrame::OnViewTestDetails)

	EVT_COMBOBOX(muhkuh_mainFrame_RepositoryCombo_id,		muhkuh_mainFrame::OnRepositoryCombo)

	EVT_TREE_ITEM_ACTIVATED(muhkuh_mainFrame_TestTree_Ctrl,		muhkuh_mainFrame::OnTestTreeItemActivated)
	EVT_TREE_ITEM_MENU(muhkuh_mainFrame_TestTree_Ctrl,		muhkuh_mainFrame::OnTestTreeContextMenu)
	EVT_TREE_SEL_CHANGED(muhkuh_mainFrame_TestTree_Ctrl,		muhkuh_mainFrame::OnTestTreeItemSelected)

	EVT_HTML_LINK_CLICKED(muhkuh_mainFrame_Welcome_id,		muhkuh_mainFrame::OnMtdLinkClicked)
	EVT_HTML_LINK_CLICKED(muhkuh_mainFrame_TestDetails_id,		muhkuh_mainFrame::OnMtdLinkClicked)

	EVT_LUA_PRINT(wxID_ANY,						muhkuh_mainFrame::OnLuaPrint)
	EVT_LUA_ERROR(wxID_ANY,						muhkuh_mainFrame::OnLuaError)

	EVT_BUTTON(muhkuh_mainFrame_cancelTestButton_id,		muhkuh_mainFrame::OnTestCancel)
	EVT_AUINOTEBOOK_PAGE_CLOSE(muhkuh_mainFrame_Notebook_id,	muhkuh_mainFrame::OnNotebookPageClose)
	EVT_AUI_PANE_CLOSE(muhkuh_mainFrame::OnPaneClose)
END_EVENT_TABLE()

/*
	EVT_SIZE(muhkuh_mainFrame::OnSize)
	EVT_LUA_DEBUG_HOOK(wxID_ANY,				muhkuh_mainFrame::OnLuaDebugHook)
*/

muhkuh_mainFrame::muhkuh_mainFrame(void)
 : wxFrame(NULL, wxID_ANY, wxEmptyString)
 , m_treeCtrl(NULL)
 , m_textCtrl(NULL)
 , m_eInitState(MAINFRAME_INIT_STATE_UNCONFIGURED)
 , m_ptLuaState(NULL)
 , m_state(muhkuh_mainFrame_state_idle)
 , m_sizTestCnt(0)
 , m_ptPluginManager(NULL)
 , m_ptRepositoryManager(NULL)
 , m_ptHelp(NULL)
 , m_testPanel(NULL)
 , m_tipProvider(NULL)
 , m_welcomeHtml(NULL)
 , m_testDetailsHtml(NULL)
{
	wxLog *pOldLogTarget;
	wxFileName cfgName;
	wxFileConfig *ptConfig;
	int iLanguage;


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

	// create the repository manager
	m_ptRepositoryManager = new muhkuh_repository_manager();

	// build version string
	m_strVersion.Printf(wxT(MUHKUH_APPLICATION_NAME) wxT(" v%d.%d.%d"), MUHKUH_VERSION_MAJ, MUHKUH_VERSION_MIN, MUHKUH_VERSION_SUB);

	// set the welcome message
	// TODO: show the "About" page from the docs here
	m_strWelcomePage  = wxT("<html><title>Welcome</title><body><h1>Welcome to ");
	m_strWelcomePage += m_strVersion;
	m_strWelcomePage += wxT("</h1>");
	m_strWelcomePage += wxT("</body></html>");

	// set the test details page for 'no test selected'
	m_strTestDetailsEmpty  = wxT("<html><title>Test Details</title><body><h1>Test Details</h1>");
	m_strTestDetailsEmpty += wxT("No test selected.<p>");
	m_strTestDetailsEmpty += wxT("Please select a test in the test tree.</body></html>");
	// set this as the current test details page
	m_strTestDetails = m_strTestDetailsEmpty;

	// use aui manager for this frame
	m_auiMgr.SetManagedWindow(this);

	// create a status bar with text on the left side and a gauge on the right
	CreateStatusBar(2);
	// use pane 1 for menu and toolbar help
	SetStatusBarPane(0);

	// set the muhkuh icon
	m_frameIcons.AddIcon(muhkuh_016_xpm);
	m_frameIcons.AddIcon(muhkuh_032_xpm);
	m_frameIcons.AddIcon(muhkuh_064_xpm);
	m_frameIcons.AddIcon(muhkuh_128_xpm);
	SetIcons(m_frameIcons);

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

	// set the window title
	SetTitle(m_strVersion);

	wxLogMessage(_("application started"));

	// read the config
	read_config();

	// create the tip provider
	createTipProvider();

	// NOTE: do not scan the tests here, this freezes the win version
	m_eInitState  = MAINFRAME_INIT_STATE_CONFIGURED;

	InitDialog();

	g_ptMainFrame = this;
}


muhkuh_mainFrame::~muhkuh_mainFrame(void)
{
	write_config();

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


	finishTest();

	// delete the plugin manager
	if( m_ptPluginManager!=NULL )
	{
		delete m_ptPluginManager;
	}

	// delete the repository manager
	if( m_ptRepositoryManager!=NULL )
	{
		delete m_ptRepositoryManager;
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
	int iSize;
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


void muhkuh_mainFrame::read_config(void)
{
	wxConfigBase *pConfig;
	int iMainFrameX, iMainFrameY, iMainFrameW, iMainFrameH;
	wxString strPerspective;
	bool fPaneIsVisible;
	bool fWelcomePageIsVisible;
	bool fTestDetailsPageIsVisible;
	int iPageIdx;


	// get the config
	pConfig = wxConfigBase::Get();
	if( pConfig==NULL )
	{
		return;
	}

	// get mainframe position and size
	pConfig->SetPath(wxT("/MainFrame"));
	iMainFrameX = pConfig->Read(wxT("x"), 50);
	iMainFrameY = pConfig->Read(wxT("y"), 50);
	iMainFrameW = pConfig->Read(wxT("w"), 640);
	iMainFrameH = pConfig->Read(wxT("h"), 480);
	strPerspective = pConfig->Read(wxT("perspective"), wxEmptyString);
	fWelcomePageIsVisible = pConfig->Read(wxT("showwelcome"), true);
	fTestDetailsPageIsVisible = pConfig->Read(wxT("showtestdetails"), true);
	m_fShowStartupTips = pConfig->Read(wxT("showtips"), true);
	m_sizStartupTipsIdx = pConfig->Read(wxT("tipidx"), (long)0);

	// get all plugins
	m_ptPluginManager->read_config(pConfig);

	// get all repositories
	m_ptRepositoryManager->read_config(pConfig);
	// show all repositories in the combo box
	updateRepositoryCombo();
	// set the active repository
	m_repositoryCombo->Select(m_ptRepositoryManager->GetActiveRepository());

	// set window properties
	SetSize(iMainFrameX, iMainFrameY, iMainFrameW, iMainFrameH);
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
	int iMainFrameX, iMainFrameY, iMainFrameW, iMainFrameH;
	wxString strPerspective;
	int iPageIdx;
	bool fWelcomePageIsVisible;
	bool fTestDetailsPageIsVisible;



	pConfig = wxConfigBase::Get();
	if( pConfig == NULL )
	{
		return;
	}

	// save the frame position
	GetPosition(&iMainFrameX, &iMainFrameY);
	GetSize(&iMainFrameW, &iMainFrameH);
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
	pConfig->Write(wxT("x"), (long)iMainFrameX);
	pConfig->Write(wxT("y"), (long)iMainFrameY);
	pConfig->Write(wxT("w"), (long)iMainFrameW);
	pConfig->Write(wxT("h"), (long)iMainFrameH);
	pConfig->Write(wxT("perspective"), strPerspective);
	pConfig->Write(wxT("showwelcome"), fWelcomePageIsVisible);
	pConfig->Write(wxT("showtestdetails"), fTestDetailsPageIsVisible);
	pConfig->Write(wxT("showtips"), m_fShowStartupTips);
	pConfig->Write(wxT("tipidx"), (long)m_sizStartupTipsIdx);
	pConfig->SetPath(wxT("/"));


	// save all plugins
	m_ptPluginManager->write_config(pConfig);

	// save repositories
	m_ptRepositoryManager->write_config(pConfig);
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
		m_fTestHasFinished = true;
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
		m_fTestHasFinished = false;
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
			if( m_fShowStartupTips==true && m_tipProvider!=NULL )
			{
				m_fShowStartupTips = wxShowTip(this, m_tipProvider);
			}

			iRepositoryIndex = m_ptRepositoryManager->GetActiveRepository();
			scanTests(iRepositoryIndex);
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
		// check for finish
		if( m_fTestHasFinished==true )
		{
			// test done -> go back to idle state
			setState(muhkuh_mainFrame_state_idle);
			finishTest();
		}
		else
		{
			strStatus.Printf(_("Test '%s' in progress..."), m_strRunningTestName.fn_str());

			// get the Lua Memory in kilobytes
			if( m_ptLuaState!=NULL )
			{
				iLuaMemKb = m_ptLuaState->lua_GetGCCount();
				strMemStatus.Printf(_("Lua uses %d kilobytes"), iLuaMemKb);
				strStatus += strMemStatus;
			}
		}
		break;
	}

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
		iResult = wxMessageBox(_("A test is still running. Are you sure you want to cancel it?"), m_strRunningTestName, wxYES_NO, this);
		if( iResult==wxYES )
		{
			wxLogMessage(_("Script canceled on user request!"));

			finishTest();

			// test done -> go back to idle state
			setState(muhkuh_mainFrame_state_idle);

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
	muhkuh_configDialog *cfgDlg;
	muhkuh_plugin_manager *ptTmpPluginManager;
	muhkuh_repository_manager *ptTmpRepositoryManager;
	int iCnt;
	int iNewSelection;
	wxString strMessage;
	wxBitmap tRepoBitmap;
	wxSize tRepoBitmapSize;


	// clone the plugin list
	ptTmpPluginManager = new muhkuh_plugin_manager(m_ptPluginManager);

	// clone the repository manager
	ptTmpRepositoryManager = new muhkuh_repository_manager(m_ptRepositoryManager);

	// show config dialog
	cfgDlg = new muhkuh_configDialog(this, m_strApplicationPath, ptTmpPluginManager, ptTmpRepositoryManager);
	if( cfgDlg->ShowModal()==wxID_OK )
	{
		// copy tmp plugin manager over current one
		delete m_ptPluginManager;
		m_ptPluginManager = ptTmpPluginManager;

		// copy tmp repository manager over current one
		delete m_ptRepositoryManager;
		m_ptRepositoryManager = ptTmpRepositoryManager;

		// show all repositories in the combo box
		updateRepositoryCombo();

		// get new test list
		iNewSelection = m_ptRepositoryManager->GetActiveRepository();
		m_repositoryCombo->Select(iNewSelection);
		scanTests(iNewSelection);
	}
	else
	{
		// changes canceled -> delete the temp managers
		delete ptTmpRepositoryManager;
		delete ptTmpPluginManager;
	}
	// delete config dialog
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


void muhkuh_mainFrame::executeTest(muhkuh_wrap_xml *ptTestData, unsigned int uiIndex)
{
	bool fCreated;
	int iResult;
	wxString strLuaCode;
	wxString strMsg;
	wxString strErrorMsg;
	wxString strLuaSystemModulePath;
	wxString strDebug;
	wxFileName cfgName;
	int iGetTop;
	int iLineNr;


	m_strRunningTestName = ptTestData->testDescription_getName();
	m_sizRunningTest_RepositoryIdx = ptTestData->getRepositoryIndex();
	m_sizRunningTest_TestIdx = ptTestData->getTestIndex();

	strDebug.Printf(wxT("execute test '") + m_strRunningTestName + wxT("', index %d"), uiIndex);
	wxLogMessage(strDebug);

	// get the boot code
	strLuaCode  = wxT("require(\"muhkuh_system\")\n");
	strLuaCode += wxT("muhkuh_system.boot_xml()\n");

	// delete old lua state
	if( m_ptLuaState!=NULL )
	{
		if( m_ptLuaState->Ok()==true )
		{
			m_ptLuaState->CloseLuaState(true);
		}
		delete m_ptLuaState;
		m_ptLuaState = NULL;
	}

	// clear all bindings
	{
		wxLuaBindingList *ptBindings;
		ptBindings = wxLuaBinding::GetBindingList();
		ptBindings->Clear();
	}

	// create a new lua state
	m_ptLuaState = new wxLuaState(false);
	if( m_ptLuaState==NULL )
	{
		wxLogError(_("Failed to allocate a new lua state"));
		return;
	}

	// init the standard lua bindings
	WXLUA_IMPLEMENT_BIND_WXLUA
	WXLUA_IMPLEMENT_BIND_WXLUASOCKET
	WXLUA_IMPLEMENT_BIND_WXBASE
	WXLUA_IMPLEMENT_BIND_WXCORE
	WXLUA_IMPLEMENT_BIND_WXADV
	WXLUA_IMPLEMENT_BIND_WXNET
	WXLUA_IMPLEMENT_BIND_WXXML
	WXLUA_IMPLEMENT_BIND_WXXRC
	WXLUA_IMPLEMENT_BIND_WXHTML
	WXLUA_IMPLEMENT_BIND_WXAUI

	// init the muhkuh lua bindings
	fCreated = wxLuaBinding_muhkuh_lua_init();
	if( fCreated!=true )
	{
		// failed to init the muhkuh lua bindings
		wxLogError(_("Failed to init the muhkuh_lua bindings"));
	}

	// init the lua bindings for all plugins
	iResult = m_ptPluginManager->initLuaBindings(m_ptLuaState);
	if( iResult!=0 )
	{
		wxLogError(_("Failed to init plugin bindings"));
		return;
	}

	// create the lua state
	fCreated = m_ptLuaState->Create(this, wxID_ANY);
	if( fCreated!=true )
	{
		wxLogError(_("Failed to create a new lua state"));
		return;
	}

	// is the state valid?
	if( m_ptLuaState->Ok()!=true )
	{
		wxLogError(_("Strange lua state"));
		return;
	}

	// TODO: overwrite the package.preload index metatable to
	// return one function for all module names.

	// set the package path
	// TODO: use a config option here
	cfgName.Assign(m_strApplicationPath, wxT("?.lua"));
	cfgName.AppendDir(wxT("lua"));
	strLuaSystemModulePath = cfgName.GetFullPath();
	wxLogMessage(wxT("Lua path:") + strLuaSystemModulePath);

	m_ptLuaState->lua_GetGlobal(wxT("package"));
	if( m_ptLuaState->lua_IsNoneOrNil(-1)==true )
	{
		wxLogError(_("Failed to get the global 'package'"));
		return;
	}
	m_ptLuaState->lua_PushString(strLuaSystemModulePath);
	m_ptLuaState->lua_SetField(-2, wxT("path"));


	// create a new panel for the test
	m_testPanel = new wxPanel(this);
	m_notebook->AddPage(m_testPanel, m_strRunningTestName, true);

	// set some global vars

	// set the lua version
	m_ptLuaState->lua_PushString(m_strVersion.ToAscii());
	m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_VERSION"));
	// set the xml document
	m_ptLuaState->wxluaT_PushUserDataType(ptTestData->getXmlDocument(), wxluatype_wxXmlDocument, false);
	m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_TEST_XML"));
	// set the selected test index
	m_ptLuaState->lua_PushNumber(uiIndex);
	m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_TEST_INDEX"));
	// set the panel
	m_ptLuaState->wxluaT_PushUserDataType(m_testPanel, wxluatype_wxPanel, false);
	m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_PANEL"));

	// set state to 'testing'
	// NOTE: this must be done before the call to 'RunString', or the state will not change before the first idle event
	setState(muhkuh_mainFrame_state_testing);

	// set the log marker
	luaSetLogMarker();

	iGetTop = m_ptLuaState->lua_GetTop();
	iResult = m_ptLuaState->RunString(strLuaCode, wxT("system boot"));
	if( iResult!=0 )
	{
		wxlua_errorinfo(m_ptLuaState->GetLuaState(), iResult, iGetTop, &strErrorMsg, &iLineNr);
		strMsg.Printf(_("error %d in line %d: ") + strErrorMsg, iResult, iLineNr);
		wxLogError(strMsg);
		finishTest();
		setState(muhkuh_mainFrame_state_idle);
	}
}


void muhkuh_mainFrame::finishTest(void)
{
	wxLuaBindingList *ptBindings;
	int iPanelIdx;


	wxLogMessage(_("Test '%s' finished, cleaning up..."), m_strRunningTestName.fn_str());

	// clear any plugin scans
	m_ptPluginManager->ClearAllMatches();

	// NOTE: dependencies must be cleared the here, they might come from
	// plugins which are already gone after a config change
	ptBindings = wxLuaBinding::GetBindingList();
	ptBindings->Clear();

	if( m_ptLuaState!=NULL )
	{
		if( m_ptLuaState->Ok()==true )
		{
			m_ptLuaState->CloseLuaState(true);
			m_ptLuaState->Destroy();
			wxSafeYield();
		}
		delete m_ptLuaState;
	
		m_ptLuaState = NULL;
	}

	if( m_testPanel!=NULL )
	{
		// does the pannel still exist?
		iPanelIdx = m_notebook->GetPageIndex(m_testPanel);
		if( iPanelIdx!=wxNOT_FOUND )
		{
			m_notebook->RemovePage(iPanelIdx);
		}
		// delete the panel
		delete m_testPanel;
		m_testPanel = NULL;
	}
}


void muhkuh_mainFrame::updateRepositoryCombo(void)
{
	wxString strReposEntry;
	wxBitmap tRepoBitmap;
	size_t sizCnt, sizMax;


	m_repositoryCombo->Clear();
	sizMax = m_ptRepositoryManager->GetRepositoryCount();
	for(sizCnt=0; sizCnt<sizMax; ++sizCnt)
	{
		// get string representation of the new entry
		strReposEntry = m_ptRepositoryManager->GetStringRepresentation(sizCnt);
		// get bitmap for the entry
		tRepoBitmap = m_ptRepositoryManager->GetBitmap(sizCnt);
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


	if( m_ptRepositoryManager!=NULL )
	{
		iRepositoryIdx = m_ptRepositoryManager->GetActiveRepository();
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


	iResult = wxMessageBox(_("Are you sure you want to cancel this test?"), m_strRunningTestName, wxYES_NO, this);
	if( iResult==wxYES )
	{
		wxLogMessage(_("Script canceled on user request!"));

		finishTest();

		// test done -> go back to idle state
		setState(muhkuh_mainFrame_state_idle);
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
		strTitle.Printf(_("Menu for %s"), m_treeCtrl->GetItemText(itemId).fn_str());
		ptMenu = new wxMenu(strTitle);
		ptMenu->Append(muhkuh_mainFrame_TestTree_Execute,	_("&Execute..."));
		ptMenu->Append(muhkuh_mainFrame_TestTree_ShowHelp,	_("&Show Help..."));
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
	wxString strTestName;
	unsigned int uiSubTestCnt;
	unsigned int uiCnt;
	wxString strSubTestName;
	bool fOk;
	long lItemIdx;
	wxString strPage;


	// clear the complete list
	m_strTestDetails = m_strTestDetailsEmpty;

	// get item data structure
	itemId = event.GetItem();
	if( itemId.IsOk() )
	{
		ptItemData = (const testTreeItemData*)m_treeCtrl->GetItemData(itemId);
		if( ptItemData!=NULL )
		{
			ptWrapXml = ptItemData->getXmlDescription();
			if( ptWrapXml!=NULL )
			{
				strTestName = ptWrapXml->testDescription_getName();

				strPage  = wxT("<html><body>\n");

				strPage += wxT("<h1><a href=\"mtd://");
				strPage += strTestName;
				strPage += wxT("\">");
				strPage += strTestName;
				strPage += wxT("</a></h1>");
				strPage += wxT("V");
				strPage += ptWrapXml->testDescription_getVersion();
				strPage += wxT("<p>");

				// append all subitems to the list
				uiSubTestCnt = ptWrapXml->testDescription_getTestCnt();
				uiCnt = 0;
				while( uiCnt<uiSubTestCnt )
				{
					fOk = ptWrapXml->testDescription_setTest(uiCnt);
					if( fOk!=true )
					{
						wxLogError(_("failed to query subtest!"));
						break;
					}

					strSubTestName = ptWrapXml->test_getName();

					strPage += wxT("<a href=\"mtd://") + strTestName + wxT('#') + strSubTestName + wxT("\">");
					strPage += strSubTestName;
					strPage += wxT("</a>");
					strPage += wxT("V");
					strPage += ptWrapXml->test_getVersion();
					strPage += wxT("<br>");

					// next item
					++uiCnt;
				}

				strPage += wxT("</body></html>");
				m_strTestDetails = strPage;
			}
		}
	}

	if( m_testDetailsHtml!=NULL )
	{
		m_testDetailsHtml->SetPage(m_strTestDetails);
	}
}


void muhkuh_mainFrame::scanTests(int iActiveRepositoryIdx)
{
	bool bRes;
	size_t sizTestCnt;


	// clear all old tests
	m_sizTestCnt = 0;
	// clear the test description
	m_strTestDetails = m_strTestDetailsEmpty;
	if( m_testDetailsHtml!=NULL )
	{
		m_testDetailsHtml->SetPage(m_strTestDetails);
	}
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
		bRes = m_ptRepositoryManager->createTestlist(iActiveRepositoryIdx, m_scannerProgress);

		// destroy the progress dialog, it's not possible to update the max count
		m_scannerProgress->Destroy();

		if( bRes==true )
		{
			// vaild directory or repository -> accept the index
			m_ptRepositoryManager->SetActiveRepository(iActiveRepositoryIdx);

			// create a new progress dialog with the correct max count
			sizTestCnt = m_ptRepositoryManager->getTestlistCount(iActiveRepositoryIdx);
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
			m_ptRepositoryManager->SetActiveRepository(wxNOT_FOUND);
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
	sizMax = m_ptRepositoryManager->getTestlistCount(iActiveRepositoryIdx);
	sizCnt = 0;
	while( sizCnt<sizMax && fScannerIsRunning==true )
	{
		// check for cancel button
		strMessage.Printf(_("scanning '%s'..."), m_ptRepositoryManager->getTestlistPrintUrl(iActiveRepositoryIdx, sizCnt).fn_str());
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
	size_t sizCnt, sizLen, sizPos;
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
		strMsg.Printf(_("The testname '%s' is no valid path to a test."), strTestPath.fn_str());
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
							strMsg.Printf(_("The path '%s' conflicts with a test in this path!"), strTestPath.fn_str());
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
				strMsg.Printf(_("The test '%s' already exists, skipping new instance!"), strTestPath.fn_str());
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
	strXmlUrl = m_ptRepositoryManager->getTestlistXmlUrl(sizRepositoryIdx, sizTestIdx);

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
			if( ptWrapXml->initialize(ptInputStream, sizRepositoryIdx, sizTestIdx)==false )
			{
				wxLogError(_("Failed to read the xml file for testdescription '%s'"), strXmlUrl.fn_str());
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
	int iResult;


	// get the selected Page
	iSelection = event.GetSelection();
	ptWin = m_notebook->GetPage(iSelection);

	// close the test panel?
	if( ptWin==m_testPanel )
	{
		// is the test still running?
		if( m_state==muhkuh_mainFrame_state_testing )
		{
			iResult = wxMessageBox(_("Are you sure you want to quit this test?"), m_strRunningTestName, wxYES_NO, this);
			if( iResult!=wxYES )
			{
				event.Veto();
			}
		}
		// close the panel?
		if( event.IsAllowed()==true )
		{
			// the panel will be deleted, forget the pointer
			m_testPanel = NULL;

			// yes -> update the main window
			finishTest();

			// test done -> go back to idle state
			setState(muhkuh_mainFrame_state_idle);
		}
	}
	// close the welcome page?
	else if( ptWin==m_welcomeHtml )
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


void muhkuh_mainFrame::OnLuaPrint(wxLuaEvent &event)
{
	wxLogMessage( event.GetString() );
}


void muhkuh_mainFrame::OnLuaError(wxLuaEvent &event)
{
	wxLogError( event.GetString() );
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
			wxLogError(_("mtd link has no valid path: '%s'"), tUri.Unescape(strHref).fn_str());
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
				wxLogError(wxT("The testname '%s' is no valid path to a test."), tUri.Unescape(strPath).fn_str());
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
							else if( iCmp<0 )
							{
								// move to next child
								searchId = m_treeCtrl->GetNextChild(parentId, searchCookie);
							}
							else
							{
								// the item can't appear anymore because of the sort order
								break;
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
					wxLogError(wxT("The test '%s' could not be found."), tUri.Unescape(strPath).fn_str());
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
										wxLogError(_("failed to query subtest '%s' in test '%s'"), strFragment.fn_str(), strPath.fn_str());
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
									wxLogError(_("test '%s' has no subtest '%s'"), strPath.fn_str(), strFragment.fn_str());
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


void muhkuh_mainFrame::luaTestHasFinished(void)
{
	m_fTestHasFinished = true;
}


wxString muhkuh_mainFrame::luaLoad(wxString strFileName)
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
		strFileUrl = m_ptRepositoryManager->getTestlistBaseUrl(m_sizRunningTest_RepositoryIdx, m_sizRunningTest_TestIdx) + wxFileName::GetPathSeparator() + strFileName;
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
			wxLogError(strMsg);
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
				wxLogError(_("lua load: failed to read file"));
			}
			delete ptGrowBuffer;
		}
	}

	return strData;
}


void muhkuh_mainFrame::luaInclude(wxString strFileName, wxString strChunkName)
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
			if( strChunkName.IsEmpty()==true )
			{
				// the chunkname parameter is invalid
				strMsg = _("lua include failed: empty chunkname");
				m_ptLuaState->wxlua_Error(strMsg);
			}
			else
			{
				strFileUrl = m_ptRepositoryManager->getTestlistBaseUrl(m_sizRunningTest_RepositoryIdx, m_sizRunningTest_TestIdx) + wxFileName::GetPathSeparator() + strFileName;
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
					wxLogError(strMsg);
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
						iResult = m_ptLuaState->luaL_LoadBuffer((const char*)pucData, sizDataSize, strChunkName.ToAscii());
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
						wxLogError(_("lua include: failed to read file"));
					}
					delete ptGrowBuffer;
				}
			}
		}
	}
}


void muhkuh_mainFrame::luaSetLogMarker(void)
{
	m_tLogMarker = m_textCtrl->GetLastPosition();
}


wxString muhkuh_mainFrame::luaGetMarkedLog(void)
{
	wxTextPos tEndPos;
	wxString strLogData;


	tEndPos = m_textCtrl->GetLastPosition();
	strLogData = m_textCtrl->GetRange(m_tLogMarker, tEndPos);
	m_tLogMarker = tEndPos;

	return strLogData;
}


void muhkuh_mainFrame::luaScanPlugins(wxString strPattern)
{
	wxString strMsg;
	bool fResult;


	// does a plugin manager exist?
	if( m_ptPluginManager!=NULL )
	{
		// search
		fResult = m_ptPluginManager->ScanPlugins(strPattern);
		if( fResult!=true )
		{
			strMsg.Printf(_("failed to compile the regex pattern: "), strPattern.fn_str());
			m_ptLuaState->wxlua_Error(strMsg);
		}
	}
	else
	{
		m_ptLuaState->wxlua_Error(_("no plugin manager exists in main application"));
	}
}


muhkuh_plugin_instance *muhkuh_mainFrame::luaGetNextPlugin(void)
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


void include(wxString strFileName, wxString strChunkName)
{
	// does the mainframe exist?
	if( g_ptMainFrame!=NULL )
	{
		// yes, the mainframe exists -> call the luaLoadFile function there
		g_ptMainFrame->luaInclude(strFileName, strChunkName);
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

