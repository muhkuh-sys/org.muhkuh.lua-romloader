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


//#include <wx/dir.h>
//#include <wx/filename.h>
#include <wx/filesys.h>
//#include <wx/stdpaths.h>
//#include <wx/txtstrm.h>
//#include <wx/url.h>

#include "muhkuh_repository_manager.h"


muhkuh_repository_manager::muhkuh_repository_manager(void)
 : m_ptRepositories(NULL)
 , m_sizActiveRepositoryIdx(wxNOT_FOUND)
{
	// create the repository list
	m_ptRepositories = new std::vector<muhkuh_repository*>;
}


muhkuh_repository_manager::muhkuh_repository_manager(const muhkuh_repository_manager *ptClone)
 : m_ptRepositories(NULL)
 , m_sizActiveRepositoryIdx(wxNOT_FOUND)
{
	std::vector<muhkuh_repository*> *tmpRepositories;
	std::vector<muhkuh_repository*>::const_iterator iter;
	muhkuh_repository *ptRepos;
	muhkuh_repository *ptNewRepos;
	int iCnt;


	// clone the repository list
	m_ptRepositories = new std::vector<muhkuh_repository*>;
	iter = ptClone->m_ptRepositories->begin();
	iCnt = 0;
	while( iter!=ptClone->m_ptRepositories->end() )
	{
		ptRepos = *iter;
		ptNewRepos = new muhkuh_repository(ptRepos);
		if( iCnt==ptClone->m_sizActiveRepositoryIdx )
		{
			ptNewRepos->SetSelected(true);
		}
		m_ptRepositories->push_back(ptNewRepos);
		// next entry
		++iter;
		++iCnt;
	}

	// get the active repository
	m_sizActiveRepositoryIdx = ptClone->m_sizActiveRepositoryIdx;
}


muhkuh_repository_manager::~muhkuh_repository_manager(void)
{
	// clear the repository list
	if( m_ptRepositories!=NULL )
	{
		clearAllRepositories();
		delete m_ptRepositories;
	}
}


void muhkuh_repository_manager::read_config(wxConfigBase *pConfig)
{
	int iRepositoryCnt;
	wxString strReposEntry;
	bool fFoundPath;
	muhkuh_repository *ptRepoCfg;
	int iActiveRepoIdx;


	// get all repositories
	clearAllRepositories();

	pConfig->SetPath(wxT("/Repositories"));
	// get the index of the active repository
	iActiveRepoIdx = pConfig->Read(wxT("active"), (long)-1);
	iRepositoryCnt = 0;
	do
	{
		strReposEntry.Printf(wxT("repository_%d"), iRepositoryCnt);
		fFoundPath = pConfig->HasGroup(strReposEntry);
		if( fFoundPath==true )
		{
			// one more entry found
			++iRepositoryCnt;
			// set new path
			pConfig->SetPath(strReposEntry);
			// try to create a repository config object from the settings
			ptRepoCfg = muhkuh_repository::CreateFromConfig(pConfig, iRepositoryCnt);
			if( ptRepoCfg!=NULL )
			{
				// add the entry to the list
				m_ptRepositories->push_back(ptRepoCfg);
			}
			// go one path element back
			pConfig->SetPath(wxT(".."));
		}
	} while( fFoundPath==true );

	// validate active repository index
	if( iRepositoryCnt==0 )
	{
		// no item in the list -> nothing can be active
		iActiveRepoIdx = -1;
	}
	else if( iActiveRepoIdx<0 )
	{
		// set to first item
		iActiveRepoIdx = 0;
	}
	else if( (unsigned int)iActiveRepoIdx>=m_ptRepositories->size() )
	{
		// the index exceeds the list, set to first item
		iActiveRepoIdx = 0;
	}
	m_sizActiveRepositoryIdx = iActiveRepoIdx;
}


void muhkuh_repository_manager::write_config(wxConfigBase *pConfig)
{
	long lActiveRepoIdx;
	wxString strReposEntry;
	std::vector<muhkuh_repository*>::const_iterator iter;
	size_t sizRepositoryCnt;
	muhkuh_repository *ptRepoCfg;


	// Delete the complete 'Repositories' path to clear all old entries.
	// Without this, old entries would pop up at the end of the list when you delete a repository.
	pConfig->DeleteGroup(wxT("/Repositories"));

	// save all repositories
	pConfig->SetPath(wxT("/Repositories"));
	// save the index of the active repository
	lActiveRepoIdx = m_sizActiveRepositoryIdx;
	pConfig->Write(wxT("active"), lActiveRepoIdx);
	iter = m_ptRepositories->begin();
	sizRepositoryCnt = 0;
	while( iter!=m_ptRepositories->end() )
	{
		strReposEntry.Printf(wxT("repository_%d"), sizRepositoryCnt);
		// set new path
		pConfig->SetPath(strReposEntry);
		// get the repository object
		ptRepoCfg = *iter;
		// write all settings
		ptRepoCfg->write_config(pConfig);
		// go one path element back
		pConfig->SetPath(wxT(".."));
		// next repository
		++sizRepositoryCnt;
		++iter;
	}
	pConfig->SetPath(wxT("/"));
}


size_t muhkuh_repository_manager::GetRepositoryCount(void) const
{
	return m_ptRepositories->size();
}


size_t muhkuh_repository_manager::GetActiveRepository(void) const
{
	return m_sizActiveRepositoryIdx;
}


wxString muhkuh_repository_manager::GetStringRepresentation(size_t sizIdx) const
{
	std::vector<muhkuh_repository*>::const_iterator iter;
	muhkuh_repository *ptRepository;
	wxString strRepository;


	/* check input parameter */
	if( sizIdx>=m_ptRepositories->size() )
	{
		wxLogError(_("muhkuh_repository_manager::GetStringRepresentation : idx %d is out of range, ignoring request"), sizIdx);
	}
	else
	{
		iter = m_ptRepositories->begin();
		iter += sizIdx;
		ptRepository = *iter;
		strRepository = ptRepository->GetStringRepresentation();
	}

	return strRepository;
}


wxBitmap muhkuh_repository_manager::GetBitmap(size_t sizIdx) const
{
	std::vector<muhkuh_repository*>::const_iterator iter;
	muhkuh_repository *ptRepository;
	wxBitmap tBitmap;


	/* check input parameter */
	if( sizIdx>=m_ptRepositories->size() )
	{
		wxLogError(_("muhkuh_repository_manager::GetBitmap : idx %d is out of range, ignoring request"), sizIdx);
	}
	else
	{
		iter = m_ptRepositories->begin();
		iter += sizIdx;
		ptRepository = *iter;
		tBitmap = ptRepository->GetBitmap();
	}

	return tBitmap;
}


int muhkuh_repository_manager::GetImageListIndex(size_t sizIdx) const
{
	std::vector<muhkuh_repository*>::const_iterator iter;
	muhkuh_repository *ptRepository;
	int iResult;


	/* check input parameter */
	if( sizIdx>=m_ptRepositories->size() )
	{
		wxLogError(_("muhkuh_repository_manager::GetBitmap : idx %d is out of range, ignoring request"), sizIdx);
		iResult = 0;
	}
	else
	{
		iter = m_ptRepositories->begin();
		iter += sizIdx;
		ptRepository = *iter;
		iResult = ptRepository->GetImageListIndex();
	}

	return iResult;
}


void muhkuh_repository_manager::SetActiveRepository(int iActiveRepoIdx)
{
	if( iActiveRepoIdx>=0 && (unsigned int)iActiveRepoIdx<m_ptRepositories->size() )
	{
		// the new index is ok, try to access it
		m_sizActiveRepositoryIdx = iActiveRepoIdx;
	}
}


long muhkuh_repository_manager::addRepository(muhkuh_repository *ptRepository)
{
	long lIdx;


	lIdx = m_ptRepositories->size();
	m_ptRepositories->push_back(ptRepository);

	return lIdx;
}


void muhkuh_repository_manager::removeRepository(size_t sizIdx)
{
	std::vector<muhkuh_repository*>::iterator iter;
	muhkuh_repository *ptRepository;


	/* check input parameter */
	if( sizIdx>=m_ptRepositories->size() )
	{
		wxLogError(_("muhkuh_repository_manager::removeRepository : idx %ld is out of range, ignoring request"), sizIdx);
	}
	else
	{
		iter = m_ptRepositories->begin();
		iter += sizIdx;
		ptRepository = *iter;
		/* remove from list */
		m_ptRepositories->erase(iter);
		/* delete the repository */
		delete ptRepository;
		// adapt active repository index
		if( sizIdx==m_sizActiveRepositoryIdx )
		{
			// deleted the active repository -> no repository is active now
			m_sizActiveRepositoryIdx = wxNOT_FOUND;
		}
		else if( m_sizActiveRepositoryIdx!=wxNOT_FOUND && sizIdx<m_sizActiveRepositoryIdx )
		{
			// deleted a repository before the active one -> index moved one up
			--m_sizActiveRepositoryIdx;
		}
	}
}


muhkuh_repository *muhkuh_repository_manager::GetRepository(size_t sizIdx)
{
	std::vector<muhkuh_repository*>::const_iterator iter;
	muhkuh_repository *ptRepository;


	/* check input parameter */
	if( sizIdx>=m_ptRepositories->size() )
	{
		wxLogError(_("muhkuh_repository_manager::GetRepository : idx %d is out of range, ignoring request"), sizIdx);
		ptRepository = NULL;
	}
	else
	{
		iter = m_ptRepositories->begin();
		iter += sizIdx;
		ptRepository = *iter;
	}

	return ptRepository;
}


void muhkuh_repository_manager::clearAllRepositories(void)
{
	std::vector<muhkuh_repository*>::const_iterator iter;
	muhkuh_repository *ptRepos;


	// loop over all repositories and add them to the list
	iter = m_ptRepositories->begin();
	while( iter!=m_ptRepositories->end() )
	{
		ptRepos = *iter;
		delete ptRepos;
		// next entry
		++iter;
	}
	// clear the complete list
	m_ptRepositories->clear();
}


bool muhkuh_repository_manager::createTestlist(size_t sizIdx, wxProgressDialog *ptScannerProgress)
{
	bool fResult;
	muhkuh_repository *ptRepoCfg;
	std::vector<muhkuh_repository*>::iterator iter;


	// default result is 'error'
	fResult = false;

	// validate the new index
	if( sizIdx>=0 && (unsigned int)sizIdx<m_ptRepositories->size() )
	{
		// the index is valid -> get the config
		iter = m_ptRepositories->begin() + sizIdx;
		ptRepoCfg = *iter;

		// call the scan routine
		fResult = ptRepoCfg->createTestlist(ptScannerProgress);
	}

	return fResult;
}


size_t muhkuh_repository_manager::getTestlistCount(size_t sizIdx) const
{
	size_t sizResult;
	const muhkuh_repository *ptRepoCfg;
	std::vector<muhkuh_repository*>::const_iterator iter;


	// default result is 'error'
	sizResult = -1;

	// validate the new index
	if( sizIdx>=0 && (unsigned int)sizIdx<m_ptRepositories->size() )
	{
		// the index is valid -> get the config
		iter = m_ptRepositories->begin() + sizIdx;
		ptRepoCfg = *iter;

		// call the scan routine
		sizResult = ptRepoCfg->getTestlistCount();
	}

	return sizResult;
}


wxString muhkuh_repository_manager::getTestlistPrintUrl(size_t sizRepositoryIdx, size_t sizTestIdx) const
{
	wxString strResult;
	const muhkuh_repository *ptRepoCfg;
	std::vector<muhkuh_repository*>::const_iterator iter;


	// default result is 'error'
	strResult = wxEmptyString;

	// validate the new index
	if( sizRepositoryIdx>=0 && (unsigned int)sizRepositoryIdx<m_ptRepositories->size() )
	{
		// the index is valid -> get the config
		iter = m_ptRepositories->begin() + sizRepositoryIdx;
		ptRepoCfg = *iter;

		// call the scan routine
		strResult = ptRepoCfg->getTestlistPrintUrl(sizTestIdx);
	}

	return strResult;
}


wxString muhkuh_repository_manager::getTestlistBaseUrl(size_t sizRepositoryIdx, size_t sizTestIdx) const
{
	wxString strResult;
	const muhkuh_repository *ptRepoCfg;
	std::vector<muhkuh_repository*>::const_iterator iter;


	// default result is 'error'
	strResult = wxEmptyString;

	// validate the new index
	if( sizRepositoryIdx>=0 && (unsigned int)sizRepositoryIdx<m_ptRepositories->size() )
	{
		// the index is valid -> get the config
		iter = m_ptRepositories->begin() + sizRepositoryIdx;
		ptRepoCfg = *iter;

		// call the scan routine
		strResult = ptRepoCfg->getTestlistBaseUrl(sizTestIdx);
	}

	return strResult;
}


wxString muhkuh_repository_manager::getTestlistXmlUrl(size_t sizRepositoryIdx, size_t sizTestIdx) const
{
	wxString strResult;
	const muhkuh_repository *ptRepoCfg;
	std::vector<muhkuh_repository*>::const_iterator iter;


	// default result is 'error'
	strResult = wxEmptyString;

	// validate the new index
	if( sizRepositoryIdx>=0 && (unsigned int)sizRepositoryIdx<m_ptRepositories->size() )
	{
		// the index is valid -> get the config
		iter = m_ptRepositories->begin() + sizRepositoryIdx;
		ptRepoCfg = *iter;

		// call the scan routine
		strResult = ptRepoCfg->getTestlistXmlUrl(sizTestIdx);
	}

	return strResult;
}

