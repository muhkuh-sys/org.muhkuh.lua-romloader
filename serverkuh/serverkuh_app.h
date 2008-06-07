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


#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/filename.h>

#include "serverkuh_mainFrame.h"


#ifndef __SERVERKUH_APP_H__
#define __SERVERKUH_APP_H__

class serverkuh_app : public wxApp
{
public:
	bool OnInit(void);
	int OnExit(void);

private:
	void showHelp(wxCmdLineParser *ptParser);
	void showVersion(void);
};

DECLARE_APP(serverkuh_app);

#endif	// __SERVERKUH_APP_H__

