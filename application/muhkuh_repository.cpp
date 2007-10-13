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


	// start with the user defined name
	strName = m_strName;

	// add the rest as details
	strName += wxT(" (");

	// append the typ
	switch( m_eTyp )
	{
	case REPOSITORY_TYP_DIRSCAN:
		strName += wxT("directory scan in ");
		strName += m_strLocation;
		strName += wxT(" for ");
		strName += m_strExtension;
		break;
	case REPOSITORY_TYP_FILELIST:
		strName += wxT("filelist from ");
		strName += m_strLocation;
		break;
	case REPOSITORY_TYP_SINGLEXML:
		strName += wxT("single xml file from ");
		strName += m_strLocation;
		break;
	default:
		strName += wxT("unknown typ");
		break;
	}

	strName += wxT(")");

	return strName;
}


wxBitmap muhkuh_repository::GetBitmap(wxSize tBitmapSize) const
{
	return GetBitmap(m_eTyp, tBitmapSize);
}


wxBitmap muhkuh_repository::GetBitmap(REPOSITORY_TYP_E eTyp, wxSize tBitmapSize)
{
	wxArtID tArtId;


	// append the typ
	switch( eTyp )
	{
	case REPOSITORY_TYP_DIRSCAN:
		tArtId = wxART_FOLDER_OPEN;
		break;
	case REPOSITORY_TYP_FILELIST:
		tArtId = wxART_HELP_BOOK;
		break;
	case REPOSITORY_TYP_SINGLEXML:
		tArtId = wxART_NORMAL_FILE;
		break;
	default:
		tArtId = wxART_ERROR;
		break;
	}

	return wxArtProvider::GetBitmap(tArtId, wxART_MENU, tBitmapSize);
}


wxImageList *muhkuh_repository::CreateNewImageList(wxSize tBitmapSize)
{
	wxImageList *ptImageList;
	wxBitmap bitmap;


	// create the new empty list and reserve space for 4 icons
	ptImageList = new wxImageList(tBitmapSize.GetWidth(), tBitmapSize.GetHeight(), true, 4);

	// NOTE: if you change the order or amount of icons here, you must also change the GetImageListIndex function
	// add 'undefined' bitmap
	bitmap = muhkuh_repository::GetBitmap(muhkuh_repository::REPOSITORY_TYP_UNDEFINED, tBitmapSize);
	ptImageList->Add(bitmap);
	// add 'dirscan' bitmap
	bitmap = muhkuh_repository::GetBitmap(muhkuh_repository::REPOSITORY_TYP_DIRSCAN, tBitmapSize);
	ptImageList->Add(bitmap);
	// add 'filelist' bitmap
	bitmap = muhkuh_repository::GetBitmap(muhkuh_repository::REPOSITORY_TYP_FILELIST, tBitmapSize);
	ptImageList->Add(bitmap);
	// add 'singlexml' bitmap
	bitmap = muhkuh_repository::GetBitmap(muhkuh_repository::REPOSITORY_TYP_SINGLEXML, tBitmapSize);
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
	wxString strMessage;


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
		strMessage.Printf(wxT("repository entry %d has no name, ignore entry"), iIndex);
		wxLogWarning(strMessage);
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
				strMessage.Printf(wxT("repository entry %d has no location, ignore entry"), iIndex);
				wxLogWarning(strMessage);
			}
			else if( strExtension.IsEmpty() )
			{
				strMessage.Printf(wxT("repository entry %d has no extension, ignore entry"), iIndex);
				wxLogWarning(strMessage);
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
				strMessage.Printf(wxT("repository entry %d has no location, ignore entry"), iIndex);
				wxLogWarning(strMessage);
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
				strMessage.Printf(wxT("repository entry %d has no location, ignore entry"), iIndex);
				wxLogWarning(strMessage);
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
			strMessage.Printf(wxT("repository entry %d has invalid typ, ignore entry"), iIndex);
			wxLogWarning(strMessage);
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
			wxLogError(wxT("failed to scan the test directory"));
		}
		break;

	case muhkuh_repository::REPOSITORY_TYP_FILELIST:
		fResult = createTestlist_url(ptScannerProgress);
		if( fResult==false )
		{
			wxLogError(wxT("failed to open the repository"));
		}
		break;

	case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
		fResult = createTestlist_singlexml(ptScannerProgress);
		if( fResult==false )
		{
			wxLogError(wxT("failed to open the repository"));
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
			filename.Assign(astrTestList.Item(sizTestIdx));
			strResult = filename.GetPath();
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
	strProgressMessage  = wxT("scanning local folder '");
	strProgressMessage += m_strLocation;
	strProgressMessage += wxT("' for tests...");
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
			wxLogError(wxT("failed to set path, IsOK returned false"));
			return false;
		}
		// does the path point to a directory?
		if( !fileName.IsDir() )
		{
			// no, the path does not point to a directory
			wxLogError(wxT("the path does not point to a directory"));
			return false;
		}
		// does the directory exist?
		if( !fileName.DirExists() )
		{
			// no, the directory does not exist
			wxLogError(wxT("the directory does not exist"));
			return false;
		}

		// the path seems to be ok
		strCompletePath = fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
		// convert path to url
		strUrl = wxFileSystem::FileNameToURL(strCompletePath);
		wxLogDebug(wxT("scanning testdescriptions at ") + strUrl);

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
			wxLogDebug(wxT("found ") + strUrl);
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
	strProgressMessage  = wxT("scanning repository '");
	strProgressMessage += m_strLocation;
	strProgressMessage += wxT("' for tests...");
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
			strMessage  = wxT("Invalid URL : '");
			strMessage += strLocation;
			strMessage += wxT("'\n");
			// try to show some details
			switch( urlError )
			{
			case wxURL_SNTXERR:
				strMessage += wxT("Syntax error in the URL string.");
				break;
			case wxURL_NOPROTO:
				strMessage += wxT("Found no protocol which can get this URL.");
				break;
			case wxURL_NOHOST:
				strMessage += wxT("An host name is required for this protocol.");
				break;
			case wxURL_NOPATH:
				strMessage += wxT("A path is required for this protocol.");
				break;
			case wxURL_CONNERR:
				strMessage += wxT("Connection error. (did anybody press connect? should never happen!)");
				break;
			case wxURL_PROTOERR:
				strMessage += wxT("An error occurred during negotiation. (should never happen!)");
				break;
			default:
				strMessage += wxT("unknown errorcode");
				break;
			}

			// show the error message
			wxMessageBox(strMessage, wxTRANSLATE("Invalid URL"), wxOK|wxICON_ERROR);
			fScannerIsRunning = false;
		}
		else
		{
			strMessage  = wxT("Get list of testdescriptions from '");
			strMessage += strLocation;
			strMessage += wxT("'");
			wxLogMessage(strMessage);

			// test if file exists
			ptFsFile = fileSystem.OpenFile(strLocation);
			if( ptFsFile==NULL )
			{
				strMessage  = wxT("Failed to open filelist at '");
				strMessage += strLocation;
				strMessage += wxT("'");

				wxMessageBox(strMessage, wxTRANSLATE("Error!"), wxOK|wxICON_ERROR);
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
					strProgressMessage = wxT("receiving ") + strFilename;
					fScannerIsRunning = ptScannerProgress->Pulse(strProgressMessage, NULL);
					// update gui for new scanner message
					wxTheApp->Yield();
				}
				delete ptFsFile;
				delete ptTextInputStream;

				// normalize the paths and convert to url
				sizCnt = 0;
				while( fScannerIsRunning==true && sizCnt<astrTmpPathNames.GetCount() )
				{
					strFilename = astrTmpPathNames.Item(sizCnt++);

					// check for cancel button
					strProgressMessage = wxT("checking ") + strFilename;
					fScannerIsRunning = ptScannerProgress->Pulse(strProgressMessage, NULL);
					// update gui for new scanner message
					wxTheApp->Yield();

					// set initial path, that's important for relative paths (almost all filelists)
					fileSystem.ChangePathTo(strLocation, false);
					ptFsFile = fileSystem.OpenFile(strFilename);
					if( ptFsFile==NULL )
					{
						strMessage  = wxT("failed to open '");
						strMessage += strFilename;
						strMessage += wxT("' from filelist");
						wxLogError(strMessage);
					}
					else
					{
						strUrl = ptFsFile->GetLocation();
						wxLogMessage(wxT("found ") + strUrl);
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
	strProgressMessage  = wxT("scanning single xml file '");
	strProgressMessage += m_strLocation;
	strProgressMessage += wxT("'...");
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
			wxLogError(wxT("failed to set path, IsOK returned false"));
			return false;
		}
		// does the path point to a file (i.e. not a directory)?
		if( fileName.IsDir()!=false )
		{
			// no, the path points to a directory
			wxLogError(wxT("the path does not point to a file"));
			return false;
		}
		// is the file readable?
		if( fileName.IsFileReadable()!=true )
		{
			// no, the file is not readable
			wxLogError(wxT("the file is not readable"));
			return false;
		}

		// the path seems to be ok
		strCompletePath = fileName.GetFullPath(wxPATH_NATIVE);
		// convert path to url
		strUrl = wxFileSystem::FileNameToURL(strCompletePath);
		wxLogDebug(wxT("found ") + strUrl);
		astrTestList.Add(strUrl);
	}

	// completed the list if the scanner was not canceled
	return fScannerIsRunning;
}
