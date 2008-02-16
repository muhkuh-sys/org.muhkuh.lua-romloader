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


#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/stdpaths.h>
#include <wx/txtstrm.h>
#include <wx/url.h>


#include "muhkuh_repository.h"


#include "icons/famfamfam_silk_icons_v013/folder_table.xpm"
#include "icons/famfamfam_silk_icons_v013/database.xpm"
#include "icons/famfamfam_silk_icons_v013/script.xpm"
#include "icons/famfamfam_silk_icons_v013/exclamation.xpm"


muhkuh_repository::muhkuh_repository(wxString strName)
 : m_strName(strName)
 , m_eTyp(REPOSITORY_TYP_UNDEFINED)
 , m_fSelected(false)
{
}


muhkuh_repository::muhkuh_repository(const muhkuh_repository *ptRepository)
{
	Assign(ptRepository);
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
}


void muhkuh_repository::SetFilelist(wxString strLocation)
{
	// set repository typ
	m_eTyp = REPOSITORY_TYP_FILELIST;
	// set location
	m_strLocation = strLocation;
	// no extension in this configuration
	m_strExtension.Clear();
}


void muhkuh_repository::SetSingleXml(wxString strLocation)
{
	// set repository typ
	m_eTyp = REPOSITORY_TYP_SINGLEXML;
	// set location
	m_strLocation = strLocation;
	// no extension in this configuration
	m_strExtension.Clear();
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
	default:
		strDetails = _("unknown typ");
		break;
	}

	strName += strDetails + wxT(")");

	return strName;
}


wxBitmap muhkuh_repository::GetBitmap(void) const
{
	return GetBitmap(m_eTyp);
}


wxBitmap muhkuh_repository::GetBitmap(REPOSITORY_TYP_E eTyp)
{
	const char **ppcXpm;


	// append the typ
	switch( eTyp )
	{
	case REPOSITORY_TYP_DIRSCAN:
		ppcXpm = icon_famfamfam_silk_folder_table;
		break;
	case REPOSITORY_TYP_FILELIST:
		ppcXpm = icon_famfamfam_silk_database;
		break;
	case REPOSITORY_TYP_SINGLEXML:
		ppcXpm = icon_famfamfam_silk_script;
		break;
	default:
		ppcXpm = icon_famfamfam_silk_exclamation;
		break;
	}

	return wxBitmap(ppcXpm);
}


wxImageList *muhkuh_repository::CreateNewImageList(void)
{
	wxImageList *ptImageList;
	wxBitmap bitmap;


	// create the new empty list and reserve space for 4 icons
	ptImageList = new wxImageList(16, 16, true, 4);

	// NOTE: if you change the order or amount of icons here, you must also change the GetImageListIndex function
	// add 'undefined' bitmap
	bitmap = muhkuh_repository::GetBitmap(muhkuh_repository::REPOSITORY_TYP_UNDEFINED);
	ptImageList->Add(bitmap);
	// add 'dirscan' bitmap
	bitmap = muhkuh_repository::GetBitmap(muhkuh_repository::REPOSITORY_TYP_DIRSCAN);
	ptImageList->Add(bitmap);
	// add 'filelist' bitmap
	bitmap = muhkuh_repository::GetBitmap(muhkuh_repository::REPOSITORY_TYP_FILELIST);
	ptImageList->Add(bitmap);
	// add 'singlexml' bitmap
	bitmap = muhkuh_repository::GetBitmap(muhkuh_repository::REPOSITORY_TYP_SINGLEXML);
	ptImageList->Add(bitmap);

	// return the new imagelist
	return ptImageList;
}


int muhkuh_repository::GetImageListIndex(REPOSITORY_TYP_E eTyp)
{
	int iIdx;


	switch( eTyp )
	{
	case REPOSITORY_TYP_DIRSCAN:
		iIdx = 1;
		break;
	case REPOSITORY_TYP_FILELIST:
		iIdx = 2;
		break;
	case REPOSITORY_TYP_SINGLEXML:
		iIdx = 3;
		break;
	case REPOSITORY_TYP_UNDEFINED:
	default:
		iIdx = 0;
		break;
	}

	return iIdx;
}


int muhkuh_repository::GetImageListIndex(void) const
{
	return GetImageListIndex(m_eTyp);
}


muhkuh_repository *muhkuh_repository::CreateFromConfig(wxConfigBase *pConfig, int iIndex)
{
	wxString strName;
	int iRepoTyp;
	REPOSITORY_TYP_E eTyp;
	wxString strLocation;
	wxString strExtension;
	muhkuh_repository *ptRepoCfg;


	// no item created yet
	ptRepoCfg = NULL;

	// read all settings
	strName = pConfig->Read(wxT("name"), wxEmptyString);
	iRepoTyp = pConfig->Read(wxT("typ"), (long)-1);
	eTyp = (REPOSITORY_TYP_E)iRepoTyp;
	strLocation = pConfig->Read(wxT("location"), wxEmptyString);
	strExtension = pConfig->Read(wxT("extension"), wxEmptyString);

	// check settings

	// the name must not be empty
	if( strName.IsEmpty() )
	{
		wxLogWarning(_("repository entry %d has no name, ignore entry"), iIndex);
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
				wxLogWarning(_("repository entry %d has no location, ignore entry"), iIndex);
			}
			else if( strExtension.IsEmpty() )
			{
				wxLogWarning(_("repository entry %d has no extension, ignore entry"), iIndex);
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
				wxLogWarning(_("repository entry %d has no location, ignore entry"), iIndex);
			}
			else
			{
				// the settings are ok, create a new item
				ptRepoCfg = new muhkuh_repository(strName);
				ptRepoCfg->SetFilelist(strLocation);
			}
			break;

		case REPOSITORY_TYP_SINGLEXML:
			// the filelist typ must have a location
			if( strLocation.IsEmpty() )
			{
				wxLogWarning(_("repository entry %d has no location, ignore entry"), iIndex);
			}
			else
			{
				// the settings are ok, create a new item
				ptRepoCfg = new muhkuh_repository(strName);
				ptRepoCfg->SetSingleXml(strLocation);
			}
			break;

		default:
			// here end up all unknown types, show an errormessage
			wxLogWarning(_("repository entry %d has invalid typ, ignore entry"), iIndex);
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
}


bool muhkuh_repository::createTestlist(wxProgressDialog *ptScannerProgress)
{
	bool fResult;


	// default result is 'error'
	fResult = false;

	// call the custom scan routine
	switch( m_eTyp )
	{
	case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
		fResult = createTestlist_local(ptScannerProgress);
		if( fResult==false )
		{
			wxLogError(_("failed to scan the test directory"));
		}
		break;

	case muhkuh_repository::REPOSITORY_TYP_FILELIST:
		fResult = createTestlist_url(ptScannerProgress);
		if( fResult==false )
		{
			wxLogError(_("failed to open the repository"));
		}
		break;

	case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
		fResult = createTestlist_singlexml(ptScannerProgress);
		if( fResult==false )
		{
			wxLogError(_("failed to open the repository"));
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
	return astrTestList.GetCount();
}


wxString muhkuh_repository::getTestlistPrintUrl(size_t sizTestIdx) const
{
	wxString strResult;


	// check test index
	if( sizTestIdx>=0 && sizTestIdx<astrTestList.GetCount() )
	{
		strResult  = astrTestList.Item(sizTestIdx);
	}

	return strResult;
}


wxString muhkuh_repository::getTestlistBaseUrl(size_t sizTestIdx) const
{
	wxString strResult;
	wxFileName filename;
	int iLastSlash;


	// check test index
	if( sizTestIdx>=0 && sizTestIdx<astrTestList.GetCount() )
	{
		switch( m_eTyp )
		{
		case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
		case muhkuh_repository::REPOSITORY_TYP_FILELIST:
			// dirscan and filelist tests are packed mtd's
			strResult  = astrTestList.Item(sizTestIdx);
			strResult += wxT("#zip:");
			break;

		case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
			// single xml points directly to the xml file
			strResult = astrTestList.Item(sizTestIdx);
			// get position of last slash
			iLastSlash = strResult.Find(wxT('/'), true);
			if( iLastSlash!=wxNOT_FOUND )
			{
				// cut off lastt path element (that's the xml description)
				strResult.Truncate(iLastSlash);
			}
			break;

		default:
			break;
		}
	}

	return strResult;
}


wxString muhkuh_repository::getTestlistXmlUrl(size_t sizTestIdx) const
{
	wxString strResult;


	if( sizTestIdx>=0 && sizTestIdx<astrTestList.GetCount() )
	{
		switch( m_eTyp )
		{
		case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
		case muhkuh_repository::REPOSITORY_TYP_FILELIST:
			// dirscan and filelist tests are packed mtd's
			strResult  = astrTestList.Item(sizTestIdx);
			strResult += wxT("#zip:");
			strResult += wxT("test_description.xml");
			break;

		case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
			// single xml points directly to the xml file
			strResult = astrTestList.Item(sizTestIdx);
			break;

		default:
			break;
		}
	}

	return strResult;
}


bool muhkuh_repository::createTestlist_local(wxProgressDialog *ptScannerProgress)
{
	wxString strProgressMessage;
	wxFileName fileName;
	wxString strFilename;
	wxString strCompletePath;
	wxString strUrl;
	wxFileSystem fileSystem;
	bool fScannerIsRunning;


	// clear any old tests
	astrTestList.Clear();

	// no idea how long the scanning will take -> set to pulse (that's 'unknown time remaining)
	strProgressMessage.Printf(_("scanning local folder '%s' for tests..."), m_strLocation.fn_str());
	fScannerIsRunning = ptScannerProgress->Pulse(strProgressMessage, NULL);
	// update gui for new scanner message
	wxTheApp->Yield();
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
			wxLogError(_("failed to set path, IsOK returned false"));
			return false;
		}
		// does the path point to a directory?
		if( !fileName.IsDir() )
		{
			// no, the path does not point to a directory
			wxLogError(_("the path does not point to a directory"));
			return false;
		}
		// does the directory exist?
		if( !fileName.DirExists() )
		{
			// no, the directory does not exist
			wxLogError(_("the directory does not exist"));
			return false;
		}

		// the path seems to be ok
		strCompletePath = fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
		// convert path to url
		strUrl = wxFileSystem::FileNameToURL(strCompletePath);
		wxLogDebug(_("scanning testdescriptions at '%s'"), strUrl.fn_str());

		// set path
		fileSystem.ChangePathTo(strUrl, true);

		// loop over all files with the requested extension in the directory
		strFilename = fileSystem.FindFirst(m_strExtension, wxFILE);
		while( fScannerIsRunning==true && strFilename.IsEmpty()==false )
		{
			fScannerIsRunning = ptScannerProgress->Pulse(strProgressMessage, NULL);
			// update gui for new scanner message
			wxTheApp->Yield();

			// convert to url
			strUrl = wxFileSystem::FileNameToURL(strFilename);
			wxLogDebug(_("found '%s'"), strUrl.fn_str());
			astrTestList.Add(strUrl);
			strFilename = fileSystem.FindNext();
		}
	}

	// completed the list if the scanner was not canceled
	return fScannerIsRunning;
}


bool muhkuh_repository::createTestlist_url(wxProgressDialog *ptScannerProgress)
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


	// clear any old tests
	astrTestList.Clear();

	// get the repository location
	strLocation = m_strLocation;

	// no idea how long the scanning will take -> set to pulse (that's 'unknown time remaining)
	strProgressMessage.Printf(_("scanning repository '%s' for tests..."), m_strLocation.fn_str());
	fScannerIsRunning = ptScannerProgress->Pulse(strProgressMessage, NULL);
	// update gui for new scanner message
	wxTheApp->Yield();

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
			wxMessageBox(strMessage, _("Invalid URL"), wxOK|wxICON_ERROR);
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
				strMessage.Printf(_("Failed to open filelist at '%s'"), strLocation.fn_str());
				wxMessageBox(strMessage, _("Error!"), wxOK|wxICON_ERROR);
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
					fScannerIsRunning = ptScannerProgress->Pulse(strProgressMessage, NULL);
					// update gui for new scanner message
					wxTheApp->Yield();
				}
				delete ptTextInputStream;
				delete ptFsFile;

				// normalize the paths and convert to url
				sizCnt = 0;
				while( fScannerIsRunning==true && sizCnt<astrTmpPathNames.GetCount() )
				{
					strFilename = astrTmpPathNames.Item(sizCnt++);

					// check for cancel button
					strProgressMessage.Printf(_("checking '%s'"), strFilename.fn_str());
					fScannerIsRunning = ptScannerProgress->Pulse(strProgressMessage, NULL);
					// update gui for new scanner message
					wxTheApp->Yield();

					// set initial path, that's important for relative paths (almost all filelists)
					fileSystem.ChangePathTo(strLocation, false);
					ptFsFile = fileSystem.OpenFile(strFilename);
					if( ptFsFile==NULL )
					{
						wxLogError(_("failed to open '%s' from filelist."), strFilename.fn_str());
					}
					else
					{
						strUrl = ptFsFile->GetLocation();
						wxLogMessage(_("found '%s'"), strUrl.fn_str());
						astrTestList.Add(strUrl);
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


bool muhkuh_repository::createTestlist_singlexml(wxProgressDialog *ptScannerProgress)
{
	wxString strProgressMessage;
	wxFileName fileName;
	wxString strFilename;
	wxString strCompletePath;
	wxString strUrl;
	wxFileSystem fileSystem;
	bool fScannerIsRunning;


	// clear any old tests
	astrTestList.Clear();

	// no idea how long the scanning will take -> set to pulse (that's 'unknown time remaining)
	strProgressMessage.Printf(_("scanning single xml file '%s'..."), m_strLocation.fn_str());
	fScannerIsRunning = ptScannerProgress->Pulse(strProgressMessage, NULL);
	// update gui for new scanner message
	wxTheApp->Yield();
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
			wxLogError(_("failed to set path, IsOK returned false"));
			return false;
		}
		// does the path point to a file (i.e. not a directory)?
		if( fileName.IsDir()!=false )
		{
			// no, the path points to a directory
			wxLogError(_("the path does not point to a file"));
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

		// the path seems to be ok
		strCompletePath = fileName.GetFullPath(wxPATH_NATIVE);
		// convert path to url
		strUrl = wxFileSystem::FileNameToURL(strCompletePath);
		wxLogDebug(_("found '%s'"), strUrl.fn_str());
		astrTestList.Add(strUrl);
	}

	// completed the list if the scanner was not canceled
	return fScannerIsRunning;
}
