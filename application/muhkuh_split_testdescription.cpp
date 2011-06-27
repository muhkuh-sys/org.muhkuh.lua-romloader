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


#include "muhkuh_split_testdescription.h"


#include <wx/filename.h>
#include <wx/log.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>


muhkuh_split_testdescription::muhkuh_split_testdescription(void)
{
}


muhkuh_split_testdescription::~muhkuh_split_testdescription(void)
{
}


bool muhkuh_split_testdescription::split(wxString strWorkingFolder)
{
	bool fResult;
	wxFileName tFileName;
	wxString strXmlFullPath;
	wxXmlNode *ptNodeTestDescription;
	size_t sizSubTestIndex;
	wxXmlNode *ptNode;


	/* Set the working folder. */
	m_strWorkingFolder = strWorkingFolder;

	/* Create the full path to the test description. */
	tFileName.AssignDir(strWorkingFolder);
	tFileName.SetFullName("test_description.xml");
	strXmlFullPath = tFileName.GetFullPath();

	/* Does the test description exist and is it readable? */
	if( tFileName.FileExists()!=true )
	{
		wxLogError(_("The file %s does not exist!"), strXmlFullPath);
		fResult = false;
	}
	else if( tFileName.IsFileReadable()!=true )
	{
		wxLogError(_("The file %s can not be read!"), strXmlFullPath);
		fResult = false;
	}
	else
	{
		/* Ok, we can access the file -> parse the XML tree. */
		fResult = m_tXmlDoc.Load(strXmlFullPath);
		if( fResult!=true )
		{
			/* FIXME: How can I get more information what went wrong here? */
			wxLogError(_("Failed to load the XML document!"));
		}
		else
		{
			/* Search the TestDescription node. */
			ptNodeTestDescription = search_node(m_tXmlDoc.GetRoot(), "TestDescription");
			if( ptNodeTestDescription==NULL )
			{
				wxLogError(_("Can not find the TestDescription node!"));
				fResult = false;
			}
			else
			{
				fResult = generate_description(ptNodeTestDescription);
				if( fResult==true )
				{
					sizSubTestIndex = 0;

					/* Add the init code block. */
					fResult = subtests_read_test(ptNodeTestDescription, sizSubTestIndex);
					if( fResult==true )
					{
						++sizSubTestIndex;

						/* Search all subtests. */
						ptNode = ptNodeTestDescription->GetChildren();
						while( ptNode!=NULL )
						{
							if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()=="Test" )
							{
								fResult = subtests_read_test(ptNode, sizSubTestIndex);
								if( fResult!=true )
								{
									break;
								}
								++sizSubTestIndex;
							}
							ptNode = ptNode->GetNext();
						}
					}
				}
			}
		}
	}

	return fResult;
}


bool muhkuh_split_testdescription::generate_description(wxXmlNode *ptNodeTestDescription)
{
	wxArrayString astrTestNames;
	wxArrayString astrTestVersions;
	wxArrayString astrTestDescription;
	wxXmlNode *ptNode;
	size_t sizSubTestIndex;
	wxString strArg;
	bool fResult;


	/* Get the name and version attribute. */
	astrTestNames.Add(ptNodeTestDescription->GetAttribute("name", wxEmptyString));
	astrTestVersions.Add(ptNodeTestDescription->GetAttribute("version", wxEmptyString));
	/* Search all subtests. */
	ptNode = ptNodeTestDescription->GetChildren();
	while( ptNode!=NULL )
	{
		if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()=="Test" )
		{
			astrTestNames.Add(ptNode->GetAttribute("name", wxEmptyString));
			astrTestVersions.Add(ptNode->GetAttribute("version", wxEmptyString));
		}
		ptNode = ptNode->GetNext();
	}
	/* Write all test names and versions to the file "test_description.lua". */
	astrTestDescription.Add("_G.__MUHKUH_ALL_TESTS = {\n");
	for(sizSubTestIndex=0; sizSubTestIndex<astrTestNames.GetCount(); ++sizSubTestIndex)
	{
		strArg.Printf("\t[%d] = { [\"name\"]=\"%s\", [\"version\"]=\"%s\" },\n", sizSubTestIndex, astrTestNames.Item(sizSubTestIndex), astrTestVersions.Item(sizSubTestIndex));
		astrTestDescription.Add(strArg);
		
	}
	astrTestDescription.Add("}\n");

	/* Write this to a file. */
	fResult = write_textfile(MUHKUH_TESTDESCRIPTION_TYP_DESCRIPTION, 0, astrTestDescription);

	return fResult;
}


bool muhkuh_split_testdescription::subtests_read_test(wxXmlNode *ptParent, size_t sizSubTestIndex)
{
	bool fResult;
	wxXmlNode *ptNode;
	wxString strData;
	wxArrayString astrParameter;
	wxString strParameterName;
	wxString strParameterValue;


	/* Expect failure. */
	fResult = false;

	/* Search the code node. */
	ptNode = search_node(ptParent->GetChildren(), "Code");
	if( ptNode!=NULL )
	{
		/* Get the node contents. */
		strData = ptNode->GetNodeContent();
		fResult = write_textfile(MUHKUH_TESTDESCRIPTION_TYP_CODE, sizSubTestIndex, strData);
		if( fResult==true )
		{
			/* Collect all parameters. */
			ptNode = ptParent->GetChildren();
			while( ptNode!=NULL )
			{
				if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()=="Parameter" )
				{
					/* Get the name parameter. */
					if( ptNode->GetAttribute("name", &strParameterName)==false )
					{
						wxLogError(_("The parameter has no name attribute."));
						fResult = false;
						break;
					}

					/* Get the value parameter. */
					strParameterValue = ptNode->GetNodeContent();

					/* Combine the name and value. */
					strData.Printf("_G.__MUHKUH_TEST_PARAMETER[\"%s\"] = \"%s\"\n", strParameterName, strParameterValue);
					astrParameter.Add(strData);
				}
				ptNode = ptNode->GetNext();
			}

			/* Write all parameters to a file. */
			fResult = write_textfile(MUHKUH_TESTDESCRIPTION_TYP_PARAMETER, sizSubTestIndex, astrParameter);
		}
	}

	return fResult;
}


wxXmlNode *muhkuh_split_testdescription::search_node(wxXmlNode *ptNode, wxString strName)
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


wxString muhkuh_split_testdescription::get_lua_filename(MUHKUH_TESTDESCRIPTION_TYP_T tTyp, size_t sizSubTextIndex)
{
	wxFileName tFileName;
	wxString strFileName;


	/* Construct the name and extension part of the filename. */
	switch( tTyp )
	{
		case MUHKUH_TESTDESCRIPTION_TYP_DESCRIPTION:
			strFileName = "test_description.lua";
			break;

		case MUHKUH_TESTDESCRIPTION_TYP_CODE:
			strFileName.Printf("test_description_%d_code.lua", sizSubTextIndex);
			break;

		case MUHKUH_TESTDESCRIPTION_TYP_PARAMETER:
			strFileName.Printf("test_description_%d_par.lua", sizSubTextIndex);
			break;
	}

	/* Construct the complete path. */
	tFileName.AssignDir(m_strWorkingFolder);
	tFileName.SetFullName(strFileName);

	return tFileName.GetFullPath();
}


bool muhkuh_split_testdescription::write_textfile(MUHKUH_TESTDESCRIPTION_TYP_T tTyp, size_t sizSubTextIndex, wxString strContents)
{
	bool fResult;
	wxString strFileName;
	wxFFileOutputStream *ptOutputStream;
	wxTextOutputStream *ptTextOutputStream;


	/* Create a new file. */
	strFileName = get_lua_filename(tTyp, sizSubTextIndex);
	ptOutputStream = new wxFFileOutputStream(strFileName, "w");
	if( ptOutputStream->IsOk()!=true )
	{
		wxLogError("Failed to create new file %s!", strFileName);
		fResult = false;
	}
	else
	{
		/* Create the text output stream. */
		ptTextOutputStream = new wxTextOutputStream(*ptOutputStream);

		/* Write the complete data to the file. */
		ptTextOutputStream->WriteString(strContents);

		delete ptTextOutputStream;

		ptOutputStream->Close();

		fResult = true;
	}
	delete ptOutputStream;

	return fResult;
}


bool muhkuh_split_testdescription::write_textfile(MUHKUH_TESTDESCRIPTION_TYP_T tTyp, size_t sizSubTextIndex, wxArrayString &astrContents)
{
	bool fResult;
	wxString strFileName;
	wxFFileOutputStream *ptOutputStream;
	wxTextOutputStream *ptTextOutputStream;
	size_t sizStringCnt;
	size_t sizStringEnd;


	/* Create a new file. */
	strFileName = get_lua_filename(tTyp, sizSubTextIndex);
	ptOutputStream = new wxFFileOutputStream(strFileName, "w");
	if( ptOutputStream->IsOk()!=true )
	{
		wxLogError("Failed to create new file %s!", strFileName);
		fResult = false;
	}
	else
	{
		/* Create the text output stream. */
		ptTextOutputStream = new wxTextOutputStream(*ptOutputStream);

		/* Write the complete data to the file. */
		sizStringCnt = 0;
		sizStringEnd = astrContents.GetCount();
		while( sizStringCnt<sizStringEnd )
		{
			ptTextOutputStream->WriteString(astrContents.Item(sizStringCnt));
			++sizStringCnt;
		}
		delete ptTextOutputStream;

		ptOutputStream->Close();

		fResult = true;
	}
	delete ptOutputStream;

	return fResult;
}
