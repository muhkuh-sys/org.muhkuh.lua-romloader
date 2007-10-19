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
#include "muhkuh_testTreeItemData.h"
#include "readFsFile.h"

//-------------------------------------
// the wxLua entries

#include <wxlua/include/wxlstate.h>
#include <wxbind/include/wxbinddefs.h>

WXIMPORT int s_wxluatag_wxXmlDocument;
WXIMPORT int s_wxluatag_wxPanel;

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
// the main window icons

#include "muhkuh_016.xpm"
#include "muhkuh_032.xpm"
#include "muhkuh_064.xpm"
#include "muhkuh_128.xpm"

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

	EVT_COMBOBOX(muhkuh_mainFrame_RepositoryCombo_id,		muhkuh_mainFrame::OnRepositoryCombo)

	EVT_TREE_ITEM_ACTIVATED(muhkuh_mainFrame_TestTree_Ctrl,		muhkuh_mainFrame::OnTestTreeItemActivated)
	EVT_TREE_ITEM_MENU(muhkuh_mainFrame_TestTree_Ctrl,		muhkuh_mainFrame::OnTestTreeContextMenu)
	EVT_TREE_SEL_CHANGED(muhkuh_mainFrame_TestTree_Ctrl,		muhkuh_mainFrame::OnTestTreeItemSelected)

	EVT_LUA_PRINT(wxID_ANY,						muhkuh_mainFrame::OnLuaPrint)
	EVT_LUA_ERROR(wxID_ANY,						muhkuh_mainFrame::OnLuaError)

	EVT_BUTTON(muhkuh_mainFrame_cancelTestButton_id,		muhkuh_mainFrame::OnTestCancel)
	EVT_AUINOTEBOOK_PAGE_CLOSE(muhkuh_mainFrame_Notebook_id,	muhkuh_mainFrame::OnNotebookPageClose)
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
{
	wxLog *pOldLogTarget;
	wxString strMsg;
	wxFileName cfgName;
	wxFileConfig *ptConfig;


	// get the application path
	cfgName.Assign(wxStandardPaths::Get().GetExecutablePath());
	m_strApplicationPath = cfgName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	// get the config
	cfgName.SetFullName(wxTheApp->GetAppName()+wxT(".cfg"));
	ptConfig = new wxFileConfig(wxTheApp->GetAppName(), wxTheApp->GetVendorName(), cfgName.GetFullPath(), cfgName.GetFullPath(), wxCONFIG_USE_LOCAL_FILE);
	wxConfigBase::Set(ptConfig);
	ptConfig->SetRecordDefaults();

	// create the plugin manager
	m_ptPluginManager = new muhkuh_plugin_manager();

	// create the repository manager
	m_ptRepositoryManager = new muhkuh_repository_manager();

	// use aui manager for this frame
	m_auiMgr.SetManagedWindow(this);

	// create a status bar with text on the left side and a gauge on the right
	CreateStatusBar(2);
	// use pane 1 for menu and toolbar help
	SetStatusBarPane(0);

	// set the muhkuh icon
	frameIcons.AddIcon(muhkuh_016_xpm);
	frameIcons.AddIcon(muhkuh_032_xpm);
	frameIcons.AddIcon(muhkuh_064_xpm);
	frameIcons.AddIcon(muhkuh_128_xpm);
	SetIcons(frameIcons);

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

	// build version string
	strVersion.Printf(wxT(MUHKUH_APPLICATION_NAME) wxT(" v%d.%d.%d"), MUHKUH_VERSION_MAJ, MUHKUH_VERSION_MIN, MUHKUH_VERSION_SUB);
	// set the window title
	SetTitle(strVersion);

	// set the welcome message
	// TODO: show the "About" page from the docs here
	strMsg  = wxT("<html><body><h1>Welcome to ");
	strMsg += strVersion;
	strMsg += wxT("</h1>");
	strMsg += wxT("</body></html>");
	m_welcomeHtml->SetPage(strMsg);

	strMsg = wxT("application started");
	wxLogMessage(strMsg);

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


	file_menu = new wxMenu;
	file_menu->Append(wxID_PREFERENCES);
	file_menu->Append(wxID_EXIT);

	test_menu = new wxMenu;
	test_menu->Append(muhkuh_mainFrame_menuTestCancel,				wxT("Cancel"),					wxT("Cancel the running test"));
	test_menu->Append(muhkuh_mainFrame_menuTestRescan,				wxT("Rescan"),					wxT("Rescan the repository"));

	view_menu = new wxMenu;
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewRepositoryPane,		wxT("View Repository Selector"),		wxT("Toggle the visibility of the repository selector"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewPanicButton,		wxT("View Panic Button"),			wxT("Toggle the visibility of the panic button"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewTestTree,			wxT("View Test Tree"),				wxT("Toggle the visibility of the test tree"));
	view_menu->AppendCheckItem(muhkuh_mainFrame_menuViewMessageLog,			wxT("View Message Log"),			wxT("Toggle the visibility of the message log"));
	view_menu->Append(muhkuh_mainFrame_menuRestoreDefaultPerspective,		wxT("Restore Default Layout"),			wxT("Restore the default window layout"));

	help_menu = new wxMenu;
	help_menu->Append(wxID_HELP);
	help_menu->AppendSeparator();
	help_menu->Append(wxID_ABOUT);
	help_menu->AppendSeparator();
	help_menu->Append(muhkuh_mainFrame_menuShowTip,				wxT("Show Tip"),				wxT("Show some tips about Muhkuh"));

	m_menuBar = new wxMenuBar;
	m_menuBar->Append(file_menu, wxT("&File"));
	m_menuBar->Append(test_menu, wxT("&Test"));
	m_menuBar->Append(view_menu, wxT("&View"));
	m_menuBar->Append(help_menu, wxT("&Help"));

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
	paneInfo.Name(wxT("repository_selector")).CaptionVisible(true).Caption(wxT("Repository Selector")).Top().Position(0);
	m_auiMgr.AddPane(m_repositoryCombo, paneInfo);

	// add the cancel button
	m_buttonCancelTest = new wxButton(this, muhkuh_mainFrame_cancelTestButton_id, "Cancel Test");
	paneInfo.Name(wxT("panic_button")).CaptionVisible(true).Caption(wxT("Panic Button")).Top().Position(1);
	m_auiMgr.AddPane(m_buttonCancelTest, paneInfo);

	// create the tree
	style = wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxSUNKEN_BORDER;
	m_treeCtrl = new wxTreeCtrl(this, muhkuh_mainFrame_TestTree_Ctrl, wxDefaultPosition, wxDefaultSize, style);
	paneInfo.Name(wxT("test_tree")).CaptionVisible(true).Caption(wxT("Test Tree")).Left().Position(0);
	m_auiMgr.AddPane(m_treeCtrl, paneInfo);

	// create the auinotebook
	m_notebook = new wxAuiNotebook(this, muhkuh_mainFrame_Notebook_id);
	paneInfo.Name(wxT("notebook")).CaptionVisible(false).Center().Position(0);
	m_auiMgr.AddPane(m_notebook, paneInfo);

	// create the "welcome" html
	m_welcomeHtml = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	m_notebook->AddPage(m_welcomeHtml, wxT("Welcome"));

	// create the test details list
	style = wxLC_REPORT | wxLC_HRULES;
	m_testDetailsList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	listItem.SetText(_("Name"));
	listItem.SetImage(-1);
	listItem.SetAlign(wxLIST_FORMAT_LEFT);
	m_testDetailsList->InsertColumn(0, listItem);
	listItem.SetText(_("Version"));
	listItem.SetImage(-1);
	listItem.SetAlign(wxLIST_FORMAT_RIGHT);
	m_testDetailsList->InsertColumn(1, listItem);
	tSize.Set(16, 16);
	m_testDetailsImages = new wxImageList(tSize.GetWidth(), tSize.GetHeight(), true, 2);
	m_testDetailsImages->Add( wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_MENU, tSize) );
	m_testDetailsImages->Add( wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_MENU, tSize) );
	m_testDetailsList->AssignImageList(m_testDetailsImages, wxIMAGE_LIST_SMALL);
	m_notebook->AddPage(m_testDetailsList, wxT("Test Details"));

	style = wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY;
	m_textCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, style);
	paneInfo.Name(wxT("message_log")).CaptionVisible(true).Caption(wxT("Message Log")).Bottom().Position(0);
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
	m_tipProvider = wxCreateFileTipProvider(_T("muhkuh_tips.txt"), m_sizStartupTipsIdx);
}


void muhkuh_mainFrame::read_config(void)
{
	wxConfigBase *pConfig;
	int iMainFrameX, iMainFrameY, iMainFrameW, iMainFrameH;
	int iDetailsCol0Size;
	int iDetailsCol1Size;
	wxString strPerspective;


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
	iDetailsCol0Size = pConfig->Read(wxT("detailscol0"), wxLIST_AUTOSIZE);
	iDetailsCol1Size = pConfig->Read(wxT("detailscol1"), wxLIST_AUTOSIZE);
	strPerspective = pConfig->Read(wxT("perspective"), wxEmptyString);
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
	// set column widths
	m_testDetailsList->SetColumnWidth(0, iDetailsCol0Size);
	m_testDetailsList->SetColumnWidth(1, iDetailsCol1Size);
	if( strPerspective.IsEmpty()==false )
	{
		m_auiMgr.LoadPerspective(strPerspective,true);
	}
}


void muhkuh_mainFrame::write_config(void)
{
	wxConfigBase *pConfig;
	int iMainFrameX, iMainFrameY, iMainFrameW, iMainFrameH;
	int iDetailsCol0Size;
	int iDetailsCol1Size;
	wxString strPerspective;


	pConfig = wxConfigBase::Get();
	if( pConfig == NULL )
	{
		return;
	}

	// save the frame position
	GetPosition(&iMainFrameX, &iMainFrameY);
	GetSize(&iMainFrameW, &iMainFrameH);
	iDetailsCol0Size = m_testDetailsList->GetColumnWidth(0);
	iDetailsCol1Size = m_testDetailsList->GetColumnWidth(1);
	strPerspective = m_auiMgr.SavePerspective();

	// save tip provider data
	if( m_tipProvider!=NULL )
	{
		m_sizStartupTipsIdx = m_tipProvider->GetCurrentTip();
	}

	pConfig->SetPath(wxT("/MainFrame"));
	pConfig->Write(wxT("x"), (long)iMainFrameX);
	pConfig->Write(wxT("y"), (long)iMainFrameY);
	pConfig->Write(wxT("w"), (long)iMainFrameW);
	pConfig->Write(wxT("h"), (long)iMainFrameH);
	pConfig->Write(wxT("detailscol0"), (long)iDetailsCol0Size);
	pConfig->Write(wxT("detailscol1"), (long)iDetailsCol1Size);
	pConfig->Write(wxT("perspective"), strPerspective);
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
	// leave old state
	switch(m_state)
	{
	case muhkuh_mainFrame_state_scanning:
		break;

	case muhkuh_mainFrame_state_idle:
		break;

	case muhkuh_mainFrame_state_testing:
		fTestHasFinished = true;
		break;
	}

	// enter new state
	switch(tNewState)
	{
	case muhkuh_mainFrame_state_scanning:
		m_buttonCancelTest->Enable(false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestCancel, false);
		m_testDetailsList->Enable(false);
		m_repositoryCombo->Enable(false);
		m_treeCtrl->Enable(false);
		m_menuBar->Enable(wxID_PREFERENCES, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestRescan, false);
		break;

	case muhkuh_mainFrame_state_idle:
		m_buttonCancelTest->Enable(false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestCancel, false);
		m_testDetailsList->Enable(true);
		m_repositoryCombo->Enable(true);
		m_treeCtrl->Enable(true);
		m_menuBar->Enable(wxID_PREFERENCES, true);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestRescan, true);
		break;

	case muhkuh_mainFrame_state_testing:
		fTestHasFinished = false;
		m_buttonCancelTest->Enable(true);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestCancel, true);
		m_testDetailsList->Enable(false);
		m_repositoryCombo->Enable(false);
		m_treeCtrl->Enable(false);
		m_menuBar->Enable(wxID_PREFERENCES, false);
		m_menuBar->Enable(muhkuh_mainFrame_menuTestRescan, false);
		break;
	}

	// set new state
	m_state = tNewState;
}


void muhkuh_mainFrame::OnIdle(wxIdleEvent& event)
{
	wxString sStatus;
	int iRepositoryIndex;


	switch(m_state)
	{
	case muhkuh_mainFrame_state_scanning:
		sStatus = wxT("Scanning test descriptions...");
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
			if( iRepositoryIndex>=0 )
			{
				scanTests(iRepositoryIndex);
			}
		}

		// show the number of loaded tests
		if( m_sizTestCnt==1 )
		{
			sStatus = _("1 test loaded...");
		}
		else
		{
			sStatus.Printf(_("%d tests loaded..."), m_sizTestCnt);
		}
		break;

	case muhkuh_mainFrame_state_testing:
		// check for finish
		if( fTestHasFinished==true )
		{
			// test done -> go back to idle state
			setState(muhkuh_mainFrame_state_idle);
			finishTest();
		}
		else
		{
			sStatus = wxT("Test '");
			sStatus += m_strRunningTestName;
			sStatus += wxT("' in progress...");
		}
		break;
	}

	// set the status text
	SetStatusText(sStatus, 1);

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
		iResult = wxMessageBox(wxT("A test is still running. Are you sure you want to cancel it?"), m_strRunningTestName, wxYES_NO, this);
		if( iResult==wxYES )
		{
			wxLogMessage(wxT("Script canceled on user request!"));

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


	pAboutDialog = new muhkuh_aboutDialog(this, strVersion, frameIcons);
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
		if( iNewSelection!=wxNOT_FOUND )
		{
			scanTests(iNewSelection);
		}
	}
	else
	{
		// changes canceled -> delete the temp managers
		delete ptTmpRepositoryManager;
		delete ptTmpPluginManager;
	}

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
	wxString strErrorMessage;
	bool fExecutedSuccessfully;
	wxString strLuaSystemModulePath;
	wxString strDebug;
	wxFileName cfgName;


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
		wxLogError(wxT("failed to allocate a new lua state"));
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
		wxLogError(wxT("failed to init the muhkuh_lua bindings"));
	}

	// init the lua bindings for all plugins
	iResult = m_ptPluginManager->initLuaBindings(m_ptLuaState);
	if( iResult!=0 )
	{
		wxLogError(wxT("failed to init plugin bindings"));
		return;
	}

	// create the lua state
	fCreated = m_ptLuaState->Create(this, wxID_ANY);
	if( fCreated!=true )
	{
		wxLogError(wxT("failed to create a new lua state"));
		return;
	}

	// is the state valid?
	if( m_ptLuaState->Ok()!=true )
	{
		wxLogError(wxT("strange lua state"));
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
		wxLogError(wxT("failed to get the global 'package'"));
		return;
	}
	m_ptLuaState->lua_PushString(strLuaSystemModulePath);
	m_ptLuaState->lua_SetField(-2, wxT("path"));


	// create a new panel for the test
	m_testPanel = new wxPanel(this);
	m_notebook->AddPage(m_testPanel, m_strRunningTestName, true);

	// set some global vars

	// set the lua version
	m_ptLuaState->lua_PushString(strVersion.ToAscii());
	m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_VERSION"));
	// set the xml document
	m_ptLuaState->PushUserDataType(s_wxluatag_wxXmlDocument, ptTestData->getXmlDocument());
	m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_TEST_XML"));
	// set the selected test index
	m_ptLuaState->lua_PushNumber(uiIndex);
	m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_TEST_INDEX"));
	// set the panel
	m_ptLuaState->PushUserDataType(s_wxluatag_wxPanel, m_testPanel);
	m_ptLuaState->lua_SetGlobal(wxT("__MUHKUH_PANEL"));

	// set state to 'testing'
	// NOTE: this must be done before the call to 'RunString', or the state will not change before the first idle event
	setState(muhkuh_mainFrame_state_testing);

	// set the log marker
	luaSetLogMarker();

	iResult = m_ptLuaState->RunString(strLuaCode, wxT("system boot"));

	fExecutedSuccessfully = wxLuaState::CheckRunError(iResult, &strErrorMessage);
	if( fExecutedSuccessfully!=true )
	{
		wxLogError(strErrorMessage);
		finishTest();
		setState(muhkuh_mainFrame_state_idle);
	}
}


void muhkuh_mainFrame::finishTest(void)
{
	wxLuaBindingList *ptBindings;
	int iPanelIdx;


	wxLogMessage(wxT("Test '") + m_strRunningTestName + wxT("' finished, cleaning up..."));

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
	wxSize tRepoBitmapSize;
	wxBitmap tRepoBitmap;
	size_t sizCnt, sizMax;


	m_repositoryCombo->Clear();
	tRepoBitmapSize.Set(16, 16);
	sizMax = m_ptRepositoryManager->GetRepositoryCount();
	for(sizCnt=0; sizCnt<sizMax; ++sizCnt)
	{
		// get string representation of the new entry
		strReposEntry = m_ptRepositoryManager->GetStringRepresentation(sizCnt);
		// get bitmap for the entry
		tRepoBitmap = m_ptRepositoryManager->GetBitmap(sizCnt, tRepoBitmapSize);
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
		if( iRepositoryIdx!=wxNOT_FOUND )
		{
			scanTests(iRepositoryIdx);
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
	// restore the default perspective
	m_auiMgr.LoadPerspective(strDefaultPerspective, true);
}


void muhkuh_mainFrame::OnTestCancel(wxCommandEvent& WXUNUSED(event))
{
	int iResult;


	iResult = wxMessageBox(wxT("Are you sure you want to cancel this test?"), m_strRunningTestName, wxYES_NO, this);
	if( iResult==wxYES )
	{
		wxLogMessage(wxT("Script canceled on user request!"));

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
		strTitle = wxT("Menu for ") + m_treeCtrl->GetItemText(itemId);
		ptMenu = new wxMenu(strTitle);
		ptMenu->Append(muhkuh_mainFrame_TestTree_Execute,	wxT("&Execute..."));
		ptMenu->Append(muhkuh_mainFrame_TestTree_ShowHelp,	wxT("&Show Help..."));
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
	wxString sTestName;
	unsigned int uiSubTestCnt;
	unsigned int uiCnt;
	wxString sSubTestName;
	bool fOk;
	long lItemIdx;


	// clear the complete list
	m_testDetailsList->DeleteAllItems();

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
				lItemIdx = m_testDetailsList->GetItemCount();

				listItem.Clear();
				listItem.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE);
				listItem.SetId(lItemIdx);
				listItem.SetColumn(0);
				listItem.SetText( ptWrapXml->testDescription_getName() );
				listItem.SetImage(0);
				m_testDetailsList->InsertItem(listItem);
				m_testDetailsList->SetItem(lItemIdx, 1, ptWrapXml->testDescription_getVersion());

				// append all subitems to the list
				uiSubTestCnt = ptWrapXml->testDescription_getTestCnt();
				uiCnt = 0;
				while( uiCnt<uiSubTestCnt )
				{
					fOk = ptWrapXml->testDescription_setTest(uiCnt);
					if( fOk!=true )
					{
						wxLogError(wxT("failed to query subtest!"));
						break;
					}

					lItemIdx = m_testDetailsList->GetItemCount();

					listItem.Clear();
					listItem.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE);
					listItem.SetId(lItemIdx);
					listItem.SetColumn(0);
					listItem.SetText( ptWrapXml->test_getName() );
					listItem.SetImage(1);
					m_testDetailsList->InsertItem(listItem);
					m_testDetailsList->SetItem(lItemIdx, 1, ptWrapXml->test_getVersion());

					// next item
					++uiCnt;
				}
			}
		}
	}
}


void muhkuh_mainFrame::scanTests(int iActiveRepositoryIdx)
{
	bool bRes;
	size_t sizTestCnt;


	// set new state
	setState(muhkuh_mainFrame_state_scanning);

	// show process dialog
	m_scannerProgress = new wxProgressDialog(
		wxT("Scanning available test descriptions"),
		wxT("Please wait..."),
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
		// clear all old tests
		m_sizTestCnt = 0;
		// clear the complete tree
		m_treeCtrl->DeleteAllItems();
		// add a root item
		m_treeCtrl->AddRoot(wxT("Root"));

		// vaild directory or repository -> accept the index
		m_ptRepositoryManager->SetActiveRepository(iActiveRepositoryIdx);

		// create a new progress dialog with the correct max count
		sizTestCnt = m_ptRepositoryManager->getTestlistCount(iActiveRepositoryIdx);
		if( sizTestCnt>0 )
		{
			// show process dialog
			m_scannerProgress = new wxProgressDialog(
				wxT("Scanning available test descriptions"),
				wxT("Please wait..."),
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
		strMessage  = wxT("scanning '");
		strMessage += m_ptRepositoryManager->getTestlistPrintUrl(iActiveRepositoryIdx, sizCnt);
		strMessage += wxT("' ...");
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
	wxTreeItemId rootId;
	wxTreeItemId testId;


	// get the url for the xml file
	strXmlUrl = m_ptRepositoryManager->getTestlistXmlUrl(sizRepositoryIdx, sizTestIdx);

	// test if file exists
	wxLogVerbose(wxT("Reading testdescription ") + strXmlUrl);
	ptFsFile = fileSystem.OpenFile(strXmlUrl);
	if( ptFsFile==NULL )
	{
		wxLogError(wxT("could not read xml file '") + strXmlUrl + wxT("'"));
		return false;
	}

	// ok, file exists. read all data into a string
	ptInputStream = ptFsFile->GetStream();
	// stream read, construct xml tree
	ptWrapXml = new muhkuh_wrap_xml();
	if( ptWrapXml==NULL )
	{
		return false;
	}

	if( ptWrapXml->initialize(ptInputStream, sizRepositoryIdx, sizTestIdx)==false )
	{
		wxLogError(wxT("Failed to read the xml file for testdescription ") + strXmlUrl);
		return false;
	}

	// get the main test name
	strTestName = ptWrapXml->testDescription_getName();
	// TODO: get the help file name
	strHelpFile.Empty();
	// create the tree item object
	ptItemData = new testTreeItemData(strTestName, strHelpFile, ptWrapXml);

	// get the root item, here the test is appended
	rootId = m_treeCtrl->GetRootItem();
	// add the test item, this is always the first item
	testId = m_treeCtrl->AppendItem(rootId, ptWrapXml->testDescription_getName(), -1, -1, ptItemData);
//	// set 'opened' icon
//	SetItemImage(testId, muhkuh_testTreeIcon_TestGroupOpened, wxTreeItemIcon_Expanded);

	delete ptFsFile;

	return true;
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
			iResult = wxMessageBox(wxT("Are you sure you want to quit this test?"), m_strRunningTestName, wxYES_NO, this);
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
}


void muhkuh_mainFrame::OnLuaPrint(wxLuaEvent &event)
{
	wxLogMessage( event.GetString() );
}


void muhkuh_mainFrame::OnLuaError(wxLuaEvent &event)
{
	wxLogError( event.GetString() );
}


void muhkuh_mainFrame::luaTestHasFinished(void)
{
	fTestHasFinished = true;
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
		m_ptLuaState->terror(strMsg);
	}
	else
	{
		strFileUrl = m_ptRepositoryManager->getTestlistBaseUrl(m_sizRunningTest_RepositoryIdx, m_sizRunningTest_TestIdx) + wxT("/") + strFileName;
		wxLogMessage(wxT("lua load: searching '") + strFileUrl + wxT("'"));
		urlError = filelistUrl.SetURL(strFileUrl);
		if( urlError!=wxURL_NOERR )
		{
			// this was no valid url
			strMsg  = wxT("lua load: invalid URL '");
			strMsg += strFileUrl;
			strMsg += wxT("'\n");
			// try to show some details
			switch( urlError )
			{
			case wxURL_SNTXERR:
				strMsg += wxT("Syntax error in the URL string.");
				break;
			case wxURL_NOPROTO:
				strMsg += wxT("Found no protocol which can get this URL.");
				break;
			case wxURL_NOHOST:
				strMsg += wxT("An host name is required for this protocol.");
				break;
			case wxURL_NOPATH:
				strMsg += wxT("A path is required for this protocol.");
				break;
			case wxURL_CONNERR:
				strMsg += wxT("Connection error. (did anybody press connect? should never happen!)");
				break;
			case wxURL_PROTOERR:
				strMsg += wxT("An error occurred during negotiation. (should never happen!)");
				break;
			default:
				strMsg += wxT("unknown errorcode");
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
				strMsg.Printf(wxT("Read 0x%08X bytes"), strData.Len());
			}
			else
			{
				wxLogError(wxT("lua load: failed to read file"));
			}
			delete ptGrowBuffer;
		}
	}

	return strData;
}


void muhkuh_mainFrame::luaInclude(wxString strFileName, wxString strChunkName)
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
	int iResult;
	int iGetTop;
	int iLineNr;


	if( m_ptLuaState!=NULL )
	{
		if( strFileName.IsEmpty()==true )
		{
			// the filename parameter is invalid
			strMsg = _("lua include failed: empty filename");
			m_ptLuaState->terror(strMsg);
		}
		else
		{
			if( strChunkName.IsEmpty()==true )
			{
				// the chunkname parameter is invalid
				strMsg = _("lua include failed: empty chunkname");
				m_ptLuaState->terror(strMsg);
			}
			else
			{
				strFileUrl = m_ptRepositoryManager->getTestlistBaseUrl(m_sizRunningTest_RepositoryIdx, m_sizRunningTest_TestIdx) + strFileName;
				wxLogMessage(wxT("lua include: searching '") + strFileUrl + wxT("'"));
				urlError = filelistUrl.SetURL(strFileUrl);
				if( urlError!=wxURL_NOERR )
				{
					// this was no valid url
					strMsg  = wxT("lua include: invalid URL '");
					strMsg += strFileUrl;
					strMsg += wxT("'\n");
					// try to show some details
					switch( urlError )
					{
					case wxURL_SNTXERR:
						strMsg += wxT("Syntax error in the URL string.");
						break;
					case wxURL_NOPROTO:
						strMsg += wxT("Found no protocol which can get this URL.");
						break;
					case wxURL_NOHOST:
						strMsg += wxT("An host name is required for this protocol.");
						break;
					case wxURL_NOPATH:
						strMsg += wxT("A path is required for this protocol.");
						break;
					case wxURL_CONNERR:
						strMsg += wxT("Connection error. (did anybody press connect? should never happen!)");
						break;
					case wxURL_PROTOERR:
						strMsg += wxT("An error occurred during negotiation. (should never happen!)");
						break;
					default:
						strMsg += wxT("unknown errorcode");
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
							m_ptLuaState->lua_Call(0,0);
							break;

						case LUA_ERRSYNTAX:
							iResult = m_ptLuaState->LuaError(iResult, iGetTop, NULL, &strMsg, &iLineNr);
							strMsg.Printf(wxT("error %d in line %d"), iResult, iLineNr);
							wxLogError(strMsg);
							strMsg = _("syntax error during pre-compilation");
							m_ptLuaState->terror(strMsg);
							break;

						case LUA_ERRMEM:
							strMsg = _("memory allocation error");
							m_ptLuaState->terror(strMsg);
							break;

						default:
							strMsg.Printf(_("Unknown error message from luaL_LoadBuffer: 0x%x"), iResult);
							m_ptLuaState->terror(strMsg);
							break;
						}
					}
					else
					{
						wxLogError(wxT("lua include: failed to read file"));
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
			strMsg = wxT("failed to compile the regex pattern: ") + strPattern;
			m_ptLuaState->terror(strMsg);
		}
	}
	else
	{
		m_ptLuaState->terror(wxT("no plugin manager exists in main application"));
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

