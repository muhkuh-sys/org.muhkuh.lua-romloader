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


#ifndef __MUHKUH_DEBUGGER__
#define __MUHKUH_DEBUGGER__

#include <wx/wx.h>

#include <wx/aui/aui.h>
#include <wx/intl.h>
#include <wx/socket.h>
#include <wx/stc/stc.h>

class muhkuh_debugger: public wxPanel
{
public:
	muhkuh_debugger(wxWindow *ptParent, wxString &strApplicationPath, unsigned short usDebugServerPort);
	~muhkuh_debugger();

	void OnDebugServerSocket(wxSocketEvent &event);
	void OnDebugConnectionSocket(wxSocketEvent &event);

	void OnDebuggerStepInto(wxCommandEvent &event);
	void OnDebuggerStepOver(wxCommandEvent &event);
	void OnDebuggerStepOut(wxCommandEvent &event);
	void OnDebuggerContinue(wxCommandEvent &event);
	void OnDebuggerBreak(wxCommandEvent &event);


	typedef enum
	{
		DBGEDIT_Marker_BreakPoint	= 1,
		DBGEDIT_Marker_CurrentLine	= 2
	} tDBGEDIT_Marker;

private:
	void create_socket_server(void);
	void delete_socket_server(void);

	void create_controls(void);
	wxStyledTextCtrl *create_debugger_editor(wxString strCaption);

	bool dbg_read_string(wxString &strData);
	bool dbg_read_int(int *piData);

	void dbg_write_u08(unsigned char ucData);
	void dbg_write_int(int iData);

	void dbg_packet_InterpreterHalted(void);
	void dbg_fillStackWindow(int iFrames);


	// debugger gui elements
	wxAuiManager *m_ptDebugAuiManager;
	wxAuiNotebook *m_ptDebugSourceNotebook;
	wxListCtrl *m_ptDebugStackWindow;
	wxTextCtrl *m_ptDebugWatchWindow;
	wxToolBar *m_ptDebugToolBar;
	wxStyledTextCtrl *m_ptDebugEditor;

	// the application path
	wxString m_strApplicationPath;

	// the debug server port
	unsigned short m_usDebugServerPort;

	// the debug server socket
	wxSocketServer *m_ptDebugSocketServer;
	wxSocketBase *m_ptDebugConnection;


    DECLARE_EVENT_TABLE()
};


#endif	// __MUHKUH_DEBUGGER__

