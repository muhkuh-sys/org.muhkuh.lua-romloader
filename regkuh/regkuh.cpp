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


#include <wx/defs.h>

#include <stdio.h>

#include <wx/app.h>
#include "wx/cmdline.h"
#include <wx/string.h>
#include <wx/stdpaths.h>

#include <string.h>

#ifdef __VISUALC__
	#pragma hdrstop
#endif

#include "regkuh.h"
#include "regkuh_version.h"

#include "muhkuh_version.h"


static const wxCmdLineEntryDesc cmdLineDesc[] =
{
	{ wxCMD_LINE_OPTION, wxT("c"), wxT("config-file"), _T("use the specified configuration file instead of the default Muhkuh.cfg"), wxCMD_LINE_VAL_STRING },
	{ wxCMD_LINE_OPTION, wxT("a"), wxT("app-dir"), _T("set the application directory, default is the current working dir"), wxCMD_LINE_VAL_STRING },

	{ wxCMD_LINE_SWITCH, wxT("v"), wxT("verbose"), _T("be verbose") },
	{ wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), _T("display this help and exit.") },

	{ wxCMD_LINE_SWITCH, NULL, wxT("version"), _T("display version information and exit.") },

	{ wxCMD_LINE_PARAM,  NULL, NULL, _T("command"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE },
	{ wxCMD_LINE_NONE }
};

static const char cmdLineLogo[] = 
	"regkuh is a helper tool for automatic installers. "
	"It registers repositories and plugins in a configuration file and sets options like autostart, lua include paths, etc."
	"\n"
;

int main(int argc, char *argv[])
{
	int iRet;
#if wxUSE_UNICODE
	wxChar **wxArgv;
	int iCnt;
	wxMB2WXbuf warg;
#endif
	wxCmdLineParser tParser;
	muhkuh_regApp *tApp;


	wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

	wxInitializer initializer;
	if ( !initializer )
	{
		fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");
		return -1;
	}

#if wxUSE_UNICODE
	// allocate array for unicode commandline
	wxArgv = new wxChar*[argc + 1];

	// convert ascii arguments to unicode
	for(iCnt=0; iCnt<argc; ++iCnt)
	{
		warg = wxConvertMB2WX(argv[iCnt]);
		wxArgv[iCnt] = wxStrdup(warg);
	}
	// end of list
	wxArgv[iCnt] = NULL;

	tParser.SetCmdLine(argc, wxArgv);
#else
	tParser.SetCmdLine(argc, argv);
#endif
	tParser.SetSwitchChars(wxT("-"));
	tParser.EnableLongOptions(true);
	tParser.SetDesc(cmdLineDesc);
	tParser.SetLogo(cmdLineLogo);

	tApp = new muhkuh_regApp();
	iRet = tApp->execute_operation(&tParser);
	delete tApp;

#if wxUSE_UNICODE
	// free the converted commandline args
	for(iCnt=0; iCnt<argc; ++iCnt)
	{
		free(wxArgv[iCnt]);
	}
        delete[] wxArgv;
#endif

	return iRet;
}





muhkuh_regApp::muhkuh_regApp(void)
 : m_ptPluginManager(NULL)
 , m_ptRepositoryManager(NULL)
{
	// create the plugin manager
	m_ptPluginManager = new muhkuh_plugin_manager();

	// create the repository manager
	m_ptRepositoryManager = new muhkuh_repository_manager();


	clearSettings();
}


muhkuh_regApp::~muhkuh_regApp(void)
{
	cleanUp();

	// delete the plugin manager
	if( m_ptPluginManager!=NULL )
	{
		delete m_ptPluginManager;
	}

	// delete the repository manager
	if( m_ptRepositoryManager!=NULL )
	{
		delete m_ptRepositoryManager;
	}
}


void muhkuh_regApp::clearSettings(void)
{
	wxFileName cfgName;


	m_fCfgVerbose = false;
	m_fCfgShowHelp = false;
	m_fCfgShowVersion = false;

	// get the default application path
	cfgName.Assign(wxStandardPaths::Get().GetExecutablePath());
	m_strCfgAppPath = cfgName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	// get the default config file
	m_strCfgConfigFile = wxT("Muhkuh.cfg");
}


void muhkuh_regApp::cleanUp(void)
{
}


void muhkuh_regApp::showVersion(void)
{
	printf(REGKUH_APPLICATION_NAME " " REGKUH_VERSION_STRING "\n");
	printf("Copyright (c) 2008, the Muhkuh team.\n");
	printf("There is NO warranty.  You may redistribute this software\n");
	printf("under the terms of the GNU General Public License.\n");
	printf("For more information about these matters, see the files named COPYING.\n");
}


void muhkuh_regApp::showHelp(void)
{
	// show general usage
	ptParser->Usage();

	printf(	"\n"
		"Examples:\n"
		"  regkuh -rp plugins/romloader_uart.xml\n"
		"        register the romloader_uart plugin in the plugins folder.\n"
		"\n"
		"Muhkuh home page: www.sf.net/projects/muhkuh\n"
	);
}


int muhkuh_regApp::parse_args(void)
{
	int iCnt;
	wxString strVal;


	clearSettings();

	m_fCfgVerbose = ptParser->Found(wxT("v"));
	m_fCfgShowHelp = ptParser->Found(wxT("h"));
	m_fCfgShowVersion = ptParser->Found(wxT("version"));

	if( ptParser->Found(wxT("c"), &strVal)==true )
	{
		m_strCfgConfigFile = strVal;
	}

	if( ptParser->Found(wxT("a"), &strVal)==true )
	{
		m_strCfgAppPath = strVal;
	}

	return EXIT_SUCCESS;
}


int muhkuh_regApp::open_config_file(void)
{
	wxFileConfig *ptConfig;
	wxFileName cfgName;


	// get the config
	cfgName.SetFullName(m_strCfgConfigFile);
	ptConfig = new wxFileConfig(wxT(MUHKUH_APPLICATION_NAME), wxT(MUHKUH_APPLICATION_NAME " team"), cfgName.GetFullPath(), cfgName.GetFullPath(), wxCONFIG_USE_LOCAL_FILE);
	wxConfigBase::Set(ptConfig);
	ptConfig->SetRecordDefaults();

	// get all plugins
	m_ptPluginManager->read_config(ptConfig);

	// get all repositories
	m_ptRepositoryManager->read_config(ptConfig);

	return EXIT_SUCCESS;
}


int muhkuh_regApp::close_config_file(void)
{
	wxConfigBase *ptConfig;
	int iRes;


	// get the config
	ptConfig = wxConfigBase::Set(NULL);
	if( ptConfig==NULL )
	{
		iRes = EXIT_FAILURE;
	}
	else
	{
		// save all plugins
		m_ptPluginManager->write_config(ptConfig);

		// save repositories
		m_ptRepositoryManager->write_config(ptConfig);

		delete ptConfig;

		iRes = EXIT_SUCCESS;
	}

	return iRes;
}


int muhkuh_regApp::execute_operation(wxCmdLineParser *ptPar)
{
	int iRet;
	int iCnt;
	int iMax;
	wxString strParam;


	ptParser = ptPar;
	if( ptParser->Parse(false)!=0 )
	{
		iRet = EXIT_FAILURE;
	}
	else
	{
		iRet = parse_args();
		if( iRet==EXIT_SUCCESS )
		{
			if( m_fCfgShowHelp==true )
			{
				showHelp();
			}
			else if( m_fCfgShowVersion==true )
			{
				showVersion();
			}
			else
			{
				// open the config file
				wxLogMessage(wxT("reading config file '%s'"), m_strCfgConfigFile.fn_str());
				iRet = open_config_file();
				if( iRet==EXIT_SUCCESS )
				{
					// loop over all parameters
					iCnt = 0;
					iMax = ptParser->GetParamCount();
					while( iCnt<iMax )
					{
						// get the parameter
						strParam = ptParser->GetParam(iCnt);
						// process the parameter
						iRet = process_parameter(strParam);
						if( iRet!=EXIT_SUCCESS )
						{
							break;
						}
						// next parameter
						++iCnt;
					}
					if( iRet==EXIT_SUCCESS )
					{
						wxLogMessage(wxT("writing config file '%s'"), m_strCfgConfigFile.fn_str());
						iRet = close_config_file();
					}
				}
			}
		}
	}

	// clean up and exit
	cleanUp();

	return iRet;
}


int muhkuh_regApp::process_parameter(wxString &strParam)
{
	int iRes;
	wxArrayString aParam;
	wxString strElem;
	size_t sizCnt;
	size_t sizMax;
	wxChar c;
	bool fQuote;


	// be optimistic
	iRes = EXIT_SUCCESS;

	// cut the parameter into elements

	// start at char 0
	sizCnt = 0;
	// max position in string
	sizMax = strParam.Len();
	// no quote
	fQuote = false;
	// clear the parameter
	strElem.Empty();
	while( sizCnt<sizMax )
	{
		c = strParam.GetChar(sizCnt);

		// escaped char?
		if( c==wxT('\\') )
		{
			++sizCnt;
			if( sizCnt>=sizMax )
			{
				wxLogError(wxT("escape at the end of the string!"));
				iRes = EXIT_FAILURE;
				break;
			}
			strElem.Append(strParam.GetChar(sizCnt));
		}
		else if( c==wxT('"') )
		{
			strElem.Append(c);
			fQuote = !fQuote;
		}
		else if( fQuote==false && c==wxT(':') )
		{
			aParam.Add(strElem);
			strElem.Empty();
		}
		else
		{
			strElem.Append(c);
		}

		// next char
		++sizCnt;
	}

	if( iRes==EXIT_SUCCESS )
	{
		// still quotes on?
		if( fQuote==true )
		{
			wxLogError(wxT("quotes not closed!"));
			iRes = EXIT_FAILURE;
		}
		else
		{
			// add the rest of the string
			if( strElem.IsEmpty()==false )
			{
				aParam.Add(strElem);
			}

			// *** DEBUG ***
			// print all elements
			{
				sizCnt = 0;
				sizMax = aParam.Count();
				wxLogMessage(wxT("found %d elements:"), sizMax);
				while(sizCnt<sizMax)
				{
					wxLogMessage(wxT(" %02d: %s"), sizCnt, aParam[sizCnt].fn_str());
					++sizCnt;
				}
			}

			// get the command
			strElem = aParam[0];
			if( strElem.Cmp(wxT("rp"))==0 )
			{
				iRes = register_plugin(aParam);
			}
			else if( strElem.Cmp(wxT("rr"))==0 )
			{
				iRes = register_repository(aParam);
			}
			else if( strElem.Cmp(wxT("ct"))==0 )
			{
				iRes = custom_title(aParam);
			}
			else if( strElem.Cmp(wxT("ci"))==0 )
			{
				iRes = custom_icon(aParam);
			}
			else if( strElem.Cmp(wxT("as"))==0 )
			{
				iRes = autostart(aParam);
			}
			else
			{
				wxLogError(_T("unknown command: ") + strParam);
				iRes = EXIT_FAILURE;
			}
		}
	}

	return iRes;
}


int muhkuh_regApp::custom_title(wxArrayString &aParam)
{
	int iRes;
	wxString strTitle;
	size_t sizParamCount;
	wxConfigBase *ptConfig;


	// be optimistic
	iRes = EXIT_SUCCESS;

	// get the config
	ptConfig = wxConfigBase::Get();
	if( ptConfig==NULL )
	{
		iRes = EXIT_FAILURE;
	}
	else
	{
		wxLogMessage(_T("custom title"));

		// check minimum number of elements
		sizParamCount = aParam.Count();
		if( sizParamCount==1 )
		{
			strTitle.Empty();
		}
		else if( sizParamCount!=2 )
		{
			strTitle = aParam[1];
		}
		else
		{
			wxLogError(wxT("not enough parameter for a plugin!"));
			iRes = EXIT_FAILURE;
		}

		if( iRes==EXIT_SUCCESS )
		{
			wxLogMessage(_("Set custom title to '%s'"), strTitle.fn_str());
			ptConfig->SetPath(wxT("/MainFrame"));
			ptConfig->Write(wxT("customtitle"), strTitle);
		}
	}

	return iRes;
}


int muhkuh_regApp::custom_icon(wxArrayString &aParam)
{
	int iRes;
	wxString strIcon;
	size_t sizParamCount;
	wxConfigBase *ptConfig;


	// be optimistic
	iRes = EXIT_SUCCESS;

	// get the config
	ptConfig = wxConfigBase::Get();
	if( ptConfig==NULL )
	{
		iRes = EXIT_FAILURE;
	}
	else
	{
		wxLogMessage(_T("custom icon"));

		// check minimum number of elements
		sizParamCount = aParam.Count();
		if( sizParamCount==1 )
		{
			strIcon.Empty();
		}
		else if( sizParamCount!=2 )
		{
			strIcon = aParam[1];
		}
		else
		{
			wxLogError(wxT("not enough parameter for a custom icon!"));
			iRes = EXIT_FAILURE;
		}

		if( iRes==EXIT_SUCCESS )
		{
			wxLogMessage(_("Set custom icon to '%s'"), strIcon.fn_str());
			ptConfig->SetPath(wxT("/MainFrame"));
			ptConfig->Write(wxT("customicon"), strIcon);
		}
	}

	return iRes;
}


int muhkuh_regApp::register_plugin(wxArrayString &aParam)
{
	int iRes;
	wxString strPlugin;
	bool fEnable;
	size_t sizParamCount;
	wxString strEnable;
	long lIdx;
	bool fIsOk;


	// be optimistic
	iRes = EXIT_SUCCESS;

	wxLogMessage(_T("register plugin"));

	// check minimum number of elements
	sizParamCount = aParam.Count();
	if( sizParamCount!=2 && sizParamCount!=3 )
	{
		wxLogError(wxT("not enough parameter for a plugin!"));
		iRes = EXIT_FAILURE;
	}
	else
	{
		// get the plugin
		strPlugin = aParam[1];
		// get the optional "enable" parameter
		if( sizParamCount==3 )
		{
			strEnable = aParam[1];
			if( strEnable.Cmp("1")==0 )
			{
				// make this repository active
				fEnable = true;
			}
			else if( strEnable.Cmp("0")==0 )
			{
				fEnable = false;
			}
			else
			{
				wxLogError(wxT("strange value for enable: ") + strEnable);
				iRes = EXIT_FAILURE;
			}
		}
		else
		{
			// don't enable by default
			fEnable = false;
		}

		if( iRes==EXIT_SUCCESS )
		{
			wxLogMessage(_("Add plugin '%s'..."), strPlugin.fn_str());
			lIdx = m_ptPluginManager->addPlugin(strPlugin);
			fIsOk = m_ptPluginManager->IsOk(lIdx);
			if( fIsOk==true )
			{
				wxLogMessage(_("Plugin is ok!"));
			}
			else
			{
				wxLogMessage(_("Plugin is not ok, init error: '%s'"), m_ptPluginManager->GetInitError(lIdx).fn_str());
			}

			if( fEnable==true )
			{
				wxLogMessage(_("Enable plugin."));
				m_ptPluginManager->SetEnable(lIdx, true);
			}
		}
	}

	return iRes;
}


int muhkuh_regApp::register_repository(wxArrayString &aParam)
{
	muhkuh_repository::REPOSITORY_TYP_E tRepositoryTyp;
	int iRes;
	wxString strRepName;
	wxString strRepTyp;
	wxString strRepLoc;
	wxString strRepExt;
	bool fMakeActive;
	size_t sizParamCount;
	wxString strActive;
	muhkuh_repository *ptRepository;
	long lRepIdx;


	// be optimistic
	iRes = EXIT_SUCCESS;

	wxLogMessage(_T("register repository"));


	// check minimum number of elements
	sizParamCount = aParam.Count();
	if( sizParamCount<3 )
	{
		wxLogError(wxT("not enough parameter for a repository!"));
		iRes = EXIT_FAILURE;
	}
	else
	{
		// get the repository name
		strRepName = aParam[1];
		// get the repository typ
		strRepTyp = aParam[2];
		// get the repository location
		strRepLoc = aParam[3];
		// don't make active by default
		strActive = wxT("0");

		// parse the repository typ
		tRepositoryTyp = muhkuh_repository::REPOSITORY_TYP_UNDEFINED;
		if( strRepTyp.Cmp(wxT("d"))==0 )
		{
			tRepositoryTyp = muhkuh_repository::REPOSITORY_TYP_DIRSCAN;
			if( sizParamCount!=5 && sizParamCount!=6 )
			{
				wxLogError(wxT("not enough parameter for a dirscan repository!"));
				iRes = EXIT_FAILURE;
			}
			else
			{
				// get the repository extension
				strRepExt = aParam[4];
				if( sizParamCount==6 )
				{
					strActive = aParam[5];
				}
			}
		}
		else if( strRepTyp.Cmp(wxT("f"))==0 )
		{
			tRepositoryTyp = muhkuh_repository::REPOSITORY_TYP_FILELIST;
			if( sizParamCount!=4 && sizParamCount!=5 )
			{
				wxLogError(wxT("not enough parameter for a filelist repository!"));
				iRes = EXIT_FAILURE;
			}
			else
			{
				if( sizParamCount==5 )
				{
					strActive = aParam[4];
				}
			}
		}
		else if( strRepTyp.Cmp(wxT("s"))==0 )
		{
			tRepositoryTyp = muhkuh_repository::REPOSITORY_TYP_SINGLEXML;
			if( sizParamCount!=4 && sizParamCount!=5 )
			{
				wxLogError(wxT("not enough parameter for a single xml repository!"));
				iRes = EXIT_FAILURE;
			}
			else
			{
				if( sizParamCount==5 )
				{
					strActive = aParam[4];
				}
			}
		}
		else
		{
			wxLogError(_T("unknown repository typ: ") + strRepTyp);
			iRes = EXIT_FAILURE;
		}

		if( tRepositoryTyp!=muhkuh_repository::REPOSITORY_TYP_UNDEFINED )
		{
			if( strActive.Cmp("1")==0 )
			{
				// make this repository active
				fMakeActive = true;
			}
			else if( strActive.Cmp("0")==0 )
			{
				fMakeActive = false;
			}
			else
			{
				wxLogError(wxT("strange value for activate: ") + strActive);
				iRes = EXIT_FAILURE;
			}

			if( iRes==EXIT_SUCCESS )
			{
				// add the new repository
				ptRepository = new muhkuh_repository(strRepName);
				switch( tRepositoryTyp )
				{
				case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
					ptRepository->SetDirscan(strRepLoc, strRepExt);
					break;
				case muhkuh_repository::REPOSITORY_TYP_FILELIST:
					ptRepository->SetFilelist(strRepLoc);
					break;
				case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
					ptRepository->SetSingleXml(strRepLoc);
					break;
				}

				wxLogMessage(wxT("Add new repository: %s"), ptRepository->GetStringRepresentation().fn_str());
				lRepIdx = m_ptRepositoryManager->addRepository(ptRepository);

				if( fMakeActive==true )
				{
					wxLogMessage(wxT("Activate repository"));
					m_ptRepositoryManager->SetActiveRepository(lRepIdx);
				}
			}
		}
	}

	return iRes;
}


int muhkuh_regApp::autostart(wxArrayString &aParam)
{
	int iRes;
	wxString strAutostart;
	bool fAutoStart;
	bool fAutoExit;
	size_t sizParamCount;
	wxString strAutoexit;
	long lIdx;
	bool fIsOk;
	wxConfigBase *ptConfig;


	// be optimistic
	iRes = EXIT_SUCCESS;

	wxLogMessage(_T("autostart"));

	// get the config
	ptConfig = wxConfigBase::Get();
	if( ptConfig==NULL )
	{
		iRes = EXIT_FAILURE;
	}
	else
	{
		// check minimum number of elements
		sizParamCount = aParam.Count();
		if( sizParamCount==1 )
		{
			// no parameter: turn off autostart/autoexit
			fAutoStart = false;
			fAutoExit = false;
			strAutostart.Empty();
		}
		else if ( sizParamCount==2 )
		{
			// 1 parameter: activate autostart, but no autoexit
			fAutoStart = true;
			fAutoExit = false;
			strAutostart = aParam[1];
		}
		else if( sizParamCount!=3 )
		{
			// 2 parameters: activate autostart, autoexit is 2nd parameter
			fAutoStart = true;
			strAutostart = aParam[1];
			strAutoexit = aParam[2];
			if( strAutoexit.Cmp("1")==0 )
			{
				// make this repository active
				fAutoExit = true;
			}
			else if( strAutoexit.Cmp("0")==0 )
			{
				fAutoExit = false;
			}
			else
			{
				wxLogError(wxT("strange value for autoexit: ") + strAutoexit);
				iRes = EXIT_FAILURE;
			}
		}
		else
		{
			wxLogError(wxT("not enough parameter for a plugin!"));
			iRes = EXIT_FAILURE;
		}

		if( iRes==EXIT_SUCCESS )
		{
			ptConfig->SetPath(wxT("/MainFrame"));
			ptConfig->Write(wxT("autostart"), fAutoStart);
			ptConfig->Write(wxT("autoexit"), fAutoExit);
			ptConfig->Write(wxT("autostarttest"), strAutostart);
		}
	}

	return iRes;
}

