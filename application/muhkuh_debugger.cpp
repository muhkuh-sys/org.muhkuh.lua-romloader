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


#include "muhkuh_debugger.h"


#include <wx/file.h>
#include <wx/filename.h>


#include "muhkuh_debug_messages.h"
#include "muhkuh_icons.h"
#include "muhkuh_id.h"


BEGIN_EVENT_TABLE(muhkuh_debugger, wxPanel)
	EVT_SOCKET(muhkuh_debugServerSocket_event,			muhkuh_debugger::OnDebugServerSocket)
	EVT_SOCKET(muhkuh_debugConnectionSocket_event,			muhkuh_debugger::OnDebugConnectionSocket)

	EVT_TOOL(muhkuh_debugger_buttonStepInto,			muhkuh_debugger::OnDebuggerStepInto)
	EVT_TOOL(muhkuh_debugger_buttonStepOver,			muhkuh_debugger::OnDebuggerStepOver)
	EVT_TOOL(muhkuh_debugger_buttonStepOut,				muhkuh_debugger::OnDebuggerStepOut)
	EVT_TOOL(muhkuh_debugger_buttonContinue,			muhkuh_debugger::OnDebuggerContinue)
	EVT_TOOL(muhkuh_debugger_buttonBreak,				muhkuh_debugger::OnDebuggerBreak)

	EVT_AUINOTEBOOK_PAGE_CLOSE(muhkuh_debugger_notebook,		muhkuh_debugger::OnNotebookPageClose)
END_EVENT_TABLE()


const char *muhkuh_debugger::acSyntaxhl0 =
{
	"and "
	"break "
	"do "
	"else "
	"elseif "
	"end "
	"false "
	"for "
	"function "
	"if "
	"in "
	"local "
	"nil "
	"not "
	"or "
	"repeat "
	"return "
	"then "
	"true "
	"until "
	"while"
};


const char *muhkuh_debugger::acSyntaxhl1 =
{
	"_VERSION "
	"assert "
	"collectgarbage "
	"dofile "
	"error "
	"gcinfo "
	"loadfile "
	"loadstring "
	"print "
	"rawget "
	"rawset "
	"require "
	"tonumber "
	"tostring "
	"type "
	"unpack"
};


const char *muhkuh_debugger::acSyntaxhl2 =
{
	"_G "
	"getfenv "
	"getmetatable "
	"ipairs "
	"loadlib "
	"next "
	"pairs "
	"pcall "
	"rawequal "
	"setfenv "
	"setmetatable "
	"xpcall "
	"string "
	"table "
	"math "
	"coroutine "
	"io "
	"os "
	"debug "
	"load "
	"module "
	"select"
};


const char *muhkuh_debugger::acSyntaxhl3 =
{
	"string.byte "
	"string.char "
	"string.dump "
	"string.find "
	"string.len "
	"string.lower "
	"string.rep "
	"string.sub "
	"string.upper "
	"string.format "
	"string.gfind "
	"string.gsub "
	"table.concat "
	"table.foreach "
	"table.foreachi "
	"table.getn "
	"table.sort "
	"table.insert "
	"table.remove "
	"table.setn "
	"math.abs "
	"math.acos "
	"math.asin "
	"math.atan "
	"math.atan2 "
	"math.ceil "
	"math.cos "
	"math.deg "
	"math.exp "
	"math.floor "
	"math.frexp "
	"math.ldexp "
	"math.log "
	"math.log10 "
	"math.max "
	"math.min "
	"math.mod "
	"math.pi "
	"math.pow "
	"math.rad "
	"math.random "
	"math.randomseed "
	"math.sin "
	"math.sqrt "
	"math.tan "
	"string.gmatch "
	"string.match "
	"string.reverse "
	"table.maxn "
	"math.cosh "
	"math.fmod "
	"math.modf "
	"math.sinh "
	"math.tanh "
	"math.huge"
};


const char *muhkuh_debugger::acSyntaxhl4 =
{
	"coroutine.create "
	"coroutine.resume "
	"coroutine.status "
	"coroutine.wrap "
	"coroutine.yield "
	"io.close "
	"io.flush "
	"io.input "
	"io.lines "
	"io.open "
	"io.output "
	"io.read "
	"io.tmpfile "
	"io.type "
	"io.write "
	"io.stdin "
	"io.stdout "
	"io.stderr "
	"os.clock "
	"os.date "
	"os.difftime "
	"os.execute "
	"os.exit "
	"os.getenv "
	"os.remove "
	"os.rename "
	"os.setlocale "
	"os.time "
	"os.tmpname "
	"coroutine.running "
	"package.cpath "
	"package.loaded "
	"package.loadlib "
	"package.path "
	"package.preload "
	"package.seeall "
	"io.popen "
	"debug.debug "
	"debug.getfenv "
	"debug.gethook "
	"debug.getinfo "
	"debug.getlocal "
	"debug.getmetatable "
	"debug.getregistry "
	"debug.getupvalue "
	"debug.setfenv "
	"debug.sethook "
	"debug.setlocal "
	"debug.setmetatable "
	"debug.setupvalue "
	"debug.traceback"
};






muhkuh_debugger::muhkuh_debugger(wxWindow *ptParent, wxString &strApplicationPath, unsigned short usDebugServerPort, muhkuh_wrap_xml *ptXml)
 : wxPanel(ptParent)
 , m_strApplicationPath(strApplicationPath)
 , m_usDebugServerPort(usDebugServerPort)
 , m_ptXml(ptXml)
 , m_ptDebugSocketServer(NULL)
 , m_ptDebugConnection(NULL)
{
	create_controls();
	create_socket_server();
}


muhkuh_debugger::~muhkuh_debugger(void)
{
}


void muhkuh_debugger::create_socket_server(void)
{
	wxIPV4address tIpAdr;


	// create the debug socket server
	if( tIpAdr.AnyAddress()==false )
	{
		wxLogError(_("failed to set local addr!"));
	}
	if( tIpAdr.Service(m_usDebugServerPort)==false )
	{
		wxLogError(_("failed to set service port!"));
	}
	m_ptDebugSocketServer = new wxSocketServer(tIpAdr, wxSOCKET_WAITALL|wxSOCKET_REUSEADDR);
	if( m_ptDebugSocketServer!=NULL && m_ptDebugSocketServer->IsOk()==true )
	{
		m_ptDebugSocketServer->SetEventHandler(*this, muhkuh_debugServerSocket_event);
		m_ptDebugSocketServer->SetNotify(wxSOCKET_CONNECTION_FLAG);
		m_ptDebugSocketServer->Notify(true);
	}
}


void muhkuh_debugger::delete_socket_server(void)
{
	// close the debug server socket
	if( m_ptDebugSocketServer!=NULL )
	{
		m_ptDebugSocketServer->Destroy();
		m_ptDebugSocketServer = NULL;
	}
	if( m_ptDebugConnection!=NULL )
	{
		m_ptDebugConnection->Destroy();
		m_ptDebugConnection = NULL;
	}
}


void muhkuh_debugger::OnDebuggerStepInto(wxCommandEvent &event)
{
	if( m_ptDebugConnection!=NULL && m_ptDebugConnection->IsConnected()==true )
	{
		dbg_write_u08(MUHDBG_CmdStepInto);
	}
}


void muhkuh_debugger::OnDebuggerStepOver(wxCommandEvent &event)
{
	if( m_ptDebugConnection!=NULL && m_ptDebugConnection->IsConnected()==true )
	{
		dbg_write_u08(MUHDBG_CmdStepOver);
	}
}


void muhkuh_debugger::OnDebuggerStepOut(wxCommandEvent &event)
{
	if( m_ptDebugConnection!=NULL && m_ptDebugConnection->IsConnected()==true )
	{
		dbg_write_u08(MUHDBG_CmdStepOut);
	}
}


void muhkuh_debugger::OnDebuggerContinue(wxCommandEvent &event)
{
	if( m_ptDebugConnection!=NULL && m_ptDebugConnection->IsConnected()==true )
	{
		dbg_write_u08(MUHDBG_CmdContinue);
	}
}


void muhkuh_debugger::OnDebuggerBreak(wxCommandEvent &event)
{
	if( m_ptDebugConnection!=NULL && m_ptDebugConnection->IsConnected()==true )
	{
		dbg_write_u08(MUHDBG_CmdBreak);
	}
}


void muhkuh_debugger::OnNotebookPageClose(wxAuiNotebookEvent &event)
{
	int iSelection;
	wxWindow *ptWin;
	tMuhkuhDocumentHash::iterator it;


	// get the selected Page
	iSelection = event.GetSelection();
	ptWin = m_ptDebugSourceNotebook->GetPage(iSelection);

	// loop over all sources
	it = m_docHash.begin();
	while( it!=m_docHash.end() )
	{
		if( it->second.m_ptEditor==ptWin )
		{
			m_docHash.erase(it);
			wxLogMessage("delete editor %p", ptWin);
			break;
		}
		++it;
	}
}


void muhkuh_debugger::create_controls(void)
{
	wxAuiPaneInfo paneInfo;
	long lStyle;


	// create the aui manager
	m_ptDebugAuiManager = new wxAuiManager(this);

	// create the debug toolbar
	m_ptDebugToolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxTB_HORIZONTAL|wxNO_BORDER|wxTB_TEXT);
	m_ptDebugToolBar->SetToolBitmapSize( wxSize(16,16) );
	m_ptDebugToolBar->AddTool(muhkuh_debugger_buttonStepInto, _("Step Into"), muhkuh_016_xpm, wxNullBitmap, wxITEM_NORMAL, _("Step Into the next statement"));
	m_ptDebugToolBar->AddTool(muhkuh_debugger_buttonStepOver, _("Step Over"), muhkuh_016_xpm, wxNullBitmap, wxITEM_NORMAL, _("Step Over the next statement"));
	m_ptDebugToolBar->AddTool(muhkuh_debugger_buttonStepOut,  _("Step Out"), muhkuh_016_xpm, wxNullBitmap, wxITEM_NORMAL, _("Step Out of the current function"));
	m_ptDebugToolBar->AddTool(muhkuh_debugger_buttonContinue, _("Continue"), muhkuh_016_xpm, wxNullBitmap, wxITEM_NORMAL, _("Continue execution"));
	m_ptDebugToolBar->AddTool(muhkuh_debugger_buttonBreak,    _("Break"), muhkuh_016_xpm, wxNullBitmap, wxITEM_NORMAL, _("Interrupt a running program"));
	m_ptDebugToolBar->Realize();
	paneInfo.Name(wxT("toolbar"));
	paneInfo.CaptionVisible(true);
	paneInfo.Caption(_("Debugger Tools"));
	paneInfo.Top();
	paneInfo.Position(0);
	paneInfo.BestSize( m_ptDebugToolBar->GetBestSize() );
	paneInfo.Resizable(false);
	m_ptDebugAuiManager->AddPane(m_ptDebugToolBar, paneInfo);

	// add the source notebook
	m_ptDebugSourceNotebook = new wxAuiNotebook(this, muhkuh_debugger_notebook);
	paneInfo.Name(wxT("source"));
	paneInfo.CaptionVisible(false);
	paneInfo.Center();
	paneInfo.Position(0);
	paneInfo.Resizable(true);
	m_ptDebugAuiManager->AddPane(m_ptDebugSourceNotebook, paneInfo);

	// add the stack window
	lStyle = wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_SINGLE_SEL|wxLC_HRULES;
	m_ptDebugStackWindow = new wxListCtrl(this, muhkuh_debugger_stackList, wxDefaultPosition, wxDefaultSize, lStyle);
	m_ptDebugStackWindow->InsertColumn(0, _("Source File"), wxLIST_FORMAT_LEFT, -1);
	m_ptDebugStackWindow->InsertColumn(1, _("Function"), wxLIST_FORMAT_LEFT, -1);
	m_ptDebugStackWindow->InsertColumn(2, _("Line"), wxLIST_FORMAT_RIGHT, -1);
	paneInfo.Name(wxT("stack"));
	paneInfo.CaptionVisible(true);
	paneInfo.Caption(_("Stack"));
	paneInfo.Bottom();
	paneInfo.Position(0);
	paneInfo.Resizable(true);
	m_ptDebugAuiManager->AddPane(m_ptDebugStackWindow, paneInfo);

	// add the watch window
	lStyle = wxTR_HAS_BUTTONS | wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT | wxTR_HIDE_ROOT | wxTR_SINGLE | wxTR_HAS_VARIABLE_ROW_HEIGHT;
	m_ptDebugWatchWindow = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
	paneInfo.Name(wxT("watch"));
	paneInfo.CaptionVisible(true);
	paneInfo.Caption(_("Watch"));
	paneInfo.Bottom();
	paneInfo.Position(1);
	paneInfo.Resizable(true);
	m_ptDebugAuiManager->AddPane(m_ptDebugWatchWindow, paneInfo);

	m_ptDebugAuiManager->Update();
}


wxStyledTextCtrl *muhkuh_debugger::create_debugger_editor(wxString strCaption)
{
	wxStyledTextCtrl *ptEditor;
//	int iCnt;
	const wxFont *ptFont;
	const wxFont *ptFontItalic;


	ptFont = wxNORMAL_FONT;
	ptFontItalic = wxITALIC_FONT;

	ptEditor = new wxStyledTextCtrl(this);

	ptEditor->SetBufferedDraw(true);
	ptEditor->StyleClearAll();

//	ptEditor->SetFont(*ptFont);
//	ptEditor->StyleSetFont(wxSTC_STYLE_DEFAULT, *ptFont);
//	for(iCnt=0; iCnt<33; ++iCnt)
//	{
//		ptEditor->StyleSetFont(iCnt, ptFont);
//	}

	ptEditor->StyleSetForeground(0,  wxColour(128, 128, 128));	// White space
	ptEditor->StyleSetForeground(1,  wxColour(0,   127, 0));	// Block Comment
//	ptEditor->StyleSetFont(1, ptFontItalic);
	// ptEditor->StyleSetUnderline(1, false);
	ptEditor->StyleSetForeground(2,  wxColour(0,   127, 0));	// Line Comment
//	ptEditor->StyleSetFont(2, m_fontItalic);			// Doc. Comment
	// ptEditor->StyleSetUnderline(2, false);
	ptEditor->StyleSetForeground(3,  wxColour(127, 127, 127));	// Number
	ptEditor->StyleSetForeground(4,  wxColour(0,   127, 127));	// Keyword
	ptEditor->StyleSetForeground(5,  wxColour(0,   0,   127));	// Double quoted string
	ptEditor->StyleSetBold(5,  true);
	// ptEditor->StyleSetUnderline(5, false);
	ptEditor->StyleSetForeground(6,  wxColour(127, 0,   127));	// Single quoted string
	ptEditor->StyleSetForeground(7,  wxColour(127, 0,   127));	// not used
	ptEditor->StyleSetForeground(8,  wxColour(0,   127, 127));	// Literal strings
	ptEditor->StyleSetForeground(9,  wxColour(127, 127, 0));	// Preprocessor
	ptEditor->StyleSetForeground(10, wxColour(0,   0,   0));	// Operators
	// ptEditor->StyleSetBold(10, true);
	ptEditor->StyleSetForeground(11, wxColour(0,   0,   0));	// Identifiers
	ptEditor->StyleSetForeground(12, wxColour(0,   0,   0));	// Unterminated strings
	ptEditor->StyleSetBackground(12, wxColour(224, 192, 224));
	ptEditor->StyleSetBold(12, true);
	ptEditor->StyleSetEOLFilled(12, true);

	ptEditor->StyleSetForeground(13, wxColour(0,   0,  95));	// Keyword 2 highlighting styles
	ptEditor->StyleSetForeground(14, wxColour(0,   95, 0));		// Keyword 3
	ptEditor->StyleSetForeground(15, wxColour(127, 0,  0));		// Keyword 4
	ptEditor->StyleSetForeground(16, wxColour(127, 0,  95));	// Keyword 5
	ptEditor->StyleSetForeground(17, wxColour(35,  95, 175));	// Keyword 6
	ptEditor->StyleSetForeground(18, wxColour(0,   127, 127));	// Keyword 7
	ptEditor->StyleSetBackground(18, wxColour(240, 255, 255));	// Keyword 8

	ptEditor->StyleSetForeground(19, wxColour(0,   127, 127));
	ptEditor->StyleSetBackground(19, wxColour(224, 255, 255));
	ptEditor->StyleSetForeground(20, wxColour(0,   127, 127));
	ptEditor->StyleSetBackground(20, wxColour(192, 255, 255));
	ptEditor->StyleSetForeground(21, wxColour(0,   127, 127));
	ptEditor->StyleSetBackground(21, wxColour(176, 255, 255));
	ptEditor->StyleSetForeground(22, wxColour(0,   127, 127));
	ptEditor->StyleSetBackground(22, wxColour(160, 255, 255));
	ptEditor->StyleSetForeground(23, wxColour(0,   127, 127));
	ptEditor->StyleSetBackground(23, wxColour(144, 255, 255));
	ptEditor->StyleSetForeground(24, wxColour(0,   127, 127));
	ptEditor->StyleSetBackground(24, wxColour(128, 155, 255));

	ptEditor->StyleSetForeground(32, wxColour(224, 192, 224));	// Line number
	ptEditor->StyleSetBackground(33, wxColour(192, 192, 192));	// Brace highlight
	ptEditor->StyleSetForeground(34, wxColour(0,   0,   255));
	ptEditor->StyleSetBold(34, true);				// Brace incomplete highlight
	ptEditor->StyleSetForeground(35, wxColour(255, 0,   0));
	ptEditor->StyleSetBold(35, true);				// Indentation guides
	ptEditor->StyleSetForeground(37, wxColour(192, 192, 192));
	ptEditor->StyleSetBackground(37, wxColour(255, 255, 255));

	ptEditor->SetUseTabs(true);
	ptEditor->SetTabWidth(8);
	ptEditor->SetIndent(8);
	ptEditor->SetIndentationGuides(true);

	ptEditor->SetVisiblePolicy(wxSTC_VISIBLE_SLOP, 3);
	// ptEditor->SetXCaretPolicy(wxSTC_CARET_SLOP, 10);
	// ptEditor->SetYCaretPolicy(wxSTC_CARET_SLOP, 3);

	ptEditor->SetMarginWidth(0, ptEditor->TextWidth(32, "99999_"));	// line # margin

	ptEditor->SetMarginWidth(1, 16);				// marker margin
	ptEditor->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
	ptEditor->SetMarginSensitive(1, true);

	ptEditor->MarkerDefine(DBGEDIT_Marker_BreakPoint,	wxSTC_MARK_ROUNDRECT, *wxWHITE, *wxRED);
	ptEditor->MarkerDefine(DBGEDIT_Marker_CurrentLine,	wxSTC_MARK_ARROW,     *wxBLACK, *wxGREEN);

	ptEditor->SetMarginWidth(2, 16);				// fold margin
	ptEditor->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
	ptEditor->SetMarginMask(2, wxSTC_MASK_FOLDERS);
	ptEditor->SetMarginSensitive(2, true);

	ptEditor->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED|wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);

	ptEditor->SetProperty("fold", "1");
	ptEditor->SetProperty("fold.compact", "1");
	ptEditor->SetProperty("fold.comment", "1");

	ptEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,	wxSTC_MARK_BOXMINUS,		0x00ffffff, 0x00808080);
	ptEditor->MarkerDefine(wxSTC_MARKNUM_FOLDER,		wxSTC_MARK_BOXPLUS,		0x00ffffff, 0x00808080);
	ptEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,		wxSTC_MARK_VLINE,		0x00ffffff, 0x00808080);
	ptEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,	wxSTC_MARK_LCORNER,		0x00ffffff, 0x00808080);
	ptEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND,		wxSTC_MARK_BOXPLUSCONNECTED,	0x00ffffff, 0x00808080);
	ptEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID,	wxSTC_MARK_BOXMINUSCONNECTED,	0x00ffffff, 0x00808080);
	ptEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL,	wxSTC_MARK_TCORNER,		0x00ffffff, 0x00808080);

	editor_set_syntaxhl(ptEditor);

	m_ptDebugSourceNotebook->AddPage(ptEditor, strCaption, false);

	return ptEditor;
}


void muhkuh_debugger::editor_set_syntaxhl(wxStyledTextCtrl *ptEditor)
{
	ptEditor->SetLexer(wxSTC_LEX_LUA);

	ptEditor->SetKeyWords(0, acSyntaxhl0);
	ptEditor->SetKeyWords(1, acSyntaxhl1);
	ptEditor->SetKeyWords(2, acSyntaxhl2);
	ptEditor->SetKeyWords(3, acSyntaxhl3);
	ptEditor->SetKeyWords(4, acSyntaxhl4);
/*
        -- Get the items in the global "wx" table for autocompletion
        if not wxkeywords then
            local keyword_table = {}
            for index, value in pairs(wx) do
                table.insert(keyword_table, "wx."..index.." ")
            end

            table.sort(keyword_table)
            wxkeywords = table.concat(keyword_table)
        end

        editor:SetKeyWords(5, wxkeywords)
*/
	ptEditor->Colourise(0, -1);
}


bool muhkuh_debugger::editor_get_source_document(wxString strSource, muhkuh_debugger_document *ptDoc)
{
	bool fResult;
	wxString strSourceFile;
	wxString strTestIndex;
	unsigned long ulTestIdx;
	wxString strFullName;
	wxFileName tFileName;
	wxFile tFile;


	// expect success
	fResult = true;

	if( strSource.StartsWith(wxT("@"), &strSourceFile)==true )
	{
		// test for next '@'
		if( strSourceFile.StartsWith(wxT("@"), &strTestIndex)==true )
		{
			// this is the index to a code section in the test description

			// TODO:build a suitable name for the tab
			ptDoc->m_strSourceFileName = strSource;

			// get the source code from the xml wrapper
			if( strTestIndex.ToULong(&ulTestIdx)==true )
			{
				if( ulTestIdx==0 )
				{
					ptDoc->m_strSourceCode = m_ptXml->testDescription_getCode();
				}
				else if( m_ptXml->testDescription_setTest(ulTestIdx)==true )
				{
					ptDoc->m_strSourceCode = m_ptXml->test_getCode();
				}
			}
		}
		else
		{
			// use the filename as source file name
			ptDoc->m_strSourceFileName = strSourceFile;

			// this is a real file, get the absolute path
			tFileName.Assign(strSourceFile);
			if( tFileName.IsAbsolute(wxPATH_NATIVE)==false )
			{
				tFileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE);
			}
			if( tFileName.FileExists()==true && tFileName.IsFileReadable()==true )
			{
				strFullName = tFileName.GetFullPath(wxPATH_NATIVE);

				// read the complete file
				if( tFile.Open(strFullName, wxFile::read)==true )
				{
					char *pucData;
					wxFileOffset fileSize;

					fileSize = tFile.Length();
					if( fileSize>0 )
					{
						pucData = new char[fileSize];
						if( tFile.Read(pucData, fileSize)==fileSize )
						{
							ptDoc->m_strSourceCode = wxString::From8BitData(pucData, fileSize);
						}
						else
						{
							ptDoc->m_strSourceCode = "Read error, failed to read file!!!";
							fResult = false;
						}
						delete[] pucData;
					}
				}
				else
				{
					ptDoc->m_strSourceCode = "Failed to open file!!!";
					fResult = false;
				}
			}
			else
			{
				ptDoc->m_strSourceCode = "Whaa, no source code found!!!";
				fResult = false;
			}
		}
	}
	else
	{
		// this is a chunk

		// no filename
		ptDoc->m_strSourceFileName = _("chunk");

		// the sourcecode
		ptDoc->m_strSourceCode = strSource;
	}

	return fResult;
}


wxStyledTextCtrl *muhkuh_debugger::editor_get_document(wxString strSource)
{
	wxString strHash;
	wxStyledTextCtrl *ptEditor;
	muhkuh_debugger_document tDoc;
	bool fResult;
	tMuhkuhDocumentHash::iterator it;


	// is the document already open?
	it = m_docHash.find(strSource);
	if( it==m_docHash.end() )
	{
		// no -> create a new document
		tDoc.m_ptEditor = NULL;
		tDoc.m_strHash = strSource;

		fResult = editor_get_source_document(strSource, &tDoc);
		if( fResult==true )
		{
			// create the editor
			ptEditor = create_debugger_editor(tDoc.m_strSourceFileName);
			if( ptEditor!=NULL )
			{
				ptEditor->AppendText(tDoc.m_strSourceCode);
				tDoc.m_ptEditor = ptEditor;

				wxLogMessage("created new editor %p with caption %s", ptEditor, tDoc.m_strSourceFileName.fn_str());
			}
		}

		// insert the new document in the hash
		m_docHash[strSource] = tDoc;
	}
	else
	{
		ptEditor = it->second.m_ptEditor;
	}

	return ptEditor;
}


void muhkuh_debugger::OnDebugServerSocket(wxSocketEvent &event)
{
	wxSocketNotify tEventTyp;


	tEventTyp = event.GetSocketEvent();
	if( tEventTyp==wxSOCKET_CONNECTION && m_ptDebugSocketServer!=NULL && m_ptDebugConnection==NULL )
	{
		// accept the socket connection
		m_ptDebugConnection = m_ptDebugSocketServer->Accept(false);
		if( m_ptDebugConnection!=NULL )
		{
			m_ptDebugConnection->SetEventHandler(*this, muhkuh_debugConnectionSocket_event);
			m_ptDebugConnection->SetNotify(wxSOCKET_INPUT_FLAG|wxSOCKET_LOST_FLAG);
			m_ptDebugConnection->Notify(true);

			// connection established!
			wxLogMessage(_("connected to debug client!"));
		}
	}
}


void muhkuh_debugger::OnDebugConnectionSocket(wxSocketEvent &event)
{
	wxSocketNotify tEventTyp;
	unsigned char ucPacketTyp;


	tEventTyp = event.GetSocketEvent();
	switch( tEventTyp )
	{
	case wxSOCKET_INPUT:
		wxLogMessage(_("wxSOCKET_INPUT"));

		m_ptDebugConnection->SetNotify(wxSOCKET_LOST_FLAG);

		m_ptDebugConnection->Read(&ucPacketTyp, 1);
		if( m_ptDebugConnection->LastCount()==1 )
		{
			switch(ucPacketTyp)
			{
			case MUHDBG_InterpreterHalted:
				dbg_packet_InterpreterHalted();
				break;

			default:
				wxLogError(_("received strange debug packet: 0x%02x"), ucPacketTyp);
				break;
			}
		}
		m_ptDebugConnection->SetNotify(wxSOCKET_LOST_FLAG|wxSOCKET_INPUT_FLAG);
		break;

	case wxSOCKET_LOST:
		wxLogMessage(wxT("wxSOCKET_LOST"));
//		finishTest();
//		setState(muhkuh_mainFrame_state_idle);
		break;
	}
}


bool muhkuh_debugger::dbg_read_u08(unsigned char *pucData)
{
	wxUint32 u32LastRead;
	bool fOk;


	fOk = false;

	m_ptDebugConnection->Read(pucData, sizeof(unsigned char));
	u32LastRead = m_ptDebugConnection->LastCount();
	if( u32LastRead==sizeof(unsigned char) )
	{
		fOk = true;
	}

	return fOk;
}


bool muhkuh_debugger::dbg_read_string(wxString &strData)
{
	wxUint32 u32LastRead;
	size_t sizLen;
	char *pcBuf;
	bool fOk;


	fOk = false;

	strData.Empty();

	m_ptDebugConnection->Read(&sizLen, sizeof(sizLen));
	u32LastRead = m_ptDebugConnection->LastCount();
	if( u32LastRead==sizeof(sizLen) )
	{
		if( sizLen==0 )
		{
			fOk = true;
		}
		else
		{
			pcBuf = new char[sizLen];
			m_ptDebugConnection->Read(pcBuf, sizLen);
			u32LastRead = m_ptDebugConnection->LastCount();
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


bool muhkuh_debugger::dbg_read_int(int *piData)
{
	wxUint32 u32LastRead;
	bool fOk;


	fOk = false;

	m_ptDebugConnection->Read(piData, sizeof(int));
	u32LastRead = m_ptDebugConnection->LastCount();
	if( u32LastRead==sizeof(int) )
	{
		fOk = true;
	}

	return fOk;
}


void muhkuh_debugger::dbg_write_u08(unsigned char ucData)
{
	m_ptDebugConnection->Write(&ucData, 1);
}


void muhkuh_debugger::dbg_write_int(int iData)
{
	m_ptDebugConnection->Write(&iData, sizeof(int));
}


void muhkuh_debugger::dbg_packet_InterpreterHalted(void)
{
	unsigned char ucStatus;
	wxString strName;
	wxString strNameWhat;
	wxString strWhat;
	wxString strSource;
	int iCurrentLine;
	int iNUps;
	int iLineDefined;
	int iLastLineDefined;
	wxStyledTextCtrl *ptEditor;
	tMuhkuhDocumentHash::iterator it;
	size_t sizPageIdx;


	if( dbg_read_u08(&ucStatus)==true )
	{
		if( ucStatus==0 )
		{
			if(	dbg_read_string(strName)==true &&
				dbg_read_string(strNameWhat)==true &&
				dbg_read_string(strWhat)==true &&
				dbg_read_string(strSource)==true &&
				dbg_read_int(&iCurrentLine)==true &&
				dbg_read_int(&iNUps)==true &&
				dbg_read_int(&iLineDefined)==true &&
				dbg_read_int(&iLastLineDefined)==true
			)
			{
				wxLogMessage(_("Debug: %s:%s:%s:%s:%d:%d:%d:%d"), strName.fn_str(), strNameWhat.fn_str(), strWhat.fn_str(), strSource.fn_str(), iCurrentLine, iNUps, iLineDefined, iLastLineDefined);

				// clear all "current line" markers
				it = m_docHash.begin();
				while( it!=m_docHash.end() )
				{
					ptEditor = it->second.m_ptEditor;
					ptEditor->MarkerDeleteAll(DBGEDIT_Marker_CurrentLine);
					++it;
				}

				ptEditor = editor_get_document(strSource);
				if( ptEditor!=NULL )
				{
					// show the source
					sizPageIdx = m_ptDebugSourceNotebook->GetPageIndex(ptEditor);
					if( sizPageIdx!=wxNOT_FOUND )
					{
						m_ptDebugSourceNotebook->SetSelection(sizPageIdx);
					}

					if( iCurrentLine>0 )
					{
						ptEditor->MarkerAdd(iCurrentLine-1, DBGEDIT_Marker_CurrentLine);
						ptEditor->GotoLine(iCurrentLine-1);
					}
				}

				dbg_fillStackWindow();
				dbg_fillWatchWindow();
			}
			else
			{
				wxLogError(_("Error reading the InterpreterHalted packet"));
			}
		}
		else
		{
			wxLogError(_("status is not ok!"));
		}
	}
	else
	{
		wxLogError(_("failed to read status"));
	}
}


void muhkuh_debugger::dbg_fillStackWindow(void)
{
	unsigned char ucState;
	wxString strName;
	wxString strNameWhat;
	wxString strWhat;
	wxString strSource;
	int iCurrentLine;
	int iNUps;
	int iLineDefined;
	int iLastLineDefined;
	int iStackCnt;
	long lListCnt;
	wxString strItem;
	wxListItem tFunctionItem;
	wxListItem tLineItem;
	long lIdx;


	// clear all elements in the list ctrl
	m_ptDebugStackWindow->DeleteAllItems();

	tFunctionItem.Clear();
//	tFunctionItem.SetMask(wxLIST_MASK_TEXT|wxLIST_MASK_FORMAT);
	tFunctionItem.SetAlign(wxLIST_FORMAT_LEFT);
	tFunctionItem.SetColumn(1);

	tLineItem.Clear();
//	tLineItem.SetMask(wxLIST_MASK_TEXT|wxLIST_MASK_FORMAT);
	tLineItem.SetAlign(wxLIST_FORMAT_RIGHT);
	tLineItem.SetColumn(2);

	iStackCnt	= 0;
	lListCnt	= 0;

	do
	{
		dbg_write_u08(MUHDBG_CmdGetStack);
		dbg_write_int(iStackCnt);
		if( dbg_read_u08(&ucState)==true )
		{
			if( ucState==0 )
			{
				if(	dbg_read_string(strName)==true &&
					dbg_read_string(strNameWhat)==true &&
					dbg_read_string(strWhat)==true &&
					dbg_read_string(strSource)==true &&
					dbg_read_int(&iCurrentLine)==true &&
					dbg_read_int(&iNUps)==true &&
					dbg_read_int(&iLineDefined)==true &&
					dbg_read_int(&iLastLineDefined)==true
				)
				{
					wxLogMessage(_("Debug: Fill Stack window: %s:%s:%s:%s:%d:%d:%d:%d"), strName.fn_str(), strNameWhat.fn_str(), strWhat.fn_str(), strSource.fn_str(), iCurrentLine, iNUps, iLineDefined, iLastLineDefined);

					if( strWhat.Cmp(wxT("Lua"))==0 || strWhat.Cmp(wxT("main"))==0 )
					{
						// insert item
						if( strName.IsEmpty()==true )
						{
							strItem = _("chunk");
						}
						else
						{
							strItem = strName;
						}
						lIdx = m_ptDebugStackWindow->InsertItem(lListCnt, strItem);

						if( strName.IsEmpty()==true )
						{
							tFunctionItem.SetText(_("no function name"));
							tFunctionItem.SetTextColour(*wxLIGHT_GREY);
							tFunctionItem.SetFont(*wxITALIC_FONT);
						}
						else
						{
							tFunctionItem.SetText(strName);
							tFunctionItem.SetTextColour(*wxBLACK);
							tFunctionItem.SetFont(*wxNORMAL_FONT);
						}
						tFunctionItem.SetId(lIdx);
						m_ptDebugStackWindow->SetItem(tFunctionItem);

						strItem.Printf("%d", iCurrentLine);
						tLineItem.SetText(strItem);
						tLineItem.SetId(lIdx);
						m_ptDebugStackWindow->SetItem(tLineItem);

						++lListCnt;
					}
					++iStackCnt;
				}
				else
				{
					// error, failed to reveice data!
					break;
				}
			}
			else
			{
				// status is not 0 -> either error or end of list
				break;
			}
		}
		else
		{
			// error, failed to receive status
			break;
		}
	// limit to max. 100 stack entries
	} while( lListCnt<100 );

	// TODO: if lListCnt is >=100, show message "display more frames"
}


void muhkuh_debugger::dbg_fillWatchWindow(void)
{
	wxTreeItemId tRootId;
	wxString strName;
	wxString strTyp;
	wxString strValue;


	// clear all elements in the tree ctrl
	m_ptDebugWatchWindow->DeleteAllItems();

	// create a new empty root node
	tRootId = m_ptDebugWatchWindow->AddRoot(wxT("watch root"));

	// get the first level of locales
	dbg_write_u08(MUHDBG_CmdGetLocal);
	dbg_write_int(0);

	do
	{
		// get the name, this is empty for end of list
		if( dbg_read_string(strName)==true )
		{
			if( strName.IsEmpty()==false )
			{
				// get typ and value
				if( dbg_read_string(strTyp)==true && dbg_read_string(strValue)==true )
				{
					// add the new node to the tree
					m_ptDebugWatchWindow->AppendItem(tRootId, strName + wxT(" (") + strTyp + wxT(") = ") + strValue);
				}
			}
		}
	} while( strName.IsEmpty()==false );
}

