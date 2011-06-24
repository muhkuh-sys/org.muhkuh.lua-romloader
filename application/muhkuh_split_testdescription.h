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
#include <wx/xml/xml.h>


#ifndef __MUHKUH_SPLIT_TESTDESCRIPTION_H__
#define __MUHKUH_SPLIT_TESTDESCRIPTION_H__


class muhkuh_split_testdescription
{
public:
	muhkuh_split_testdescription(void);
	~muhkuh_split_testdescription(void);

	bool split(wxString strWorkingFolder);

private:
	typedef enum
	{
		MUHKUH_TESTDESCRIPTION_TYP_CODE      = 0,
		MUHKUH_TESTDESCRIPTION_TYP_PARAMETER = 1
	} MUHKUH_TESTDESCRIPTION_TYP_T;

	bool subtests_read_test(wxXmlNode *ptParent, size_t sizSubTextIndex);
	wxXmlNode *search_node(wxXmlNode *ptNode, wxString strName);
	wxString get_lua_filename(MUHKUH_TESTDESCRIPTION_TYP_T tTyp, size_t sizSubTextIndex);
	bool write_textfile(MUHKUH_TESTDESCRIPTION_TYP_T tTyp, size_t sizSubTextIndex, wxString strContents);
	bool write_textfile(MUHKUH_TESTDESCRIPTION_TYP_T tTyp, size_t sizSubTextIndex, wxArrayString &astrContents);

	wxString m_strWorkingFolder;
	wxXmlDocument m_tXmlDoc;
};

#endif  /* __MUHKUH_SPLIT_TESTDESCRIPTION_H__ */

