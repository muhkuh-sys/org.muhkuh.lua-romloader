/***************************************************************************
 *   Copyright (C) 2011 by Christoph Thelen                                *
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


#include "muhkuh_capture_std.h"

muhkuh_capture_std::muhkuh_capture_std(wxEvtHandler *ptParent=NULL, int iId=-1)
 : wxProcess(ptParent, iId)
{
	wxLogMessage("muhkuh_capture_std created");
}

muhkuh_capture_std::~muhkuh_capture_std(void)
{
	wxLogMessage("muhkuh_capture_std deleted");
}


void muhkuh_capture_std::OnTerminate(int iPid, int iStatus)
{
	wxLogMessage("muhkuh_capture_std process terminated");
}



MuhkuhCaptureThread::MuhkuhCaptureThread(wxProcess *ptProcess)
 : wxThread()
 , m_ptProcess(ptProcess)
{
}

void MuhkuhCaptureThread::OnExit(void)
{
}

void *MuhkuhCaptureThread::Entry(void)
{
	wxInputStream *ptStreamError;
	wxInputStream *ptStreamInput;
	long lPid;


	/* Redirect stdin, stdout and stderr. */
	m_ptProcess->Redirect();

	/* Get stdin and stderr. */
	ptStreamError = m_ptProcess->GetErrorStream();
	if( ptStreamError==NULL )
	{
		wxLogError(wxT("Failed to get error stream!"));
	}
	else
	{
		ptStreamInput = m_ptProcess->GetInputStream();
		if( ptStreamInput==NULL )
		{
			wxLogError(wxT("Failed to get input stream!"));
		}
		else
		{
			/* Execute the command. */
			lPid = wxExecute(char **argv, int flags = wxEXEC_ASYNC, wxProcess *callback = NULL);
			if( lPid==0 )
			{
				wxLogError(wxT("Failed to execute the command!"));
			}
			else
			{
				do
				{
				} while( m_ptProcess->);
			}
		}
	}

	return NULL;
}


