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

#include <wx/file.h>
#include <wx/wfstream.h>


muhkuh_plugin::muhkuh_plugin(void)
 : m_strPluginCfgPath(wxEmptyString)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
 , m_ptCfgNode(NULL)
{
	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));
}


muhkuh_plugin::muhkuh_plugin(wxString strPluginCfgPath)
 : m_strPluginCfgPath(wxEmptyString)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
 , m_ptCfgNode(NULL)
{
	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));

	/* try to open the plugin */
	Load(strPluginCfgPath);
}


muhkuh_plugin::muhkuh_plugin(const muhkuh_plugin *ptCloneMe)
 : m_strPluginCfgPath(wxEmptyString)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
 , m_ptCfgNode(NULL)
{
	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));

	/* copy the enable flag */
	m_fPluginIsEnabled = ptCloneMe->m_fPluginIsEnabled;

	/* try to open the plugin */
	Load(ptCloneMe->m_strPluginCfgPath);
}


muhkuh_plugin::muhkuh_plugin(wxConfigBase *pConfig)
 : m_strPluginCfgPath(wxEmptyString)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
 , m_ptCfgNode(NULL)
{
	bool fReadOk;
	wxString strPath;


	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));

	/* try to get the settings */
	fReadOk = pConfig->Read(wxT("path"), &strPath);
	if( fReadOk==false )
	{
		wxLogError(wxT("missing path information in plugin configuration"));
	}
	else
	{
		fReadOk = pConfig->Read(wxT("enable"), &m_fPluginIsEnabled, false);
		if( fReadOk==false )
		{
			wxLogWarning(wxT("missing 'enable' entry in plugin configuration, defaulting to 'disabled'"));
		}
		Load(strPath);
	}
}


muhkuh_plugin::~muhkuh_plugin(void)
{
	fn_leave();
	close();
}


bool muhkuh_plugin::openXml(wxString strXmlPath)
{
	bool fResult;
	wxFile cFile;
	wxFileInputStream *ptFileInputStream;
	wxXmlNode *ptNode;
	wxXmlNode *ptSoNode;
	wxXmlNode *ptCfgNode;
	wxString strCfgName;
	wxString strSoName;
	wxString strId;
	wxFileName cFileName;


	/* expect failure */
	fResult = false;

	/* clear config node */
	m_ptCfgNode = NULL;

	/* open the xml document */
	fResult = cFile.Open(strXmlPath, wxFile::read);
	if( fResult!=true )
	{
		wxLogError(wxT("Failed to open xml file '") + strXmlPath + wxT("'"));
	}
	else
	{
		ptFileInputStream = new wxFileInputStream(cFile);
		fResult = m_xmldoc.Load(*ptFileInputStream);
		if( fResult!=true )
		{
			// failed to load the xml file
			wxLogError(wxT("failed to load the plugin config from the input stream"));
		}
		else
		{
			/* look for root node */
			ptNode = m_xmldoc.GetRoot();
			while( ptNode!=NULL && ptNode->GetType()!=wxXML_ELEMENT_NODE && ptNode->GetName()!=wxT("MuhkuhPluginConfig") )
			{
				ptNode = ptNode->GetNext();
			}
			// found node?
			if( ptNode==NULL )
			{
				wxLogError(wxT("Root node 'MuhkuhPluginConfig' not found. Is this really a Muhkuh plugin config?"));
				fResult = false;
			}
			else
			{
				// get name attribute
				if( ptNode->GetPropVal(wxT("name"), &strCfgName)!=true )
				{
					wxLogError(wxT("Root node 'MuhkuhPluginConfig' has no 'name' attribute!"));
				}
				else if( strCfgName.IsEmpty()!=false )
				{
					wxLogError(wxT("'name' attribute must not be empty!"));
				}
				else if( ptNode->GetPropVal(wxT("so"), &strSoName)!=true )
				{
					wxLogError(wxT("Root node 'MuhkuhPluginConfig' has no 'so' attribute!"));
				}
				else if( strSoName.IsEmpty()!=false )
				{
					wxLogError(wxT("'so' attribute must not be empty!"));
				}
				else if( ptNode->GetPropVal(wxT("id"), &strId)!=true )
				{
					wxLogError(wxT("Root node 'MuhkuhPluginConfig' has no 'id' attribute!"));
				}
				else if( strId.IsEmpty()!=false )
				{
					wxLogError(wxT("'id' attribute must not be empty!"));
				}
				else
				{
					// get the config node
					ptCfgNode = ptNode->GetChildren();
					while( ptCfgNode!=NULL && ptCfgNode->GetType()!=wxXML_ELEMENT_NODE && ptCfgNode->GetName()!=wxT("Cfg") )
					{
						ptCfgNode = ptCfgNode->GetNext();
					}
					// found node?
					if( ptCfgNode==NULL )
					{
						wxLogError(wxT("Node 'Cfg' not found."));
						fResult = false;
					}
					else
					{
						// all data found
						m_strCfgName = strCfgName;
						m_ptCfgNode = ptCfgNode;
						// get the directory of the xml file
						cFileName.Assign(strXmlPath);
						m_strXmlCfgPath = cFileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
						// assign the so name
						cFileName.Assign(strSoName);
						// if the so has no extension, set the standard one for the platform
						if( cFileName.HasExt()==false )
						{
							cFileName.Assign( wxDynamicLibrary::CanonicalizeName(strSoName, wxDL_MODULE) );
						}
						if( cFileName.IsRelative()==true )
						{
							cFileName.Normalize(wxPATH_NORM_ALL, m_strXmlCfgPath, wxPATH_NATIVE);
						}
						m_strSoName = cFileName.GetFullPath(wxPATH_NATIVE);
						m_strPluginId = strId;
						fResult = true;
					}
				}
			}
		}

		/* delete xml input stream */
		delete ptFileInputStream;
	}

	return fResult;
}


wxString muhkuh_plugin::GetConfigName(void) const
{
	return m_strCfgName;
}


bool muhkuh_plugin::Load(wxString strPluginCfgPath)
{
	bool fResult;
	int iResult;
	wxString strMsg;
	wxLog *ptLogTarget;
	const muhkuh_plugin_desc *ptDesc;


	wxLogMessage(wxT("loading plugin ") + strPluginCfgPath);

	/* do not open a plugin twice */
	if( m_tPluginIf.tHandle!=NULL )
	{
		wxLogError(wxT("...plugin is already open"));
		fResult = false;
	}
	else
	{
		/* open plugin xml config */
		fResult = openXml(strPluginCfgPath);
		if( fResult!=true )
		{
			wxLogError(wxT("failed to read plugin xml config"));
		}
		else
		{
			/* try to open the plugin */

			fResult = open(m_strSoName);
			if( fResult!=true )
			{
				strMsg = wxT("...failed to open the plugin ");
				wxLogError(strMsg);
			}
			else
			{
				ptLogTarget = wxLog::GetActiveTarget();
				iResult = fn_init(ptLogTarget, m_ptCfgNode, m_strPluginId);
				if( iResult<0 )
				{
					strMsg.Printf(wxT("...plugin init failed with errorcode %d"), iResult);
					wxLogError(strMsg);

					fResult = false;
				}
				else
				{
					/* get the description */
					ptDesc = m_tPluginIf.uSym.tFn.fn_get_desc();
					if( ptDesc==NULL )
					{
						wxLogError(wxT("Failed to get plugin description"));
						fResult = false;
					}
					else
					{
						/* copy the structure elements */
						if( ptDesc->strPluginName.IsEmpty()!=false )
						{
							tPluginDesc.strPluginName = wxT("missing name");
						}
						else
						{
							tPluginDesc.strPluginName = ptDesc->strPluginName;
						}
						tPluginDesc.strPluginId = ptDesc->strPluginId;
						tPluginDesc.tVersion = ptDesc->tVersion;

						/* ok, accept plugin name */
						m_strPluginCfgPath = strPluginCfgPath;
					}
				}

				if( fResult!=true )
				{
					/* close the plugin */
					wxLogMessage(wxT("closing plugin ") + strPluginCfgPath);
					close();
				}
			}
		}
	}

	return fResult;
}


bool muhkuh_plugin::IsOk(void)
{
	return (m_tPluginIf.tHandle!=NULL);
}


void muhkuh_plugin::SetEnable(bool fPluginIsEnabled)
{
	m_fPluginIsEnabled = fPluginIsEnabled;
}


bool muhkuh_plugin::GetEnable(void) const
{
	return m_fPluginIsEnabled;
}


void muhkuh_plugin::write_config(wxConfigBase *pConfig)
{
	pConfig->Write(wxT("path"), m_strPluginCfgPath);
	pConfig->Write(wxT("enable"), m_fPluginIsEnabled);
}


int muhkuh_plugin::fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId)
{
	int iResult;


	/* check for loaded plugin */
	if( m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_init!=NULL )
	{
		iResult = m_tPluginIf.uSym.tFn.fn_init(ptLogTarget, ptCfgNode, strPluginId);
	}
	else
	{
		/* plugin is not open, return some general error */
		iResult = -1;
	}

	return iResult;
}


int muhkuh_plugin::fn_init_lua(wxLuaState *ptLuaState)
{
	int iResult;


	/* check for loaded plugin */
	if( m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_init_lua!=NULL )
	{
		iResult = m_tPluginIf.uSym.tFn.fn_init_lua(ptLuaState);
	}
	else
	{
		/* plugin is not open, return some general error */
		iResult = -1;
	}

	return iResult;
}


int muhkuh_plugin::fn_leave(void)
{
	int iResult;


	/* check for loaded plugin */
	if( m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_leave!=NULL )
	{
		iResult = m_tPluginIf.uSym.tFn.fn_leave();
	}
	else
	{
		/* plugin is not open, return some general error */
		iResult = -1;
	}

	return iResult;
}


const muhkuh_plugin_desc *muhkuh_plugin::fn_get_desc(void)
{
	return &tPluginDesc;
}


int muhkuh_plugin::fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList)
{
	int iResult;


	/* check for loaded plugin */
	if( m_fPluginIsEnabled==true && m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_detect_interfaces!=NULL )
	{
		iResult = m_tPluginIf.uSym.tFn.fn_detect_interfaces(pvInterfaceList);
	}
	else
	{
		/* plugin is not open, return some general error */
		iResult = -1;
	}

	return iResult;
}


const muhkuh_plugin::muhkuh_plugin_symbol_offset_t muhkuh_plugin::atPluginSymbolOffsets[] =
{
	{
		wxT("fn_init"),
		offsetof(muhkuh_plugin_function_interface_t, fn_init) / sizeof(void*)
	},
	{
		wxT("fn_init_lua"),
		offsetof(muhkuh_plugin_function_interface_t, fn_init_lua) / sizeof(void*)
	},
	{
		wxT("fn_leave"),
		offsetof(muhkuh_plugin_function_interface_t, fn_leave) / sizeof(void*)
	},
	{
		wxT("fn_get_desc"),
		offsetof(muhkuh_plugin_function_interface_t, fn_get_desc) / sizeof(void*)
	},
	{
		wxT("fn_detect_interfaces"),
		offsetof(muhkuh_plugin_function_interface_t, fn_detect_interfaces) / sizeof(void*)
	}
};


bool muhkuh_plugin::open(wxString strPluginPath)
{
	wxFileName fileName;
	wxString strFullPath;
	wxDynamicLibrary dll;
	bool fDllLoaded;
	const muhkuh_plugin_symbol_offset_t *ptCnt, *ptEnd;
	void *pvPtr;
	wxString strMsg;
	wxDllType tDllHandle;


	/* try to open a plugin */

	/* init result structure */
	tDllHandle = NULL;

	fileName.Assign(strPluginPath, wxPATH_NATIVE);

	// check the path
	if( fileName.IsOk()==false )
	{
		// path is not correct
		wxLogError(wxT("failed to set path, IsOK returned false"));
	}
	else
	{
		// does the path point to a directory?
		if( fileName.IsFileExecutable()==false )
		{
			// no, the path does not point to a directory
			wxLogError(wxT("the path does not point to an executable file"));
		}
		else
		{
			strFullPath = fileName.GetFullPath();
			fDllLoaded = dll.Load(strFullPath, wxDL_DEFAULT);
			if( fDllLoaded==false )
			{
				wxLogMessage(wxT("failed to load the plugin"));
			}
			else
			{
				/* loop over all symbols in the list and enter them in the struct */
				ptCnt = atPluginSymbolOffsets;
				ptEnd = ptCnt + (sizeof(atPluginSymbolOffsets)/sizeof(atPluginSymbolOffsets[0]));
				pvPtr = NULL;
				while( ptCnt<ptEnd )
				{
					pvPtr = dll.GetSymbol(ptCnt->pcSymbolName);
					if( pvPtr==NULL )
					{
						strMsg = wxT("failed to get symbol ");
						strMsg.Append(ptCnt->pcSymbolName);
						strMsg.Append(wxT(" from plugin "));
						strMsg.Append(strFullPath);
						wxLogError(strMsg);
						break;
					}
					m_tPluginIf.uSym.apv[ptCnt->sizOffset] = pvPtr;

					/* next symbol */
					++ptCnt;
				};

				/* check for error (pvPtr is NULL) */
				if( pvPtr!=NULL )
				{
					/* pvPtr is not NULL, this means all symbols were loaded */

					/* get handle for the dll */
					tDllHandle = dll.Detach();
				}
				else
				{
					/* unload the dll */
					dll.Unload();
				}
			}
		}
	}

	m_tPluginIf.tHandle = tDllHandle;
	return (tDllHandle!=NULL);
}


void muhkuh_plugin::close(void)
{
	if( m_tPluginIf.tHandle!=NULL )
	{
		wxDynamicLibrary::Unload(m_tPluginIf.tHandle);
	}
	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));
}

