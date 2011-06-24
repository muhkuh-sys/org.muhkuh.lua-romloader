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


#include <wx/xml/xml.h>

#ifndef __muhkuh_wrap_xml_h__
#define __muhkuh_wrap_xml_h__


typedef struct
{
	wxString strName;
	wxString strVersion;                    // version string
} tTesterXml_Test;

typedef struct
{
	wxString strName;                       // name of the testdescription
	wxString strVersion;                    // version string
	unsigned int uiTests;                   // number of tests
	tTesterXml_Test *ptTests;
} tTesterXml_TestDescription;


typedef struct
{
	char *pcName;
	char *pcValue;
} MTD_SUBTEST_PARAMETER_T;

typedef struct
{
	char *pcCode;
	size_t sizParameter;
	MTD_SUBTEST_PARAMETER_T *ptParameter;
} MTD_SUBTEST_T;



class muhkuh_wrap_xml
{
public:
	muhkuh_wrap_xml(void);
	~muhkuh_wrap_xml(void);

	bool initialize(wxInputStream *pStream, size_t sizRepositoryIdx, size_t sizTestIdx);

	size_t getRepositoryIndex(void) const;
	size_t getTestIndex(void) const;

	wxString testDescription_getName(void) const;
	wxString testDescription_getVersion(void) const;
	unsigned int testDescription_getTestCnt(void) const;
	bool testDescription_setTest(unsigned int uiTestIdx);

	wxString test_getName(void) const;
	wxString test_getVersion(void) const;

	bool subtests_parse(void);
	const MTD_SUBTEST_T *subtests_get(unsigned int uiIdx) const;
	void subtests_free(void);

private:
	bool subtests_read_test(wxXmlNode *ptParent, MTD_SUBTEST_T *ptSubtest);

	bool readTestDescription(wxXmlDocument *xmldoc);
	bool readTest(wxXmlNode *xml_parent, tTesterXml_TestDescription *ptTestDesc);

	wxXmlNode *search_node(wxXmlNode *ptNode, wxString strName);

	wxXmlDocument *m_xml_doc;

	tTesterXml_TestDescription *m_ptTestDescription;
	tTesterXml_Test *m_ptTest;

	size_t m_sizRepositoryIdx;
	size_t m_sizTestIdx;

	size_t m_sizSubtests;
	MTD_SUBTEST_T *m_ptSubtests;
};


#endif	// __muhkuh_wrap_xml_h__
