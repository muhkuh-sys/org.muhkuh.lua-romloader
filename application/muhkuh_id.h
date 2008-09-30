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


#include "wx/wx.h"


#ifndef __MUHKUH_ID_H__
#define __MUHKUH_ID_H__

enum
{
	muhkuh_mainFrame_TestTree_Ctrl		= wxID_HIGHEST,
	muhkuh_mainFrame_TestTree_Execute,
	muhkuh_mainFrame_TestTree_ShowHelp,
	muhkuh_mainFrame_TestTree_Rescan,
	muhkuh_mainFrame_RepositoryCombo_id,
	muhkuh_mainFrame_cancelTestButton_id,
	muhkuh_mainFrame_Notebook_id,
	muhkuh_mainFrame_Debug,
	muhkuh_mainFrame_Welcome_id,
	muhkuh_mainFrame_TestDetails_id,

	muhkuh_mainFrame_menuViewRepositoryPane,
	muhkuh_mainFrame_menuViewPanicButton,
	muhkuh_mainFrame_menuViewTestTree,
	muhkuh_mainFrame_menuViewMessageLog,
	muhkuh_mainFrame_menuViewWelcomePage,
	muhkuh_mainFrame_menuViewTestDetails,

	muhkuh_mainFrame_menuShowTip,
	muhkuh_mainFrame_menuRestoreDefaultPerspective,
	muhkuh_mainFrame_menuTestCancel,
	muhkuh_mainFrame_menuTestRescan,

	muhkuh_brokenPluginDialog_ButtonDisable,
	muhkuh_brokenPluginDialog_ButtonIgnore,
	muhkuh_brokenPluginDialog_ButtonConfig,

	muhkuh_configDialog_TreeBook,
	muhkuh_configDialog_RadioWelcomeBuiltIn,
	muhkuh_configDialog_RadioWelcomeFile,
	muhkuh_configDialog_RadioDetailsBuiltIn,
	muhkuh_configDialog_RadioDetailsFile,
	muhkuh_configDialog_StartPageBrowse,
	muhkuh_configDialog_DetailsPageBrowse,
	muhkuh_configDialog_RepositoryList,
	muhkuh_configDialog_PluginList,
	muhkuh_configDialog_AddRepository,
	muhkuh_configDialog_EditRepository,
	muhkuh_configDialog_RemoveRepository,
	muhkuh_configDialog_AddPlugin,
	muhkuh_configDialog_RemovePlugin,
	muhkuh_configDialog_EnablePlugin,
	muhkuh_configDialog_DisablePlugin,
	muhkuh_configDialog_LuaPathList,
	muhkuh_configDialog_LuaAddPath,
	muhkuh_configDialog_LuaDeletePath,
	muhkuh_configDialog_LuaEditPath,
	muhkuh_configDialog_LuaMovePathUp,
	muhkuh_configDialog_LuaMovePathDown,
	muhkuh_configAutostart_CheckboxAutostart,
	muhkuh_configAutostart_BrowseTests,
	muhkuh_configAutostart_CheckboxAutoexit,
	muhkuh_configReposEntryDialog_radioDirscan,
	muhkuh_configReposEntryDialog_radioFilelist,
	muhkuh_configReposEntryDialog_radioSingleXml,
	muhkuh_configReposEntryDialog_radioAllLocal,
	muhkuh_configReposEntryDialog_ButtonDirscanLocationBrowse,
	muhkuh_configReposEntryDialog_ButtonSingleXmlLocationBrowse,
	muhkuh_configReposEntryDialog_ButtonAllLocalLocationBrowse,
	muhkuh_configReposEntryDialog_CheckboxDirscanRelPaths,
	muhkuh_configReposEntryDialog_CheckboxSingleXmlRelPaths,

	muhkuh_dirlistbox_BrowseButton,
	muhkuh_dirlistbox_TextCtrl,

	muhkuh_debugger_notebook,
	muhkuh_debugger_buttonStepInto,
	muhkuh_debugger_buttonStepOver,
	muhkuh_debugger_buttonStepOut,
	muhkuh_debugger_buttonContinue,
	muhkuh_debugger_buttonBreak,
	muhkuh_debugger_stackList,

	muhkuh_scanner_thread_event,
	muhkuh_tester_thread_event,
	muhkuh_mainFrame_testFinish_event,

	muhkuh_debugServerSocket_event,
	muhkuh_debugConnectionSocket_event,

	muhkuh_serverProcess_terminate,
};

#endif	// __MUHKUH_ID_H__
