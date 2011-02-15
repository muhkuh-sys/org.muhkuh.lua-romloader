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


#include <wx/wx.h>
#include <wx/process.h>
#include <wx/thread.h>


#ifndef __MUHKUH_CAPTURE_STD_H__
#define __MUHKUH_CAPTURE_STD_H__


class muhkuh_capture_std : public wxProcess
{
public:
	muhkuh_capture_std(wxEvtHandler *ptParent=NULL, int iId=-1);
	~muhkuh_capture_std(void);

	void OnTerminate(int iPid, int iStatus);

private:
	wxT *m_ptTextCtrl;
};


class MuhkuhCaptureThread : public wxThread
{
public:
	MuhkuhCaptureThread(wxProcess *ptProcess);
	virtual void *Entry(void);
	virtual void OnExit(void);

private:
	wxProcess *m_ptProcess;
};


#endif  /* __MUHKUH_CAPTURE_STD_H__ */
