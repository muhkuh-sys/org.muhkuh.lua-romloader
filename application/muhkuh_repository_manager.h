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


#include <vector>

#include <wx/wx.h>
#include <wx/dynlib.h>
#include <wx/filename.h>


#include "muhkuh_repository.h"


#ifndef __MUHKUH_REPOSITORY_MANAGER_H__
#define __MUHKUH_REPOSITORY_MANAGER_H__


class muhkuh_repository_manager
{
public:
	muhkuh_repository_manager(void);
	muhkuh_repository_manager(const muhkuh_repository_manager *ptClone);
	~muhkuh_repository_manager(void);

	long addRepository(muhkuh_repository *ptRepository);
	void removeRepository(unsigned long ulIdx);
	muhkuh_repository *GetRepository(size_t sizIdx);

	void SetActiveRepository(int iActiveRepoIdx);
	size_t GetActiveRepository(void) const;

	size_t GetRepositoryCount(void) const;
	wxString GetStringRepresentation(size_t sizIdx) const;
	wxBitmap GetBitmap(size_t sizIdx) const;
	int GetImageListIndex(size_t sizIdx) const;

	void read_config(wxConfigBase *pConfig);
	void write_config(wxConfigBase *pConfig);

	bool createTestlist(size_t sizIdx, wxProgressDialog *ptScannerProgress);
	size_t getTestlistCount(size_t sizIdx) const;
	wxString getTestlistPrintUrl(size_t sizRepositoryIdx, size_t sizTestIdx) const;
	wxString getTestlistBaseUrl(size_t sizRepositoryIdx, size_t sizTestIdx) const;
	wxString getTestlistXmlUrl(size_t sizRepositoryIdx, size_t sizTestIdx) const;

private:
	void clearAllRepositories(void);


	// all repositories
	std::vector<muhkuh_repository*> *m_ptRepositories;

	// the active repository
	size_t m_sizActiveRepositoryIdx;
};


#endif	/* __MUHKUH_REPOSITORY_MANAGER_H__ */

