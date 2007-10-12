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


#include <wx/wx.h>
#include <wx/treectrl.h>

#include "muhkuh_wrap_xml.h"


#ifndef __MUHKUH_TESTTREEITEMDATA_H__
#define __MUHKUH_TESTTREEITEMDATA_H__


class testTreeItemData : public wxTreeItemData
{
public:
	testTreeItemData(const wxString &strTestLabel, wxString strHelpBook, muhkuh_wrap_xml *ptWrapXml)
	 : m_strTestLabel(strTestLabel), m_strHelpBook(strHelpBook), m_ptWrapXml(ptWrapXml) { };

	~testTreeItemData(void)
	{
		if( m_ptWrapXml!=NULL )
		{
			delete m_ptWrapXml;
		}
	};

	wxString getTestLabel(void) const { return m_strTestLabel; };
	wxString getHelpBook(void) const { return m_strHelpBook; };
	muhkuh_wrap_xml *getXmlDescription(void) const { return m_ptWrapXml; };

private:
	wxString m_strTestLabel;		// pretty print name of the test
	wxString m_strHelpBook;			// help book for this test
	muhkuh_wrap_xml *m_ptWrapXml;		// xml description of the test
};


#endif	// __MUHKUH_TESTTREEITEMDATA_H__
