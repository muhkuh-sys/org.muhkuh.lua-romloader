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


muhkuh_plugin::muhkuh_plugin(void)
 : m_strPluginPath(wxEmptyString)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
{
	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));
}


muhkuh_plugin::muhkuh_plugin(wxString strPluginPath)
 : m_strPluginPath(wxEmptyString)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
{
	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));

	/* try top open the plugin */
	Load(strPluginPath);
}


muhkuh_plugin::muhkuh_plugin(const muhkuh_plugin *ptCloneMe)
 : m_strPluginPath(wxEmptyString)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
{
	memset(&m_tPluginIf, 0, sizeof(muhkuh_plugin_interface));

	/* copy the enable flag */
	m_fPluginIsEnabled = ptCloneMe->m_fPluginIsEnabled;

	/* try top open the plugin */
	Load(ptCloneMe->m_strPluginPath);
}


muhkuh_plugin::muhkuh_plugin(wxConfigBase *pConfig)
 : m_strPluginPath(wxEmptyString)
 , m_fPluginIsEnabled(false)
 , m_ptLuaState(NULL)
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


bool muhkuh_plugin::Load(wxString strPluginPath)
{
	bool fResult;
	int iResult;
	wxString strMsg;
	wxLog *ptLogTarget;


	wxLogMessage(wxT("loading plugin ") + strPluginPath);

	/* do not open a plugin twice */
	if( m_tPluginIf.tHandle!=NULL )
	{
		wxLogError(wxT("...plugin is already open"));

		fResult = false;
	}
	else
	{
		/* try to open the plugin */

		fResult = open(strPluginPath);
		if( fResult!=true )
		{
			strMsg = wxT("...failed to open the plugin ");
			wxLogError(strMsg);
		}
		else
		{
			ptLogTarget = wxLog::GetActiveTarget();
			iResult = fn_init(ptLogTarget);
			if( iResult<0 )
			{
				strMsg.Printf(wxT("...plugin init failed with errorcode %d"), iResult);
				wxLogError(strMsg);

				/* close the plugin */
				wxLogMessage(wxT("closing plugin ") + strPluginPath);
				close();

				fResult = false;
			}
			else
			{
				/* ok, accept plugin name */
				m_strPluginPath = strPluginPath;
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


bool muhkuh_plugin::GetEnable(void)
{
	return m_fPluginIsEnabled;
}


void muhkuh_plugin::write_config(wxConfigBase *pConfig)
{
	pConfig->Write(wxT("path"), m_strPluginPath);
	pConfig->Write(wxT("enable"), m_fPluginIsEnabled);
}


int muhkuh_plugin::fn_init(wxLog *ptLogTarget)
{
	int iResult;


	/* check for loaded plugin */
	if( m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_init!=NULL )
	{
		iResult = m_tPluginIf.uSym.tFn.fn_init(ptLogTarget);
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
	const muhkuh_plugin_desc *ptResult;


	/* check for loaded plugin */
	if( m_tPluginIf.tHandle!=NULL && m_tPluginIf.uSym.tFn.fn_get_desc!=NULL )
	{
		ptResult = m_tPluginIf.uSym.tFn.fn_get_desc();
	}
	else
	{
		/* plugin is not open, return some general error */
		ptResult = NULL;
	}

	return ptResult;
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

#if defined(__GNU__) | defined(__GNUWIN32__) | defined(__MINGW32__)
const muhkuh_plugin::muhkuh_plugin_symbol_offset_t muhkuh_plugin::atPluginSymbolOffsets[] =
{
	{
		wxT("_Z7fn_initP5wxLog"),
		offsetof(muhkuh_plugin_function_interface_t, fn_init) / sizeof(void*)
	},
	{
		wxT("_Z11fn_init_luaP10wxLuaState"),
		offsetof(muhkuh_plugin_function_interface_t, fn_init_lua) / sizeof(void*)
	},
	{
		wxT("_Z8fn_leavev"),
		offsetof(muhkuh_plugin_function_interface_t, fn_leave) / sizeof(void*)
	},
	{
		wxT("_Z11fn_get_descv"),
		offsetof(muhkuh_plugin_function_interface_t, fn_get_desc) / sizeof(void*)
	},
	{
		wxT("_Z20fn_detect_interfacesPSt6vectorIP22muhkuh_plugin_instanceSaIS1_EE"),
		offsetof(muhkuh_plugin_function_interface_t, fn_detect_interfaces) / sizeof(void*)
	}
};

#else
#if defined(__VISUALC__)
// this is for MSVC
const muhkuh_plugin::muhkuh_plugin_symbol_offset_t muhkuh_plugin::atPluginSymbolOffsets[] =
{
	{
		wxT("?fn_init@@YAHPAVwxLog@@@Z"),
		offsetof(muhkuh_plugin_function_interface_t, fn_init) / sizeof(void*)
	},
	{
		wxT("?fn_init_lua@@YAHPAVwxLuaState@@@Z"),
		offsetof(muhkuh_plugin_function_interface_t, fn_init_lua) / sizeof(void*)
	},
	{
		wxT("?fn_leave@@YAHXZ"),
		offsetof(muhkuh_plugin_function_interface_t, fn_leave) / sizeof(void*)
	},
	{
		wxT("?fn_get_desc@@YAPBUmuhkuh_plugin_desc@@XZ"),
		offsetof(muhkuh_plugin_function_interface_t, fn_get_desc) / sizeof(void*)
	},
	{
		wxT("?fn_detect_interfaces@@YAHPAV?$vector@PAVmuhkuh_plugin_instance@@V?$allocator@PAVmuhkuh_plugin_instance@@@std@@@std@@@Z"),
		offsetof(muhkuh_plugin_function_interface_t, fn_detect_interfaces) / sizeof(void*)
	}
};
#else
#error "unknown compiler, please add the symbol names!"
#endif
#endif

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

