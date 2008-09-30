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


#include "muhkuh_repository.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/txtstrm.h>
#include <wx/url.h>

#include "muhkuh_icons.h"


muhkuh_repository::muhkuh_repository(wxString strName)
 : m_strName(strName)
 , m_eTyp(REPOSITORY_TYP_UNDEFINED)
 , m_fSelected(false)
 , m_ptTestData(NULL)
{
	// set prefix for messages
	setMe();

	// create the test list
	m_ptTestData = new std::vector<tTestData*>;
}


muhkuh_repository::muhkuh_repository(const muhkuh_repository *ptRepository)
 : m_ptTestData(NULL)
{
	// set prefix for messages
	setMe();

	// create the test list
	m_ptTestData = new std::vector<tTestData*>;

	Assign(ptRepository);
}


muhkuh_repository::~muhkuh_repository(void)
{
	if( m_ptTestData!=NULL )
	{
		removeAllTests();
		delete m_ptTestData;
	}
}

void muhkuh_repository::setMe(void)
{
	m_strMe.Printf("muhkuh_repository(%p) :", this);
}


void muhkuh_repository::removeAllTests(void)
{
	std::vector<tTestData*>::const_iterator iter;
	tTestData *ptTestData;


	if( m_ptTestData!=NULL )
	{
		// loop over all tests
		iter = m_ptTestData->begin();
		while( iter!=m_ptTestData->end() )
		{
			ptTestData = *iter;
			delete ptTestData;
			// next entry
			++iter;
		}
		// clear the complete list
		m_ptTestData->clear();
	}
}


void muhkuh_repository::Assign(const muhkuh_repository *ptRepository)
{
	m_strName = ptRepository->m_strName;
	m_eTyp = ptRepository->m_eTyp;
	m_strLocation = ptRepository->m_strLocation;
	m_strExtension = ptRepository->m_strExtension;

	wxLogMessage( ptRepository->GetStringRepresentation() );
	wxLogMessage( GetStringRepresentation() );
}


void muhkuh_repository::SetName(wxString strName)
{
	m_strName = strName;
}


void muhkuh_repository::SetDirscan(wxString strLocation, wxString strExtension)
{
	// set repository typ
	m_eTyp = REPOSITORY_TYP_DIRSCAN;
	// set location
	m_strLocation = strLocation;
	// set extension
	m_strExtension = strExtension;
	// no xml pattern in this configuration
	m_strXmlPattern.Clear();
}


void muhkuh_repository::SetFilelist(wxString strLocation)
{
	// set repository typ
	m_eTyp = REPOSITORY_TYP_FILELIST;
	// set location
	m_strLocation = strLocation;
	// no extension in this configuration
	m_strExtension.Clear();
	// no xml pattern in this configuration
	m_strXmlPattern.Clear();
}


void muhkuh_repository::SetSingleXml(wxString strLocation)
{
	// set repository typ
	m_eTyp = REPOSITORY_TYP_SINGLEXML;
	// set location
	m_strLocation = strLocation;
	// no extension in this configuration
	m_strExtension.Clear();
	// no xml pattern in this configuration
	m_strXmlPattern.Clear();
}


void muhkuh_repository::SetAllLocal(wxString strLocation, wxString strExtension, wxString strXmlPattern)
{
	// set repository typ
	m_eTyp = REPOSITORY_TYP_ALLLOCAL;
	// set location
	m_strLocation = strLocation;
	// set extension
	m_strExtension = strExtension;
	// set xml pattern
	m_strXmlPattern = strXmlPattern;
}


void muhkuh_repository::SetSelected(bool fSelected)
{
	m_fSelected = fSelected;
}


wxString muhkuh_repository::GetName(void) const
{
	return m_strName;
}


muhkuh_repository::REPOSITORY_TYP_E muhkuh_repository::GetTyp(void) const
{
	return m_eTyp;
}


wxString muhkuh_repository::GetLocation(void) const
{
	return m_strLocation;
}


wxString muhkuh_repository::GetExtension(void) const
{
	return m_strExtension;
}


wxString muhkuh_repository::GetXmlPattern(void) const
{
	return m_strXmlPattern;
}


bool muhkuh_repository::GetSelected(void) const
{
	return m_fSelected;
}


wxString muhkuh_repository::GetStringRepresentation(void) const
{
	wxString strName;
	wxString strDetails;


	// start with the user defined name
	strName = m_strName;

	// add the rest as details
	strName += wxT(" (");

	// append the typ
	switch( m_eTyp )
	{
	case REPOSITORY_TYP_DIRSCAN:
		strDetails.Printf(_("directory scan in '%s' for '%s'"), m_strLocation.fn_str(), m_strExtension.fn_str());
		break;
	case REPOSITORY_TYP_FILELIST:
		strDetails.Printf(_("filelist from '%s'"), m_strLocation.fn_str());
		break;
	case REPOSITORY_TYP_SINGLEXML:
		strDetails.Printf(_("single xml file from '%s'"), m_strLocation.fn_str());
		break;
	case REPOSITORY_TYP_ALLLOCAL:
		strDetails.Printf(_("all local files from '%s'"), m_strLocation.fn_str());
		break;
	default:
		strDetails = _("unknown typ");
		break;
	}

	strName += strDetails + wxT(")");

	return strName;
}


muhkuh_repository *muhkuh_repository::CreateFromConfig(wxConfigBase *pConfig, int iIndex)
{
	wxString strName;
	int iRepoTyp;
	REPOSITORY_TYP_E eTyp;
	wxString strLocation;
	wxString strExtension;
	wxString strXmlPattern;
	muhkuh_repository *ptRepoCfg;
	wxString strMe;


	// set prefix for messages
	strMe = wxT("muhkuh_repository: ");

	// no item created yet
	ptRepoCfg = NULL;

	// read all settings
	strName = pConfig->Read(wxT("name"), wxEmptyString);
	iRepoTyp = pConfig->Read(wxT("typ"), (long)-1);
	eTyp = (REPOSITORY_TYP_E)iRepoTyp;
	strLocation = pConfig->Read(wxT("location"), wxEmptyString);
	strExtension = pConfig->Read(wxT("extension"), wxEmptyString);
	strXmlPattern = pConfig->Read(wxT("xmlpattern"), wxEmptyString);

	// check settings

	// the name must not be empty
	if( strName.IsEmpty() )
	{
		wxLogWarning(strMe + _("repository entry %d has no name, ignore entry"), iIndex);
	}
	else
	{
		// the typ must be known
		switch( eTyp )
		{
		case REPOSITORY_TYP_DIRSCAN:
			// the dirscan typ must have a location and an extension
			if( strLocation.IsEmpty() )
			{
				wxLogWarning(strMe + _("repository entry %d has no location, ignore entry"), iIndex);
			}
			else if( strExtension.IsEmpty() )
			{
				wxLogWarning(strMe + _("repository entry %d has no extension, ignore entry"), iIndex);
			}
			else
			{
				// the settings are ok, create a new item
				ptRepoCfg = new muhkuh_repository(strName);
				ptRepoCfg->SetDirscan(strLocation, strExtension);
			}
			break;

		case REPOSITORY_TYP_FILELIST:
			// the filelist typ must have a location
			if( strLocation.IsEmpty() )
			{
				wxLogWarning(strMe + _("repository entry %d has no location, ignore entry"), iIndex);
			}
			else
			{
				// the settings are ok, create a new item
				ptRepoCfg = new muhkuh_repository(strName);
				ptRepoCfg->SetFilelist(strLocation);
			}
			break;

		case REPOSITORY_TYP_SINGLEXML:
			// the singlexml typ must have a location
			if( strLocation.IsEmpty() )
			{
				wxLogWarning(strMe + _("repository entry %d has no location, ignore entry"), iIndex);
			}
			else
			{
				// the settings are ok, create a new item
				ptRepoCfg = new muhkuh_repository(strName);
				ptRepoCfg->SetSingleXml(strLocation);
			}
			break;

		case REPOSITORY_TYP_ALLLOCAL:
			// the alllocal typ must have a location, an extension and an xml pattern
			if( strLocation.IsEmpty() )
			{
				wxLogWarning(strMe + _("repository entry %d has no location, ignore entry"), iIndex);
			}
			else if( strExtension.IsEmpty() )
			{
				wxLogWarning(strMe + _("repository entry %d has no extension, ignore entry"), iIndex);
			}
			else if( strXmlPattern.IsEmpty() )
			{
				wxLogWarning(strMe + _("repository entry %d has no xml pattern, ignore entry"), iIndex);
			}
			else
			{
				// the settings are ok, create a new item
				ptRepoCfg = new muhkuh_repository(strName);
				ptRepoCfg->SetAllLocal(strLocation, strExtension, strXmlPattern);
			}
			break;

		default:
			// here end up all unknown types, show an errormessage
			wxLogWarning(strMe + _("repository entry %d has invalid typ, ignore entry"), iIndex);
			break;
		}
	}

	return ptRepoCfg;
}


void muhkuh_repository::write_config(wxConfigBase *pConfig) const
{
	long lRepoTyp;


	// read all settings
	pConfig->Write(wxT("name"), m_strName);
	lRepoTyp = (long)m_eTyp;
	pConfig->Write(wxT("typ"), lRepoTyp);
	pConfig->Write(wxT("location"), m_strLocation);
	pConfig->Write(wxT("extension"), m_strExtension);
	pConfig->Write(wxT("xmlpattern"), m_strXmlPattern);
}


bool muhkuh_repository::createTestlist(pfnTestlistProgress pfnCallback, void *pvCallbackUser)
{
	bool fResult;


	// default result is 'error'
	fResult = false;

	// call the custom scan routine
	switch( m_eTyp )
	{
	case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
		fResult = createTestlist_dirscan(pfnCallback, pvCallbackUser);
		if( fResult==false )
		{
			wxLogError(m_strMe + _("failed to scan the test directory"));
		}
		break;

	case muhkuh_repository::REPOSITORY_TYP_FILELIST:
		fResult = createTestlist_filelist(pfnCallback, pvCallbackUser);
		if( fResult==false )
		{
			wxLogError(m_strMe + _("failed to open the repository"));
		}
		break;

	case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
		fResult = createTestlist_singlexml(pfnCallback, pvCallbackUser);
		if( fResult==false )
		{
			wxLogError(m_strMe + _("failed to open the repository"));
		}
		break;

	case muhkuh_repository::REPOSITORY_TYP_ALLLOCAL:
		fResult = createTestlist_alllocal(pfnCallback, pvCallbackUser);
		if( fResult==false )
		{
			wxLogError(m_strMe + _("failed to scan the directory"));
		}
		break;

	default:
		fResult = false;
		break;
	}

	return fResult;
}


size_t muhkuh_repository::getTestlistCount(void) const
{
	size_t sizTests;


	sizTests = 0;
	if( m_ptTestData!=NULL )
	{
		sizTests = m_ptTestData->size();
	}
	return sizTests;
}


wxString muhkuh_repository::getTestlistPrintUrl(size_t sizTestIdx) const
{
	wxString strResult;
	std::vector<tTestData*>::const_iterator iter;
	tTestData *ptTestData;


	// check test index
	if( m_ptTestData!=NULL && sizTestIdx>=0 && sizTestIdx<m_ptTestData->size() )
	{
		iter = m_ptTestData->begin() + sizTestIdx;
		ptTestData = *iter;
		strResult  = ptTestData->strPrintUrl;
	}

	return strResult;
}


wxString muhkuh_repository::getTestlistBaseUrl(size_t sizTestIdx) const
{
	wxString strResult;
	std::vector<tTestData*>::const_iterator iter;
	tTestData *ptTestData;


	// check test index
	if( m_ptTestData!=NULL && sizTestIdx>=0 && sizTestIdx<m_ptTestData->size() )
	{
		iter = m_ptTestData->begin() + sizTestIdx;
		ptTestData = *iter;
		strResult  = ptTestData->strBaseUrl;
	}

	return strResult;
}


wxString muhkuh_repository::getTestlistXmlUrl(size_t sizTestIdx) const
{
	wxString strResult;
	std::vector<tTestData*>::const_iterator iter;
	tTestData *ptTestData;


	if( m_ptTestData!=NULL && sizTestIdx>=0 && sizTestIdx<m_ptTestData->size() )
	{
		iter = m_ptTestData->begin() + sizTestIdx;
		ptTestData = *iter;
		strResult = ptTestData->strTestDescriptionUrl;
	}

	return strResult;
}


bool muhkuh_repository::createTestlist_dirscan(pfnTestlistProgress pfnCallback, void *pvCallbackUser)
{
	wxString strProgressMessage;
	wxFileName fileName;
	wxString strFilename;
	wxString strCompletePath;
	wxString strUrl;
	wxFileSystem fileSystem;
	bool fScannerIsRunning;
	tTestData *ptTestData;


	// clear any old tests
	removeAllTests();

	// no idea how long the scanning will take -> set to pulse (that's 'unknown time remaining)
	strProgressMessage.Printf(_("scanning local folder '%s' for tests..."), m_strLocation.fn_str());
	fScannerIsRunning = pfnCallback(pvCallbackUser, strProgressMessage, -1, -1);
	if( fScannerIsRunning==true )
	{
		// move to the folder where muhkuh is stored
		// NOTE: this is important for relative paths
		fileName.SetCwd(wxStandardPaths::Get().GetExecutablePath());
		// change to new directory
		fileName.AssignDir(m_strLocation);

		// check the path
		if( !fileName.IsOk() )
		{
			// path is not correct
			wxLogError(m_strMe + _("failed to set path, IsOK returned false"));
			return false;
		}
		// does the path point to a directory?
		if( !fileName.IsDir() )
		{
			// no, the path does not point to a directory
			wxLogError(m_strMe + _("the path does not point to a directory"));
			return false;
		}
		// does the directory exist?
		if( !fileName.DirExists() )
		{
			// no, the directory does not exist
			wxLogError(m_strMe + _("the directory does not exist"));
			return false;
		}

		// the path seems to be ok
		strCompletePath = fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
		// convert path to url
		strUrl = wxFileSystem::FileNameToURL(strCompletePath);
		wxLogDebug(m_strMe + _("scanning testdescriptions at '%s'"), strUrl.fn_str());

		// set path
		fileSystem.ChangePathTo(strUrl, true);

		// loop over all files with the requested extension in the directory
		strFilename = fileSystem.FindFirst(m_strExtension, wxFILE);
		while( fScannerIsRunning==true && strFilename.IsEmpty()==false )
		{
			fScannerIsRunning = pfnCallback(pvCallbackUser, strProgressMessage, -1, -1);

			// convert to url
			strUrl = wxFileSystem::FileNameToURL(strFilename);
			wxLogDebug(m_strMe + _("found '%s'"), strUrl.fn_str());

			ptTestData = new tTestData;
			ptTestData->strPrintUrl = strUrl;
			ptTestData->strBaseUrl = strUrl + wxT("#zip:");
			ptTestData->strTestDescriptionUrl = strUrl + wxT("#zip:test_description.xml");
			ptTestData->strPrePath.Clear();
			m_ptTestData->push_back(ptTestData);

			strFilename = fileSystem.FindNext();
		}
	}

	// completed the list if the scanner was not canceled
	return fScannerIsRunning;
}


bool muhkuh_repository::createTestlist_filelist(pfnTestlistProgress pfnCallback, void *pvCallbackUser)
{
	wxString strLocation;
	wxURL filelistUrl;
	wxURLError urlError;
	wxString strProgressMessage;
	wxString strMessage;
	wxFileSystem fileSystem;
	wxFSFile *ptFsFile;
	wxInputStream *ptInputStream;
	wxTextInputStream *ptTextInputStream;
	wxString strFilename;
	wxString strUrl;
	bool fScannerIsRunning;
	wxArrayString astrTmpPathNames;
	size_t sizCnt;
	size_t sizMax;
	tTestData *ptTestData;


	// clear any old tests
	removeAllTests();

	// get the repository location
	strLocation = m_strLocation;

	// no idea how long the scanning will take -> set to pulse (that's 'unknown time remaining)
	strProgressMessage.Printf(_("scanning repository '%s' for tests..."), m_strLocation.fn_str());
	fScannerIsRunning = pfnCallback(pvCallbackUser, strProgressMessage, -1, -1);

	if( fScannerIsRunning==true )
	{
		// convert filelist to url
		urlError = filelistUrl.SetURL(strLocation);
		if( urlError!=wxURL_NOERR )
		{
			// this was no valid url
			strMessage.Printf(_("Invalid URL : '%s': "), strLocation.fn_str());
			// try to show some details
			switch( urlError )
			{
			case wxURL_SNTXERR:
				strMessage += _("Syntax error in the URL string.");
				break;
			case wxURL_NOPROTO:
				strMessage += _("Found no protocol which can get this URL.");
				break;
			case wxURL_NOHOST:
				strMessage += _("An host name is required for this protocol.");
				break;
			case wxURL_NOPATH:
				strMessage += _("A path is required for this protocol.");
				break;
			case wxURL_CONNERR:
				strMessage += _("Connection error.");
				break;
			case wxURL_PROTOERR:
				strMessage += _("An error occurred during negotiation. (should never happen!)");
				break;
			default:
				strMessage += _("unknown errorcode");
				break;
			}

			// show the error message
			wxLogError(m_strMe + strMessage);
			fScannerIsRunning = false;
		}
		else
		{
			strMessage.Printf(_("Get list of testdescriptions from '%s'"), strLocation.fn_str());
			wxLogMessage(strMessage);

			// test if file exists
			ptFsFile = fileSystem.OpenFile(strLocation);
			if( ptFsFile==NULL )
			{
				wxLogError(m_strMe + _("Failed to open filelist at '%s'"), strLocation.fn_str());
				fScannerIsRunning = false;
			}
			else
			{
				// read the file line by line
				ptInputStream = ptFsFile->GetStream();
				// convert to text input stream for easier handling
				ptTextInputStream = new wxTextInputStream(*ptInputStream);
				while( fScannerIsRunning==true && ptInputStream->Eof()==false )
				{
					strFilename = ptTextInputStream->ReadLine();
					// skip empty lines and comments (start with a hash '#')
					strFilename.Trim(false);
					if( !strFilename.IsEmpty() && strFilename.GetChar(0)!=wxT('#') )
					{
						// add filename to the temp array
						astrTmpPathNames.Add(strFilename);
					}

					// check for cancel button
					strProgressMessage.Printf(_("receiving '%s'"), strFilename.fn_str());
					fScannerIsRunning = pfnCallback(pvCallbackUser, strProgressMessage, -1, -1);
				}
				delete ptTextInputStream;
				delete ptFsFile;

				// normalize the paths and convert to url
				sizCnt = 0;
				sizMax = astrTmpPathNames.GetCount();
				while( fScannerIsRunning==true && sizCnt<sizMax )
				{
					strFilename = astrTmpPathNames.Item(sizCnt++);

					// check for cancel button
					strProgressMessage.Printf(_("checking '%s'"), strFilename.fn_str());
					fScannerIsRunning = pfnCallback(pvCallbackUser, strProgressMessage, sizCnt, sizMax);

					// set initial path, that's important for relative paths (almost all filelists)
					fileSystem.ChangePathTo(strLocation, false);
					ptFsFile = fileSystem.OpenFile(strFilename);
					if( ptFsFile==NULL )
					{
						wxLogError(m_strMe + _("failed to open '%s' from filelist."), strFilename.fn_str());
					}
					else
					{
						strUrl = ptFsFile->GetLocation();
						wxLogMessage(m_strMe + _("found '%s'"), strUrl.fn_str());

						ptTestData = new tTestData;
						ptTestData->strPrintUrl = strUrl;
						ptTestData->strBaseUrl = strUrl + wxT("#zip:");
						ptTestData->strTestDescriptionUrl = strUrl + wxT("#zip:test_description.xml");
						ptTestData->strPrePath.Clear();
						m_ptTestData->push_back(ptTestData);

						// delete the fsfile
						delete ptFsFile;
					}
				}
			}
		}
	}

	// completed the list if the scanner was not canceled
	return fScannerIsRunning;
}


bool muhkuh_repository::createTestlist_singlexml(pfnTestlistProgress pfnCallback, void *pvCallbackUser)
{
	wxString strProgressMessage;
	wxFileName fileName;
	wxString strFilename;
	wxString strUrl;
	wxString strBaseUrl;
	wxFileSystem fileSystem;
	bool fScannerIsRunning;
	tTestData *ptTestData;


	// clear any old tests
	removeAllTests();

	// no idea how long the scanning will take -> set to pulse (that's "unknown time remaining")
	strProgressMessage.Printf(_("scanning single xml file '%s'..."), m_strLocation.fn_str());
	fScannerIsRunning = pfnCallback(pvCallbackUser, strProgressMessage, -1, -1);

	if( fScannerIsRunning==true )
	{
		// move to the folder where muhkuh is stored
		// NOTE: this is important for relative paths
		fileName.SetCwd(wxStandardPaths::Get().GetExecutablePath());
		// change to the file
		fileName.Assign(m_strLocation);

		// check the path
		if( fileName.IsOk()!=true )
		{
			// path is not correct
			wxLogError(m_strMe + _("failed to set path, IsOK returned false"));
			return false;
		}
		// does the path point to a file (i.e. not a directory)?
		if( fileName.IsDir()!=false )
		{
			// no, the path points to a directory
			wxLogError(m_strMe + _("the path does not point to a file"));
			return false;
		}
// this does not work with win98
//		// is the file readable?
//		if( fileName.IsFileReadable()!=true )
//		{
//			// no, the file is not readable
//			wxLogError(wxT("the file is not readable"));
//			return false;
//		}

		// convert path to url
		strUrl = wxFileSystem::FileNameToURL( fileName.GetFullPath(wxPATH_NATIVE) );
		strBaseUrl = wxFileSystem::FileNameToURL( fileName.GetPath(wxPATH_GET_VOLUME, wxPATH_NATIVE) );
		wxLogDebug(m_strMe + _("found '%s'"), strUrl.fn_str());

		ptTestData = new tTestData;
		ptTestData->strPrintUrl = strUrl;
		ptTestData->strBaseUrl = strBaseUrl;
		ptTestData->strTestDescriptionUrl = strUrl;
		ptTestData->strPrePath.Clear();
		m_ptTestData->push_back(ptTestData);
	}

	// completed the list if the scanner was not canceled
	return fScannerIsRunning;
}


bool muhkuh_repository::createTestlist_alllocal(pfnTestlistProgress pfnCallback, void *pvCallbackUser)
{

}

