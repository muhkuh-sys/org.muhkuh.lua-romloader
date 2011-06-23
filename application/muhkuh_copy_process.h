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


#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/stream.h>
#include <wx/string.h>
#include <wx/thread.h>

#ifndef __MUHKUH_COPY_PROCESS_H__
#define __MUHKUH_COPY_PROCESS_H__


typedef enum
{
	MUHKUH_COPY_PROCESS_STATE_Idle              = 0,
	MUHKUH_COPY_PROCESS_STATE_Scanning          = 1,
	MUHKUH_COPY_PROCESS_STATE_Copy              = 2,
	MUHKUH_COPY_PROCESS_STATE_Finished          = 3
} MUHKUH_COPY_PROCESS_STATE_T;


/* FIXME: Replace this with wxThreadEvent. */
class wxMuhkuhCopyProgressEvent: public wxEvent
{
public:
	wxMuhkuhCopyProgressEvent(wxEventType tCommandType=wxEVT_NULL, int iId=0)
	 : wxEvent(iId, tCommandType)
	{
		m_tState = MUHKUH_COPY_PROCESS_STATE_Idle;
		m_sizTotalFiles = 0;
		m_sizCurrentFileIdx = 0;
		m_sizCurrentFileInBytes = 0;
		m_sizCurrentFileBytesProcessed = 0;
	}
	wxMuhkuhCopyProgressEvent(const wxMuhkuhCopyProgressEvent &tSrc)
	 : wxEvent(tSrc)
	{
		m_tState                       = tSrc.m_tState;
		m_sizTotalFiles                = tSrc.m_sizTotalFiles;
		m_sizCurrentFileIdx            = tSrc.m_sizCurrentFileIdx;
		m_sizCurrentFileInBytes        = tSrc.m_sizCurrentFileInBytes;
		m_sizCurrentFileBytesProcessed = tSrc.m_sizCurrentFileBytesProcessed;
		m_strCurrentFileName           = tSrc.m_strCurrentFileName;
	}

	MUHKUH_COPY_PROCESS_STATE_T GetState(void) const
	{
		return m_tState;
	}
	size_t GetTotalFiles(void) const
	{
		return m_sizTotalFiles;
	}
	size_t GetCurrentFileIdx(void) const
	{
		return m_sizCurrentFileIdx;
	}
	size_t GetCurrentFileSizeInBytes(void) const
	{
		return m_sizCurrentFileInBytes;
	}
	size_t GetCurrentFileBytesProcessed(void) const
	{
		return m_sizCurrentFileBytesProcessed;
	}
	wxString GetCurrentFileName(void) const
	{
		return m_strCurrentFileName;
	}

	void SetState(MUHKUH_COPY_PROCESS_STATE_T tState)
	{
		m_tState = tState;
	}
	void SetTotalFiles(size_t sizTotalFiles)
	{
		m_sizTotalFiles = sizTotalFiles;
	}
	void SetCurrentFileIdx(size_t sizCurrentFileIdx)
	{
		m_sizCurrentFileIdx = sizCurrentFileIdx;
	}
	void SetCurrentFileSizeInBytes(size_t sizCurrentFileInBytes)
	{
		m_sizCurrentFileInBytes = sizCurrentFileInBytes;
	}
	void SetCurrentFileBytesProcessed(size_t sizCurrentFileBytesProcessed)
	{
		m_sizCurrentFileBytesProcessed = sizCurrentFileBytesProcessed;
	}
	void SetCurrentFileName(wxString strCurrentFileName)
	{
		m_strCurrentFileName = strCurrentFileName;
	}

	wxEvent *Clone(void) const
	{
		return new wxMuhkuhCopyProgressEvent(*this);
	}

protected:
	MUHKUH_COPY_PROCESS_STATE_T m_tState;
	size_t m_sizTotalFiles;
	size_t m_sizCurrentFileIdx;
	size_t m_sizCurrentFileInBytes;
	size_t m_sizCurrentFileBytesProcessed;
	wxString m_strCurrentFileName;
};


DECLARE_EVENT_TYPE(wxEVT_MUHKUH_COPY_PROGRESS, -1)

typedef void (wxEvtHandler::*wxMuhkuhCopyProgressEventFunction)(wxMuhkuhCopyProgressEvent&);

#define EVT_MUHKUH_COPY_PROGRESS(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_MUHKUH_COPY_PROGRESS, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxMuhkuhCopyProgressEventFunction, & fn ), (wxObject *) NULL ),




class muhkuh_copy_process : public wxThreadHelper
{
public:
	muhkuh_copy_process(wxString strRemoteBase, wxString strLocalBase, wxEvtHandler *ptEventHandler);
	~muhkuh_copy_process(void);

	wxThread::ExitCode Entry(void);

private:
	bool get_local_folder(wxString strRemoteUrl, wxFileName *ptFileName);
	bool create_local_folder(wxString strRemoteUrl);
	bool copy_file(wxString strRemoteUrl);
	bool copy_file_data(wxInputStream *ptInputStream, wxOutputStream *ptOutputStream, size_t sizFile);
	void send_progress_information(size_t sizProcessedBytes=0, size_t sizTotalBytes=0);

	static const size_t m_sizCopyChunkSize = 8192;

	wxString m_strRemoteBase;
	wxString m_strLocalBase;
	wxEvtHandler *m_ptEventHandler;

	MUHKUH_COPY_PROCESS_STATE_T m_tState;

	wxArrayString m_astrTodoProcessFolders;
	wxArrayString m_astrVisitedFolders;
	wxArrayString m_astrTodoCopyFiles;

	/* This is just for the progress information. */
	size_t m_sizCurrentFileIdx;
	wxString m_strCurrentFileName;
};


#endif  /* __MUHKUH_COPY_PROCESS_H__ */

