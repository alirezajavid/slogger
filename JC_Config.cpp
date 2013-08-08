/*
 * JC_Config.cpp
 *
 *  Created on: Jun 19, 2012
 *      Author: root
 */

#include "JC_Config.h"

JC_Config::JC_Config() {

}
//-----------------------------------------------------------------------------
bool JC_Config::Start() {

	if (!LoadConfig())
		return false;
	return true;
}
//-----------------------------------------------------------------------------
void JC_Config::End() {

}
//-----------------------------------------------------------------------------
bool JC_Config::LoadConfig()
{
	if (!LoadDatabseConfig())
		return false;

	return true;

}
//-----------------------------------------------------------------------------
bool JC_Config::LoadDatabseConfig() {
	MC_IniParser * inifile;
	const char * p;

	inifile = new MC_IniParser();
	if (!inifile->Start(MCONST_CONFIG_FILE))
	{
		inifile->End();
		delete inifile;
		return false;
	}

	strcpy(JP_Config.MySQL.Hostname, inifile->GetVal("MySQL", "host", ""));
	strcpy(JP_Config.MySQL.Username, inifile->GetVal("MySQL", "user", ""));
	strcpy(JP_Config.MySQL.Password, inifile->GetVal("MySQL", "pass", ""));
	strcpy(JP_Config.MySQL.DBName, inifile->GetVal("MySQL", "dbname", ""));

	strcpy(JP_Config.Oracle.Hostname, inifile->GetVal("Oracle", "host", ""));
	strcpy(JP_Config.Oracle.Username, inifile->GetVal("Oracle", "user", ""));
	strcpy(JP_Config.Oracle.Password, inifile->GetVal("Oracle", "pass", ""));


	p = inifile->GetVal("Provider", "Type", "");
	if (!strcmp(p, "UDP"))
		JP_App_Config.Provider.Type = JE_Provider_Type_UDP;
	else if (!strcmp(p, "FILE"))
		JP_App_Config.Provider.Type = JE_Provider_Type_File;
	else if (!strcmp(p, "PCAP"))
		JP_App_Config.Provider.Type = JE_Provider_Type_PCap;
	else
		JP_App_Config.Provider.Type = JE_Provider_Type_None;

	strcpy(JP_App_Config.Provider.SourcePath, inifile->GetVal("Provider", "SourcePath", ""));
	strcpy(JP_App_Config.Provider.PCapFileName, inifile->GetVal("Provider", "File", ""));

	JP_App_Config.PreProcessors.Syslog =  (atoi(inifile->GetVal("Preprocessors", "Syslog", ""))) > 0;

	JP_App_Config.Logger.DatabaseWrite =  (atoi(inifile->GetVal("Logger", "DatabaseWriter", ""))) > 0;
	JP_App_Config.Logger.FileWrite =  (atoi(inifile->GetVal("Logger", "FileWriter", ""))) > 0;
	JP_App_Config.Logger.FastbitWrite =  (atoi(inifile->GetVal("Logger", "FastbitWrite", ""))) > 0;
	JP_App_Config.Logger.OracleWrite =  (atoi(inifile->GetVal("Logger", "OracleWrite", ""))) > 0;
	strcpy(JP_App_Config.Logger.FastbitPath, inifile->GetVal("Logger", "FastbitPath", ""));


	JP_App_Config.Logger.OracleBulkSize =  atoi(inifile->GetVal("Logger", "OracleBulkSize", ""))?atoi(inifile->GetVal("Logger", "OracleBulkSize", "")):1;
	JP_App_Config.Logger.FastbitBulkSize =  atoi(inifile->GetVal("Logger", "FastbitBulkSize", ""))?atoi(inifile->GetVal("Logger", "FastbitBulkSize", "")):1;

	JP_App_Config.Logger.WriteRateStatisticsInterval =  atoi(inifile->GetVal("Logger", "WriteRateStatisticsInterval", ""));
	JP_App_Config.Logger.WriteRateStatisticsActionInterval =  atoi(inifile->GetVal("Logger", "WriteRateStatisticsActionInterval", ""));

	strcpy(JP_App_Config.Logger.DestinationPath, inifile->GetVal("Logger", "DestinationPath", ""));
	JP_App_Config.General.DeleteBeforeHours = atoi(inifile->GetVal("General", "DeleteBeforeHours", "24"));

	inifile->End();
	delete inifile;
	return true;
}
