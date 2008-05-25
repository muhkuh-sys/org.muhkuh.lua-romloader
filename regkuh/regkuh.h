

#ifndef __MREG_H__
#define __MREG_H__

#include "application/muhkuh_plugin_manager.h"
#include "application/muhkuh_repository_manager.h"


class muhkuh_regApp
{
public:
	muhkuh_regApp(void);
	~muhkuh_regApp();

	int execute_operation(wxCmdLineParser *ptPar);

private:
	void clearSettings(void);
	void cleanUp(void);

	void showVersion(void);
	void showHelp(void);

	int parse_args();

	int open_config_file(void);
	int close_config_file(void);

	int process_parameter(wxString &strParam);
	int custom_title(wxArrayString &aParam);
	int custom_icon(wxArrayString &aParam);
	int register_plugin(wxArrayString &aParam);
	int register_repository(wxArrayString &aParam);
	int autostart(wxArrayString &aParam);

	wxCmdLineParser *ptParser;

	bool m_fCfgVerbose;
	bool m_fCfgShowHelp;
	bool m_fCfgShowVersion;
	wxString m_strCfgConfigFile;
	wxString m_strExePath;

	// the plugin manager
	muhkuh_plugin_manager *m_ptPluginManager;

	// the repository manager
	muhkuh_repository_manager *m_ptRepositoryManager;
};


#endif	// __MREG_H__
