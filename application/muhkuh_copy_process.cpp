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
#include <wx/url.h>
#include <wx/wfstream.h>


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
		wxLogDebug("Processing folder '%s'", strCurrentItem);

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
		strFileUrl = tFileSystemRemote.FindFirst("*", wxFILE);
		while( strFileUrl.IsEmpty()==false )
		{
			if( m_thread->TestDestroy()==true )
			{
				fResult = false;
				break;
			}

			wxLogDebug("Found file '%s'", strFileUrl);

			m_astrTodoCopyFiles.Add(strFileUrl);

			send_progress_information();

			strFileUrl = tFileSystemRemote.FindNext();
		}

		/* Now add all subfolders to the todo list. */
		strFileUrl = tFileSystemRemote.FindFirst("*", wxDIR);
		while( strFileUrl.IsEmpty()==false )
		{
			if( m_thread->TestDestroy()==true )
			{
				fResult = false;
				break;
			}

			/* Paths must end with a slash for URIs. */
			if( strFileUrl.EndsWith("/", NULL)==false )
			{
				strFileUrl.Append("/");
			}
			wxLogDebug("found folder '%s'", strFileUrl);

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

		m_sizCurrentFileIdx = 0;
		sizFileEnd = m_astrTodoCopyFiles.GetCount();
		while( m_sizCurrentFileIdx<sizFileEnd )
		{
			if( m_thread->TestDestroy()==true )
			{
				fResult = false;
				break;
			}

			/* Get the first entry from the todo list. */
			strCurrentItem = m_astrTodoCopyFiles.Item(m_sizCurrentFileIdx);
			wxLogDebug("Processing file '%s'", strCurrentItem);

			fResult = copy_file(strCurrentItem);

			send_progress_information();

			++m_sizCurrentFileIdx;
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
		/* Split the relative path in its components. */
		sizChunkStart = 0;
		sizChunkEnd = 0;
		sizString = strRelativePath.Len();
		while( sizChunkEnd<sizString )
		{
			if( strRelativePath[sizChunkEnd]=='/' )
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
		/* If the path does not end with a slash, the last chunk is still open. */
		if( sizChunkStart<sizChunkEnd )
		{
			strPathComponent = strRelativePath.Mid(sizChunkStart, sizChunkEnd-sizChunkStart);
			/* Create this path component. */
			ptFileName->AppendDir(strPathComponent);
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


bool muhkuh_copy_process::copy_file(wxString strRemoteUrl)
{
	bool fResult;
	int iLastSlash;
	int iLastColon;
	int iFilePart;
	wxString strRemotePath;
	wxString strRemoteFile;
	wxFileName tLocalFileName;
	wxFileSystem tRemoteFileSystem;
	wxFFileOutputStream *ptLocalOutputStream;
	wxFSFile *ptRemoteFile;
	wxInputStream *ptInputStream;
	size_t sizFile;


	/*
	 * Get the local filename for the remote URL.
	 */
	iLastSlash = strRemoteUrl.Find('/', true);
	iLastColon = strRemoteUrl.Find(':', true);
	if( iLastSlash==wxNOT_FOUND && iLastColon==wxNOT_FOUND )
	{
		wxLogError(_("The URL has no file part: %s"), strRemoteUrl);
		fResult = false;
	}
	else
	{
		if( (iLastColon==wxNOT_FOUND) || (iLastColon!=wxNOT_FOUND && (iLastSlash>iLastColon)) )
		{
			/* The path ends with a slash and a file component.
			 * Do not include the slash in the path or file component.
			 */
			strRemotePath = strRemoteUrl.Left(iLastSlash);
			strRemoteFile = strRemoteUrl.Right(strRemoteUrl.Len()-iLastSlash-1);
		}
		else
		{
			/* The path ends with a colon.
			 * Add the colon to the path component.
			 */
			strRemotePath = strRemoteUrl.Left(iLastColon+1);
			strRemoteFile = strRemoteUrl.Right(strRemoteUrl.Len()-iLastColon-1);
		}

		/* Set the filename for the progress information.
		 * Is this a subdirectory of the test base folder?
		 * Yes -> only show the relative path.
		 */
		fResult = strRemoteUrl.StartsWith(m_strRemoteBase, &m_strCurrentFileName);
		if( fResult!=true )
		{
			m_strCurrentFileName = strRemoteUrl;
		}

		/* Get the URL without the file, i.e. everything before the last slash or colon. */
		fResult = get_local_folder(strRemotePath, &tLocalFileName);
		if( fResult==true )
		{
			/* Append the filename part to the local path. */
			tLocalFileName.SetFullName(strRemoteFile);
			wxLogDebug("Copy '%s' to '%s", strRemoteUrl, tLocalFileName.GetFullPath());

			/* Open the local file for writing. */
			ptLocalOutputStream = new wxFFileOutputStream(tLocalFileName.GetFullPath(), "wb");
			if( ptLocalOutputStream->IsOk()!=true )
			{
				wxLogError(_("Failed to open the local file for writing: %s"), tLocalFileName.GetFullPath());
				fResult = false;
			}
			else
			{
				ptRemoteFile = tRemoteFileSystem.OpenFile(strRemoteUrl, wxFS_READ);
				if( ptRemoteFile==NULL )
				{
					wxLogError(_("Failed to open the remote file for reading: %s"), strRemoteUrl);
					fResult = false;
				}
				else
				{
					/* Get the input stream. */
					ptInputStream = ptRemoteFile->GetStream();
					if( ptInputStream==NULL )
					{
						wxLogError(_("Failed to get the stream for the remote file: %s"), strRemoteUrl);
						fResult = false;
					}
					else
					{
						sizFile = ptInputStream->GetSize();
						wxLogDebug("The remote file %s claims to have %d bytes.", strRemoteUrl, sizFile);
						fResult = copy_file_data(ptInputStream, ptLocalOutputStream, sizFile);

						/* NOTE: do not delete the input stream ptInputStream. It is still owned by ptRemoteFile. */
					}
					/* Close the remote file. */
					delete ptRemoteFile;
				}
				/* Close the local file. */
				delete ptLocalOutputStream;
			}
		}
	}

	return fResult;
}


bool muhkuh_copy_process::copy_file_data(wxInputStream *ptInputStream, wxOutputStream *ptOutputStream, size_t sizFile)
{
	bool fResult;
	size_t sizProcessed;
	unsigned char aucBuffer[m_sizCopyChunkSize];
	size_t sizRead;
	wxStreamError tReadError;
	wxStreamError tWriteError;
	unsigned char *pucBuffer;
	size_t sizWriteBuffer;
	size_t sizWrite;


	sizProcessed = 0;

	wxLogDebug("Copy File data");

	/* Expect success. */
	fResult = true;
	tReadError = wxSTREAM_NO_ERROR;
	tWriteError = wxSTREAM_NO_ERROR;
	do
	{
		send_progress_information(sizProcessed, sizFile);

		if( m_thread->TestDestroy()==true )
		{
			fResult = false;
			break;
		}

		/* Read a chunk. */
		sizRead = ptInputStream->Read(aucBuffer, m_sizCopyChunkSize).LastRead();
		wxLogDebug("read %d bytes", sizRead);
		if( sizRead!=m_sizCopyChunkSize )
		{
			tReadError = ptInputStream->GetLastError();
			if( tReadError!=wxSTREAM_NO_ERROR && tReadError!=wxSTREAM_EOF )
			{
				break;
			}
		}

		/* Write the chunk. */
		if( sizRead>0 )
		{
			sizWriteBuffer = sizRead;
			pucBuffer = aucBuffer;
			do
			{
				if( m_thread->TestDestroy()==true )
				{
					fResult = false;
					break;
				}

				sizWrite = ptOutputStream->Write(pucBuffer, sizWriteBuffer).LastWrite();
				wxLogDebug("wrote %d bytes", sizWrite);
				sizWriteBuffer -= sizWrite;
				pucBuffer += sizWrite;
				tWriteError = ptOutputStream->GetLastError();
				if( tWriteError==wxSTREAM_NO_ERROR )
				{
					sizProcessed += sizWrite;
				}
			} while( tWriteError==wxSTREAM_NO_ERROR && sizWriteBuffer>0 );
		}
	} while( tReadError==wxSTREAM_NO_ERROR && tWriteError==wxSTREAM_NO_ERROR );

	wxLogDebug("copy done!");

	ptOutputStream->Close();

	/* The result can already be false, that's from the TestDestroy function. */
	fResult &= (tReadError==wxSTREAM_EOF && tWriteError==wxSTREAM_NO_ERROR);
	return fResult;
}


void muhkuh_copy_process::send_progress_information(size_t sizProcessedBytes, size_t sizTotalBytes)
{
	bool fResult;
	wxString strFileName;
	wxMuhkuhCopyProgressEvent tEvent(wxEVT_MUHKUH_COPY_PROGRESS, wxID_ANY);


	/* Set the state. */
	tEvent.SetState(m_tState);

	/* Set the number of files to copy. */
	tEvent.SetTotalFiles(m_astrTodoCopyFiles.GetCount());
	if( m_tState==MUHKUH_COPY_PROCESS_STATE_Copy )
	{
		tEvent.SetCurrentFileName(m_strCurrentFileName);
		tEvent.SetCurrentFileIdx(m_sizCurrentFileIdx);
		tEvent.SetCurrentFileBytesProcessed(sizProcessedBytes);
		tEvent.SetCurrentFileSizeInBytes(sizTotalBytes);
	}

	if( m_ptEventHandler!=NULL )
	{
		m_ptEventHandler->AddPendingEvent(tEvent);
	}
}

