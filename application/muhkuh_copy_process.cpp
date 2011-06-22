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


#include "muhkuh_copy_process.h"

#include <wx/filesys.h>
#include <wx/log.h>


DEFINE_EVENT_TYPE(wxEVT_MUHKUH_COPY_PROGRESS)


muhkuh_copy_process::muhkuh_copy_process(wxString strRemoteBase, wxString strLocalBase, wxEvtHandler *ptEventHandler)
 : wxThreadHelper()
 , m_tState(MUHKUH_COPY_PROCESS_STATE_Idle)
 , m_ptEventHandler(ptEventHandler)
{
	m_strRemoteBase = strRemoteBase;

	m_strLocalBase = strLocalBase;
}


muhkuh_copy_process::~muhkuh_copy_process(void)
{
}


wxThread::ExitCode muhkuh_copy_process::Entry(void)
{
	wxFileSystem tFileSystemRemote;
	wxString strCurrentItem;
	wxString strFileUrl;
	wxString strRelativePath;
	bool fResult;
	size_t sizFileCnt;
	size_t sizFileEnd;


	/* A fresh start -> clear all todo lists. */
	m_astrTodoProcessFolders.Empty();
	m_astrVisitedFolders.Empty();
	m_astrTodoCopyFiles.Empty();

	/* Loop over the complete remote directory structure and collect all files and folders. */

	/* Add the base folder of the test to the copy todo list. */
	m_astrTodoProcessFolders.Add(m_strRemoteBase);

	/* Process all entries in the copy todo list. */
	m_tState = MUHKUH_COPY_PROCESS_STATE_Scanning;
	fResult = true;
	while( m_astrTodoProcessFolders.IsEmpty()==false )
	{
		/* Get the first entry from the todo list. */
		strCurrentItem = m_astrTodoProcessFolders.Item(0);
		/* Remove it from the list so it will not be processed again. */
		m_astrTodoProcessFolders.RemoveAt(0,1);
		wxLogMessage(_("Processing folder '%s'"), strCurrentItem.c_str());

		/* Is the current folder already in the visited list? */
		if( m_astrVisitedFolders.Index(strCurrentItem.c_str(), true, false)!=wxNOT_FOUND )
		{
			wxLogError(_("Cyclic path to '%s' found, aborting!"), strCurrentItem.c_str());
			fResult = false;
			break;
		}
		/* Add this folder to the visited list. */
		m_astrVisitedFolders.Add(strCurrentItem);

		/* Change to this folder. */
		tFileSystemRemote.ChangePathTo(strCurrentItem);

		/* First get all files. */
		strFileUrl = tFileSystemRemote.FindFirst(wxT("*"), wxFILE);
		while( strFileUrl.IsEmpty()==false )
		{
			if( m_thread->TestDestroy()==true )
			{
				fResult = false;
				break;
			}

			wxLogMessage(wxT("Found file '%s'"), strFileUrl.c_str());

			m_astrTodoCopyFiles.Add(strFileUrl);

			send_progress_information();

			strFileUrl = tFileSystemRemote.FindNext();
		}

		/* Now add all subfolders to the todo list. */
		strFileUrl = tFileSystemRemote.FindFirst(wxT("*"), wxDIR);
		while( strFileUrl.IsEmpty()==false )
		{
			if( m_thread->TestDestroy()==true )
			{
				fResult = false;
				break;
			}

			/* Paths must end with a slash for URIs. */
			if( strFileUrl.EndsWith(wxT("/"), NULL)==false )
			{
				strFileUrl.Append(wxT("/"));
			}
			wxLogMessage(wxT("found folder '%s'"), strFileUrl.c_str());

			fResult = create_local_folder(strFileUrl);
			if( fResult!=true )
			{
				break;
			}

			m_astrTodoProcessFolders.Add(strFileUrl);

			send_progress_information();

			strFileUrl = tFileSystemRemote.FindNext();
		}
	}

	if( fResult==true )
	{
		m_tState = MUHKUH_COPY_PROCESS_STATE_Copy;

		sizFileCnt = 0;
		sizFileEnd = m_astrTodoCopyFiles.GetCount();
		while( sizFileCnt<sizFileEnd )
		{
			if( m_thread->TestDestroy()==true )
			{
				fResult = false;
				break;
			}

			/* Get the first entry from the todo list. */
			strCurrentItem = m_astrTodoCopyFiles.Item(sizFileCnt);
			wxLogMessage(_("Processing file '%s'"), strCurrentItem.c_str());

			send_progress_information();

			++sizFileCnt;
		}
	}

	m_tState = MUHKUH_COPY_PROCESS_STATE_Finished;
	send_progress_information();

	return (wxThread::ExitCode)(fResult ? 0 : 1);
}


bool muhkuh_copy_process::get_local_folder(wxString strRemoteUrl, wxFileName *ptFileName)
{
	bool fResult;
	wxString strRelativePath;
	size_t sizChunkStart;
	size_t sizChunkEnd;
	size_t sizString;
	wxString strPathComponent;


	/* Start in the working folder. */
	ptFileName->AssignDir(m_strLocalBase);

	/* Get the relative path from the test base url to the current url. */
	fResult = strRemoteUrl.StartsWith(m_strRemoteBase, &strRelativePath);
	if( fResult!=true )
	{
		/* FIXME: find some better mesaage for this. */
		wxLogError(_("Invalid archive, path escapes root!"));
	}
	else
	{
		wxLogMessage(wxT("relative path is: %s"), strRelativePath.c_str());

		/* Split the relative path in its components. */
		sizChunkStart = 0;
		sizChunkEnd = 0;
		sizString = strRelativePath.Len();
		while( sizChunkEnd<sizString )
		{
			if( strRelativePath[sizChunkEnd]==wxT('/') )
			{
				/* Found the end of a chunk. */
				if( sizChunkStart<sizChunkEnd )
				{
					strPathComponent = strRelativePath.Mid(sizChunkStart, sizChunkEnd-sizChunkStart);
					/* Create this path component. */
					ptFileName->AppendDir(strPathComponent);
				}
				/* The next chunk starts after this slash. */
				sizChunkStart = sizChunkEnd + 1;
			}
			++sizChunkEnd;
		}
	}

	return fResult;
}


bool muhkuh_copy_process::create_local_folder(wxString strRemoteUrl)
{
	bool fResult;
	wxFileName tLocalFileName;


	fResult = get_local_folder(strRemoteUrl, &tLocalFileName);
	if( fResult==true )
	{
		fResult = tLocalFileName.Mkdir();
		if( fResult!=true )
		{
			wxLogError(_("Failed to create the folder %s"), tLocalFileName.GetFullPath().c_str());
		}
	}

	return fResult;
}


void muhkuh_copy_process::send_progress_information(void)
{
	wxMuhkuhCopyProgressEvent tEvent(wxEVT_MUHKUH_COPY_PROGRESS, wxID_ANY);


	/* Set the state. */
	tEvent.SetState(m_tState);

	/* Set the number of files to copy. */
	tEvent.SetTotalFiles(m_astrTodoCopyFiles.GetCount());
	if( m_tState==MUHKUH_COPY_PROCESS_STATE_Copy )
	{
		tEvent.SetCurrentFileIdx(0);
		
	}

	if( m_ptEventHandler!=NULL )
	{
		m_ptEventHandler->AddPendingEvent(tEvent);
	}
}

