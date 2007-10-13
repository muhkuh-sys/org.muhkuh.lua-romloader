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
#include <wx/artprov.h>
#include <wx/fileconf.h>
#include <wx/imaglist.h>
#include <wx/progdlg.h>


#ifndef __MUHKUH_REPOSITORY_H__
#define __MUHKUH_REPOSITORY_H__


class muhkuh_repository
{
public:
	// NOTE: these values are written to the config file. do not change existing entries or old configs will be messed up!
	typedef enum
	{
		REPOSITORY_TYP_UNDEFINED		= -1,			// unconfigured repository object
		REPOSITORY_TYP_DIRSCAN			= 0,			// scan a complete directory for one extension
		REPOSITORY_TYP_FILELIST			= 1,			// get the testlist from a file
		REPOSITORY_TYP_SINGLEXML		= 2			// use one single unpacked test
	} REPOSITORY_TYP_E;


	muhkuh_repository(wxString strName);
	muhkuh_repository(const muhkuh_repository *ptRepository);

	void Assign(const muhkuh_repository *ptRepository);

	void SetName(wxString strName);
	void SetDirscan(wxString strLocation, wxString strExtension);
	void SetFilelist(wxString strLocation);
	void SetSingleXml(wxString strLocation);
	void SetSelected(bool fSelected);

	wxString GetName(void) const;
	REPOSITORY_TYP_E GetTyp(void) const;
	wxString GetLocation(void) const;
	wxString GetExtension(void) const;
	bool GetSelected(void) const;

	wxString GetStringRepresentation(void) const;
	wxBitmap GetBitmap(wxSize tBitmapSize) const;
	static wxBitmap GetBitmap(REPOSITORY_TYP_E eTyp, wxSize tBitmapSize);
	static wxImageList *CreateNewImageList(wxSize tBitmapSize);
	static int GetImageListIndex(REPOSITORY_TYP_E eTyp);
	int GetImageListIndex(void) const;

	static muhkuh_repository *CreateFromConfig(wxConfigBase *pConfig, int iIndex);
	void write_config(wxConfigBase *pConfig) const;

	bool createTestlist(wxProgressDialog *ptScannerProgress);
	size_t getTestlistCount(void) const;
	wxString getTestlistPrintUrl(size_t sizTestIdx) const;
	wxString getTestlistBaseUrl(size_t sizTestIdx) const;
	wxString getTestlistXmlUrl(size_t sizTestIdx) const;

private:
	bool createTestlist_local(wxProgressDialog *ptScannerProgress);
	bool createTestlist_url(wxProgressDialog *ptScannerProgress);
	bool createTestlist_singlexml(wxProgressDialog *ptScannerProgress);

	wxString m_strName;			// user defined name
	REPOSITORY_TYP_E m_eTyp;		// typ of this repository
	wxString m_strLocation;			// directory for dirscan typ or url of the filelist for filelist typ
	wxString m_strExtension;		// dirscan only, that's the extension to look for

	bool m_fSelected;			// flag for 'selected in the combo box, used to keep an item selected over a change of configration

	// the tests found in this repository
	wxArrayString astrTestList;
};


#endif	/* __MUHKUH_REPOSITORY_H__ */

