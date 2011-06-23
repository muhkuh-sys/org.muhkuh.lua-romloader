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
#include <wx/log.h>

#include <stdlib.h>


#include "muhkuh_wrap_xml.h"


muhkuh_wrap_xml::muhkuh_wrap_xml(void)
 : m_xml_doc(NULL)
 , m_ptTestDescription(NULL)
 , m_ptTest(NULL)
 , m_sizSubtests(0)
 , m_ptSubtests(NULL)
{
}


muhkuh_wrap_xml::~muhkuh_wrap_xml(void)
{
	tTesterXml_Test *ts;


	if( m_xml_doc!=NULL )
	{
		delete m_xml_doc;
	}

	if( m_ptTestDescription!=NULL )
	{
		// loop over all tests
		ts = m_ptTestDescription->ptTests;
		if( ts!=NULL )
		{
			delete[] ts;
		}
		delete m_ptTestDescription;
	}

	/* Free all subtests. */
	if( m_ptSubtests!=NULL )
	{
		subtests_free();
	}
}


bool muhkuh_wrap_xml::initialize(wxInputStream *pStream, size_t sizRepositoryIdx, size_t sizTestIdx)
{
	bool fOk;
	wxXmlDocument *xmldoc;


	// set the test ids
        m_sizRepositoryIdx = sizRepositoryIdx;
	m_sizTestIdx = sizTestIdx;

	// assume failure
	fOk = false;

	// create the new xml document
	xmldoc = new wxXmlDocument();
	if( xmldoc==NULL )
	{
		// failed to create the new xml document
		wxLogFatalError(_("could not create xml object"));
	}
	else
	{
		// xml document created -> load the xml file
		if( xmldoc->Load(*pStream)!=true )
		{
			// failed to load the xml file
			wxLogError(_("failed to load the test description from the input stream"));
			delete xmldoc;
		}
		else
		{
			// xml file loaded -> interpret the data
			if( readTestDescription(xmldoc)!=true )
			{
				// strange test description
				wxLogError(_("could not read test"));
				delete xmldoc;
			}
			else
			{
				// ok
				fOk = true;
				m_xml_doc = xmldoc;
			}
		}
	}

	return fOk;
}


size_t muhkuh_wrap_xml::getRepositoryIndex(void) const
{
	return m_sizRepositoryIdx;
}


size_t muhkuh_wrap_xml::getTestIndex(void) const
{
	return m_sizTestIdx;
}

/*
wxXmlDocument *muhkuh_wrap_xml::getXmlDocument(void)
{
	return m_xml_doc;
}
*/

wxString muhkuh_wrap_xml::testDescription_getName(void) const
{
	// does a testdescription exist?
	if( m_ptTestDescription==NULL )
	{
		return wxEmptyString;
	}

	// yes -> get the name
	return m_ptTestDescription->strName;
}


wxString muhkuh_wrap_xml::testDescription_getVersion(void) const
{
	// does a testdescription exist?
	if( m_ptTestDescription==NULL )
	{
		return wxEmptyString;
	}

	// yes -> get the name
	return m_ptTestDescription->strVersion;
}

/*
wxString muhkuh_wrap_xml::testDescription_getCode(void) const
{
	// does a testdescription exist?
	if( m_ptTestDescription==NULL )
	{
		return wxEmptyString;
	}

	// yes -> get the name
	return m_ptTestDescription->strCode;
}
*/

unsigned int muhkuh_wrap_xml::testDescription_getTestCnt(void) const
{
	// does a testdescription exist?
	if( m_ptTestDescription==NULL )
	{
		return 0;
	}

	// yes -> get the test counter
	return m_ptTestDescription->uiTests;
}


bool muhkuh_wrap_xml::testDescription_setTest(unsigned int uiTestIdx)
{
	// does a testdescription exist?
	if( m_ptTestDescription==NULL )
	{
		return false;
	}

	// does the test index exist?
	if( uiTestIdx>=m_ptTestDescription->uiTests )
	{
		return false;
	}

	// ok, set the new test
	m_ptTest = m_ptTestDescription->ptTests + uiTestIdx;

	return true;
}


wxString muhkuh_wrap_xml::test_getName(void) const
{
	// does the test exist?
	if( m_ptTest==NULL )
	{
		return wxEmptyString;
	}

	// return the name
	return m_ptTest->strName;
}


wxString muhkuh_wrap_xml::test_getVersion(void) const
{
	// does a testdescription exist?
	if( m_ptTest==NULL )
	{
		return wxEmptyString;
	}

	// yes -> get the name
	return m_ptTest->strVersion;
}

/*
wxString muhkuh_wrap_xml::test_getCode(void) const
{
	// does a testdescription exist?
	if( m_ptTest==NULL )
	{
		return wxEmptyString;
	}

	// yes -> get the name
	return m_ptTest->strCode;
}
*/


bool muhkuh_wrap_xml::subtests_read_test(wxXmlNode *ptParent, MTD_SUBTEST_T *ptSubtest)
{
	bool fOk;
	wxXmlNode *ptNode;
	size_t sizData;
	wxString strData;
	size_t sizParameter;
	MTD_SUBTEST_PARAMETER_T *ptParameter;


	/* Expect success. */
	fOk = true;

	/* Search the code node. */
	ptNode = search_node(ptParent->GetChildren(), wxT("Code"));
	if( ptNode!=NULL )
	{
		/* Get the node contents. */
		strData = ptNode->GetNodeContent();
		/* Allocate the buffer for the code string and the trailing 0. */
		sizData = strData.Len() + 1;
		ptSubtest->pcCode = (char*)malloc(sizData);
		if( ptSubtest->pcCode==NULL )
		{
			wxLogError(_("Out of memory!"));
			fOk = false;
		}
		else
		{
			memcpy(ptSubtest->pcCode, strData.fn_str(), sizData);

			/* Count the parameters. */
			sizParameter = 0;
			ptNode = ptParent->GetChildren();
			while( ptNode!=NULL )
			{
				if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()==wxT("Parameter") )
				{
					++sizParameter;
				}
				ptNode = ptNode->GetNext();
			}

			ptSubtest->sizParameter = sizParameter;
			if( sizParameter==0 )
			{
				ptSubtest->ptParameter = NULL;
			}
			else
			{
				ptSubtest->ptParameter = (MTD_SUBTEST_PARAMETER_T*)calloc(sizParameter, sizeof(MTD_SUBTEST_PARAMETER_T));
				if( ptSubtest->ptParameter==NULL )
				{
					wxLogError(_("Out of memory!"));
					fOk = false;
				}
				else
				{
					ptParameter = ptSubtest->ptParameter;
					ptNode = ptParent->GetChildren();
					while( ptNode!=NULL )
					{
						if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()==wxT("Parameter") )
						{
							/* Get the name parameter. */
							if( ptNode->GetAttribute(wxT("name"), &strData)==false )
							{
								wxLogError(_("The parameter has no name attribute."));
								fOk = false;
								break;
							}
							/* Allocate the buffer for the code string and the trailing 0. */
							sizData = strData.Len() + 1;
							ptParameter->pcName = (char*)malloc(sizData);
							if( ptParameter->pcName==NULL )
							{
								wxLogError(_("Out of memory!"));
								fOk = false;
								break;
							}
							memcpy(ptParameter->pcName, strData.fn_str(), sizData);

							/* Get the value parameter. */
							strData = ptNode->GetNodeContent();
							/* Allocate the buffer for the code string and the trailing 0. */
							sizData = strData.Len() + 1;
							ptParameter->pcValue = (char*)malloc(sizData);
							if( ptParameter->pcValue==NULL )
							{
								wxLogError(_("Out of memory!"));
								fOk = false;
								break;
							}
							memcpy(ptParameter->pcValue, strData.fn_str(), sizData);

							++ptParameter;
						}
						ptNode = ptNode->GetNext();
					}
				}
			}
		}
	}

	return fOk;
}


bool muhkuh_wrap_xml::subtests_parse(void)
{
	bool fResult;
	wxXmlNode *ptNodeTestDescription;
	MTD_SUBTEST_T *ptSubtestCnt;
	MTD_SUBTEST_T *ptSubtestEnd;
	wxXmlNode *ptNode;


	/* Expect success. */
	fResult = true;

	/* Does a testdescription exist? */
	if( m_ptTestDescription==NULL )
	{
		wxLogError(_("The xml file was not parsed yet."));
		fResult = false;
	}
	else
	{
		/* Allocate a new array with one global entry and one for each test. */
		m_sizSubtests = m_ptTestDescription->uiTests + 1;
		m_ptSubtests = (MTD_SUBTEST_T*)calloc(m_sizSubtests, sizeof(MTD_SUBTEST_T));
		if( m_ptSubtests==NULL )
		{
			wxLogError(_("Out of memory error!"));
			fResult = false;
		}
		else
		{
			/* Search the TestDescription node. */
			ptNodeTestDescription = search_node(m_xml_doc->GetRoot(), wxT("TestDescription"));
			if( ptNodeTestDescription==NULL )
			{
				wxLogError(_("Can not find the TestDescription node!"));
				fResult = false;
			}
			else
			{
				ptSubtestCnt = m_ptSubtests;
				ptSubtestEnd = m_ptSubtests + m_sizSubtests;

				/* Add the init code block. */
				fResult = subtests_read_test(ptNodeTestDescription, ptSubtestCnt);
				if( fResult==true )
				{
					++ptSubtestCnt;

					/* Search all subtests. */
					ptNode = ptNodeTestDescription->GetChildren();
					while( ptNode!=NULL )
					{
						if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()==wxT("Test") )
						{
							fResult = subtests_read_test(ptNode, ptSubtestCnt);
							if( fResult!=true )
							{
								break;
							}
							++ptSubtestCnt;
						}
						ptNode = ptNode->GetNext();
					}
				}
			}
		}
	}

	if( fResult!=true )
	{
		subtests_free();
	}

	return fResult;
}


const MTD_SUBTEST_T *muhkuh_wrap_xml::subtests_get(unsigned int uiIdx) const
{
	const MTD_SUBTEST_T *ptSubtest;


	/* Is the index valid? */
	ptSubtest = NULL;
	if( uiIdx<m_sizSubtests )
	{
		ptSubtest = m_ptSubtests + uiIdx;
	}
	return ptSubtest;
}


void muhkuh_wrap_xml::subtests_free(void)
{
	MTD_SUBTEST_T *ptTestCnt;
	MTD_SUBTEST_T *ptTestEnd;
	MTD_SUBTEST_PARAMETER_T *ptParamCnt;
	MTD_SUBTEST_PARAMETER_T *ptParamEnd;


	if( m_ptSubtests!=NULL )
	{
		ptTestCnt = m_ptSubtests;
		ptTestEnd = m_ptSubtests + m_sizSubtests;
		while( ptTestCnt<ptTestEnd )
		{
			/* Free the code. */
			if( ptTestCnt->pcCode!=NULL )
			{
				free(ptTestCnt->pcCode);
			}
			/* Free all parameters. */
			if( ptTestCnt->ptParameter!=NULL )
			{
				ptParamCnt = ptTestCnt->ptParameter;
				ptParamEnd = ptTestCnt->ptParameter + ptTestCnt->sizParameter;
				while( ptParamCnt<ptParamEnd )
				{
					/* Free the name. */
					if( ptParamCnt->pcName!=NULL )
					{
						free(ptParamCnt->pcName);
					}
					/* Free the value. */
					if( ptParamCnt->pcValue!=NULL )
					{
						free(ptParamCnt->pcValue);
					}
					++ptParamCnt;
				}
				free(ptTestCnt->ptParameter);
			}
			++ptTestCnt;
		}
		free(m_ptSubtests);
		m_ptSubtests = NULL;
	}
}



wxXmlNode *muhkuh_wrap_xml::search_node(wxXmlNode *ptNode, wxString strName)
{
	while( ptNode!=NULL )
	{
		if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()==strName )
		{
			break;
		}
		ptNode = ptNode->GetNext();
	}

	return ptNode;
}


bool muhkuh_wrap_xml::readTestDescription(wxXmlDocument *xmldoc)
{
	wxXmlNode *xml_testdesc;
	tTesterXml_TestDescription *ptTestDesc;


	// look for the first root node named "TestDescription"
	xml_testdesc = xmldoc->GetRoot();
	while( xml_testdesc!=NULL )
	{
		if( xml_testdesc->GetType()==wxXML_ELEMENT_NODE && xml_testdesc->GetName()==wxT("TestDescription") )
		{
			break;
		}
		xml_testdesc = xml_testdesc->GetNext();
	}
	// not found (node is NULL) ?
	if( xml_testdesc==NULL )
	{
		return false;
	}

	// create new test description
	ptTestDesc = new tTesterXml_TestDescription;

	// get the testdescription name
	if( xml_testdesc->GetAttribute(wxT("name"), &ptTestDesc->strName)==false )
	{
		delete ptTestDesc;
		return false;
	}
/*
	// read the code node
	if( readCodeNode(xml_testdesc, ptTestDesc->strCode)==false )
	{
		delete ptTestDesc;
		return false;
	}
*/
	// get the testdescription version
	// NOTE: this is optional to support older tests
	if( xml_testdesc->GetAttribute(wxT("version"), &ptTestDesc->strVersion)==false )
	{
		// show a warning
		wxLogWarning(_("testdescription '%s' has no version information"), ptTestDesc->strName.c_str());
	}

	// read in all tests
	if( readTest(xml_testdesc, ptTestDesc)==false )
	{
		delete ptTestDesc;
		return false;
	}

	// publish the data
	m_ptTestDescription = ptTestDesc;
	return true;
}


bool muhkuh_wrap_xml::readTest(wxXmlNode *xml_parent, tTesterXml_TestDescription *ptTestDesc)
{
	unsigned int ulTestCnt;
	wxXmlNode *xml_test_cnt;
	tTesterXml_Test *ptTests;
	tTesterXml_Test *tc;


	// count the tests
	ulTestCnt = 0;
	ptTests = NULL;
	xml_test_cnt = xml_parent->GetChildren();
	while( xml_test_cnt!=NULL )
	{
		if( xml_test_cnt->GetType()==wxXML_ELEMENT_NODE && xml_test_cnt->GetName()==wxT("Test") )
		{
			++ulTestCnt;
		}
		xml_test_cnt = xml_test_cnt->GetNext();
	}

	// found some tests?
	if( ulTestCnt!=0 )
	{
		// allocate array
		ptTests = new tTesterXml_Test[ulTestCnt];

		// enter all elements
		tc = ptTests;
		xml_test_cnt = xml_parent->GetChildren();
		while( xml_test_cnt!=NULL )
		{
			if( xml_test_cnt->GetType()==wxXML_ELEMENT_NODE && xml_test_cnt->GetName()==wxT("Test") )
			{
				// get test name
				if( xml_test_cnt->GetAttribute(wxT("name"), &tc->strName)==false )
				{
					delete[] ptTests;
					return false;
				}
/*
				// read the code node
				if( readCodeNode(xml_test_cnt, tc->strCode)==false )
				{
					delete[] ptTests;
					return false;
				}
*/
				// get the test version
				// NOTE: this is optional to support older tests
				if( xml_test_cnt->GetAttribute(wxT("version"), &tc->strVersion)==false )
				{
					// show a warning
					wxLogWarning(_("test '%s' has no version information"), tc->strName.c_str());
				}

				// next slot in test array
				++tc;
			}

			// move to next xml element
			xml_test_cnt = xml_test_cnt->GetNext();
		}
	}

	ptTestDesc->ptTests = ptTests;
	ptTestDesc->uiTests = ulTestCnt;

	return true;
}

/*
bool muhkuh_wrap_xml::readCodeNode(wxXmlNode *xml_parent, wxString &strCode)
{
	wxXmlNode *ptNode;
	bool fResult;


	// expect failure
	fResult = false;

	// look for the first root node named "TestDescription"
	ptNode = xml_parent->GetChildren();
	while( ptNode!=NULL )
	{
		if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()==wxT("Code") )
		{
			break;
		}
		ptNode = ptNode->GetNext();
	}
	// found Code node (node is not NULL) ?
	if( ptNode!=NULL )
	{
		// get the node contents
		strCode = ptNode->GetNodeContent();
		fResult = true;
	}

	return fResult;
}
*/
