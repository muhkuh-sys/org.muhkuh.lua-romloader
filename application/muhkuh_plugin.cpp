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


#include "muhkuh_plugin.h"

#include "muhkuh_lua.h"

#include <wx/file.h>
#include <wx/log.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>


muhkuh_plugin::muhkuh_plugin(void)
 : m_fPluginIsOk(false)
 , m_fPluginIsEnabled(false)
// , m_ptCfgNode(NULL)
 , m_strInitError(wxEmptyString)
{
	// set prefix for messages
	setMe();
}


muhkuh_plugin::muhkuh_plugin(wxString strPluginCfgPath)
 : m_fPluginIsOk(false)
 , m_fPluginIsEnabled(false)
// , m_ptCfgNode(NULL)
 , m_strInitError(wxEmptyString)
{
	// set prefix for messages
	setMe();

	// try to open the plugin
	Load(strPluginCfgPath);
}


muhkuh_plugin::muhkuh_plugin(const muhkuh_plugin *ptCloneMe)
 : m_fPluginIsOk(false)
 , m_fPluginIsEnabled(false)
// , m_ptCfgNode(NULL)
 , m_strInitError(wxEmptyString)
{
	// set prefix for messages
	setMe();

	// copy the enable flag
	m_fPluginIsEnabled = ptCloneMe->m_fPluginIsEnabled;

	// try to open the plugin
	Load(ptCloneMe->m_strPluginCfgPath);
}


muhkuh_plugin::muhkuh_plugin(wxConfigBase *pConfig)
 : m_fPluginIsOk(false)
 , m_fPluginIsEnabled(false)
// , m_ptCfgNode(NULL)
 , m_strInitError(wxEmptyString)
{
	bool fReadOk;
	wxString strPath;


	// set prefix for messages
	setMe();

	// try to get the settings
	fReadOk = pConfig->Read(wxT("path"), &strPath);
	if( fReadOk==false )
	{
		setInitError(_("missing path information in the application config file"), wxEmptyString);
	}
	else
	{
		fReadOk = pConfig->Read(wxT("enable"), &m_fPluginIsEnabled, false);
		if( fReadOk==false )
		{
			wxLogWarning(m_strMe + _("missing 'enable' entry in application config file, defaulting to 'disabled'"));
		}
		Load(strPath);
	}
}


muhkuh_plugin::~muhkuh_plugin(void)
{
//	close();
}


void muhkuh_plugin::setMe(void)
{
	m_strMe.Printf(wxT("muhkuh_plugin(%p) :"), this);
}


void muhkuh_plugin::setInitError(wxString strMessage, wxString strPath)
{
	wxString strMsg;


	// build errormessage
	m_strInitError.Printf(_("The plugin '%s' failed to load: %s"), strPath.c_str(), strMessage.c_str());
	// show message in logfile
	wxLogError(m_strMe + m_strInitError);

	// the plugin state is "not ok"
	m_fPluginIsOk = false;
}


wxXmlNode *muhkuh_plugin::find_child_node(wxXmlNode *ptNode, wxString strNodeName)
{
	wxXmlNode *ptFoundNode;


	ptFoundNode = NULL;
	while( ptNode!=NULL && ptNode->GetType()==wxXML_ELEMENT_NODE )
	{
		if( ptNode->GetName()==strNodeName )
		{
			ptFoundNode = ptNode;
			break;
		}
		ptNode = ptNode->GetNext();
	}
	return ptFoundNode;
}


bool muhkuh_plugin::openXml(wxString strXmlPath)
{
	bool fResult;
	wxFile cFile;
	wxFileInputStream *ptFileInputStream;
	wxXmlNode *ptMuhkuhNode;
	wxXmlNode *ptChildNode;
	wxXmlNode *ptVersionNode;
	wxString strVersion;
	wxString strCfgName;
	wxString strId;
	wxFileName cFileName;
	wxString strLuaModuleName;
	wxString strError;
	wxXmlDocument tXmlConfig;
	wxStringOutputStream tStringOutStream;
	long lVersionMaj;
	long lVersionMin;
	long lVersionSub;


	/* Expect failure. */
	fResult = false;

	/* Set the default plugin description. */
	m_tPluginDescription.strPluginName = strXmlPath;
	m_tPluginDescription.strPluginId = wxEmptyString;
	m_tPluginDescription.uiVersionMajor = 0;
	m_tPluginDescription.uiVersionMinor = 0;
	m_tPluginDescription.uiVersionSub = 0;

	/* Open the xml document. */
	fResult = cFile.Open(strXmlPath, wxFile::read);
	if( fResult!=true )
	{
		setInitError(_("failed to open the xml file"), strXmlPath);
	}
	else
	{
		ptFileInputStream = new wxFileInputStream(cFile);
		fResult = m_xmldoc.Load(*ptFileInputStream);
		if( fResult!=true )
		{
			/* Failed to load the xml file. */
			setInitError(_("failed to load the plugin config from the input stream"), strXmlPath);
		}
		else
		{
			/* Look for root node. */
			ptMuhkuhNode = find_child_node(m_xmldoc.GetRoot(), wxT("MuhkuhPluginConfig"));
			if( ptMuhkuhNode==NULL )
			{
				setInitError(_("Root node 'MuhkuhPluginConfig' not found. Is this really a Muhkuh plugin config?"), strXmlPath);
				fResult = false;
			}
			else
			{
				/* Expect failure. */
				fResult = false;
				/* Get name attribute. */
				if( ptMuhkuhNode->GetPropVal(wxT("name"), &strCfgName)!=true )
				{
					strError.Printf(_("The node '%s' has no property '%s'!"), wxT("MuhkuhPluginConfig"), wxT("name"));
					setInitError(strError, strXmlPath);
				}
				else if( strCfgName.IsEmpty()!=false )
				{
					strError.Printf(_("Property '%s' of node '%s' is empty!"), wxT("name"), wxT("MuhkuhPluginConfig"));
					setInitError(strError, strXmlPath);
				}
				else if( ptMuhkuhNode->GetPropVal(wxT("id"), &strId)!=true )
				{
					strError.Printf(_("The node '%s' has no attribute '%s'!"), wxT("MuhkuhPluginConfig"), wxT("id"));
					setInitError(strError, strXmlPath);
				}
				else if( strId.IsEmpty()!=false )
				{
					strError.Printf(_("Property '%s' of node '%s' is empty!"), wxT("id"), wxT("MuhkuhPluginConfig"));
					setInitError(strError, strXmlPath);
				}
				else
				{

#if defined(USE_LUA)
					/* Get the lua node. */
					ptChildNode = find_child_node(ptMuhkuhNode->GetChildren(), wxT("Lua"));
					if( ptChildNode!=NULL )
					{
						/* Get the lua module name. */
						strLuaModuleName = ptChildNode->GetNodeContent();
						cFileName.Assign(strLuaModuleName);
						/* Remove any extensions. */
						cFileName.ClearExt();
						m_strLuaModuleName = cFileName.GetFullPath(wxPATH_NATIVE);
					}
#endif

					/* Get the version node. */
					ptChildNode = find_child_node(ptMuhkuhNode->GetChildren(), wxT("Version"));
					/* Found the node? */
					if( ptChildNode==NULL )
					{
						setInitError(_("The configuration has no 'Version' node!"), strXmlPath);
					}
					else
					{
						ptVersionNode = find_child_node(ptChildNode->GetChildren(), wxT("Maj"));
						if( ptVersionNode==NULL )
						{
							setInitError(_("The 'Version' node has no 'Maj' child!"), strXmlPath);
						}
						else if( ptVersionNode->GetNodeContent().ToLong(&lVersionMaj)!=true )
						{
							setInitError(_("The 'Maj' node is no integer!"), strXmlPath);
						}
						else
						{
							ptVersionNode = find_child_node(ptChildNode->GetChildren(), wxT("Min"));
							if( ptVersionNode==NULL )
							{
								setInitError(_("The 'Version' node has no 'Min' child!"), strXmlPath);
							}
							else if( ptVersionNode->GetNodeContent().ToLong(&lVersionMin)!=true )
							{
								setInitError(_("The 'Min' node is no integer!"), strXmlPath);
							}
							else
							{
								ptVersionNode = find_child_node(ptChildNode->GetChildren(), wxT("Sub"));
								if( ptVersionNode==NULL )
								{
									setInitError(_("The 'Version' node has no 'Sub' child!"), strXmlPath);
								}
								else if( ptVersionNode->GetNodeContent().ToLong(&lVersionSub)!=true )
								{
									setInitError(_("The 'Sub' node is no integer!"), strXmlPath);
								}
								else
								{
									/* Create a new xml document from the config node and convert it to text. */
									ptChildNode = find_child_node(ptMuhkuhNode->GetChildren(), wxT("Cfg"));
									/* Found the node? */
									if( ptChildNode!=NULL )
									{
										tXmlConfig.SetRoot(ptChildNode);
										tXmlConfig.Save(tStringOutStream, wxXML_NO_INDENTATION);
										m_strConfigNodeContents = tStringOutStream.GetString();
									}

									/* Get the directory of the xml file. */
									cFileName.Assign(strXmlPath);
									/* Save path to xml config to build a relative path to the dll later. */
									m_strPluginCfgPath = cFileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

									/* All data found. */
									m_tPluginDescription.strPluginName = strCfgName;
									m_tPluginDescription.strPluginId = strId;
									m_tPluginDescription.uiVersionMajor = lVersionMaj;
									m_tPluginDescription.uiVersionMinor = lVersionMin;
									m_tPluginDescription.uiVersionSub = lVersionSub;

									fResult = true;
								}
							}
						}
					}
				}
			}
		}

		/* Delete xml input stream. */
		delete ptFileInputStream;
	}

	return fResult;
}


bool muhkuh_plugin::Load(wxString strPluginCfgPath)
{
	bool fResult;
	int iResult;
	lua_State *ptLuaState;


	wxLogMessage(m_strMe + _("loading plugin '%s'"), strPluginCfgPath.c_str());

	/* Set the config file. */
	m_strPluginCfgPath = strPluginCfgPath;

	/* Init the plugin description with defaults. */
//	m_strCfgName = strPluginCfgPath;

	/* Open the plugin xml config. */
	fResult = openXml(strPluginCfgPath);
	if( fResult!=true )
	{
		/* NOTE: do not use 'setInitError' here. this errormessage is
		*  just a summary. 'openXml' provided the details already.
		*/
		wxLogError(m_strMe + _("failed to read plugin xml config"));
	}
	else
	{
		/* Create a new lua state. */
		ptLuaState = lua_muhkuh_create_state();
		if( ptLuaState==NULL )
		{
			wxLogError(m_strMe + _("failed to create a new lua state."));
			fResult = false;
		}
		else
		{
			iResult = luaL_dostring(ptLuaState, "print(\"hallo!\")\n");
			wxLogMessage(wxT("luaL_dostring result = %d."), iResult);

			lua_close(ptLuaState);
		}

/*
		else if(lua_muhkuh_require(ptLuaState, 
		{
			lua_getglobal(L, "require");
			lua_pushstring(L, name);
return report(L, docall(L, 1, 1));
			 if (status && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error object is not a string)";
    l_message(progname, msg);
    lua_pop(L, 1);
  }

		}
*/
/*
				if( fResult!=true )
				{
					// close the plugin
					wxLogMessage(m_strMe + _("closing plugin '%s'"), strPluginCfgPath.c_str());
					close();
				}
			}
		}
*/
	}

	return fResult;
}


bool muhkuh_plugin::IsOk(void) const
{
	return m_fPluginIsOk;
}


void muhkuh_plugin::SetEnable(bool fPluginIsEnabled)
{
	m_fPluginIsEnabled = fPluginIsEnabled;
}


bool muhkuh_plugin::GetEnable(void) const
{
	return m_fPluginIsEnabled;
}


wxString muhkuh_plugin::GetInitError(void) const
{
	return m_strInitError;
}


void muhkuh_plugin::write_config(wxConfigBase *pConfig)
{
	pConfig->Write(wxT("path"), m_strPluginCfgPath);
	pConfig->Write(wxT("enable"), m_fPluginIsEnabled);
}

