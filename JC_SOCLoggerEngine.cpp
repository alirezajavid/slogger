#include "JC_SOCLoggerEngine.h"

JC_SOCLoggerEngine::JC_SOCLoggerEngine() {
	JP_Config =0;
	JP_DataProvider =0;
	MP_LogManager =0;
	MP_PreprocessorManager =0;
	JP_Logger =0;
}
//-----------------------------------------------------------------------------
bool JC_SOCLoggerEngine::Start()
{
	struct rlimit limit={RLIM_INFINITY, RLIM_INFINITY};
	if(setrlimit(RLIMIT_CORE,&limit)<0)
		printf("set core limit failed with error %s \n",strerror(errno));

	JP_Config = new JC_Config();
	if (!JP_Config->Start())
	{
		printf("could not load config files \n");
		return false;
	}

	JP_Logger = new JC_DLogger();
	if (!JP_Logger->Start())
	{
		printf("could not start Log manager \n");
		return false;
	}

	if (!DatabaseTest())
	{
		JP_Logger->Log("could not connect to Oracle Database");
		//return false;
	}

	//==========================================================================================================
	MP_PreprocessorManager=new JC_PreProcessorManager();
	if (!MP_PreprocessorManager->Start())
	{
		JP_Logger->Log("Could not start preprocessor manager");
		return false;
	}

	if (JP_Config->GetConfig()->PreProcessors.Syslog)
	{
		JC_Preprocesseor_Syslog * pre = new JC_Preprocesseor_Syslog();
		if (!pre->Start(JP_Config, JP_Logger))
		{
			JP_Logger->Log("Could not start syslog preproccessor");
			return false;
		}
		MP_PreprocessorManager->AddPreprocesor(pre);
	}


	//==========================================================================================================
	MP_LogManager=new JC_LogManager();
	if (!MP_LogManager->Start())
	{
		JP_Logger->Log("Could not start Log manager");
		return false;
	}

	if (JP_Config->GetConfig()->Logger.DatabaseWrite)
	{
		JC_Logger_DatabaseWriter * lg = new JC_Logger_DatabaseWriter();
		if (!lg->Start(JP_Logger, JP_Config))
		{
			JP_Logger->Log("Could not start databasewriter logger");
			return false;
		}
		MP_LogManager->AddLogger(lg);
	}

	if (JP_Config->GetConfig()->Logger.FileWrite)
	{
		JC_Logger_FileWriter *lg = new JC_Logger_FileWriter();
		if (!lg->Start(JP_Logger, JP_Config))
		{
			JP_Logger->Log("Could not start statistic logger");
			return false;
		}
		MP_LogManager->AddLogger(lg);
	}

	bool b = false;
	switch (JP_Config->GetConfig()->Provider.Type) {
		case JE_Provider_Type_UDP:
			JP_DataProvider=new JC_DataProvider_UDP();
			b = ((JC_DataProvider_UDP *)JP_DataProvider)->Start(this);
			break;
		case JE_Provider_Type_File:
			JP_DataProvider=new JC_DataProvider_TextFile();
			b = ((JC_DataProvider_TextFile *)JP_DataProvider)->Start(this);
			break;
		case JE_Provider_Type_PCap:
			JP_DataProvider=new JC_DataProvider_PCap();
			b = ((JC_DataProvider_PCap *)JP_DataProvider)->Start(this);
			break;
		default:
			b = false;
			break;
	}

	if (!b) {
		JP_Logger->Log("Could not start DataProvider. ");
		return false;
	}
	JP_DataProvider->BeginRecive();
	return true;
}
//-----------------------------------------------------------------------------
void JC_SOCLoggerEngine::OnReciveData(MS_DataProviderBuffer* raw_buff)
{
	int count;
	MS_PreparedDataForLogger PreparedBuffer[9];
	memset(&PreparedBuffer, 0, sizeof(MS_PreparedDataForLogger) * 9);
	if (MP_PreprocessorManager->Preprocess(raw_buff, PreparedBuffer, count))
		for (int i=0; i<count; i++)
		{
		    MP_LogManager->Log(&PreparedBuffer[i]);
		}
}

//-----------------------------------------------------------------------------
void JC_SOCLoggerEngine::End()
{
	unlink("/var/log/soc_logger/Logger.kill");
	if (JP_DataProvider)
	{
		switch (JP_DataProvider->GetType()) {
			case JE_Provider_Type_UDP:
				((JC_DataProvider_UDP *)JP_DataProvider)->End();
				break;
			case JE_Provider_Type_File:
				((JC_DataProvider_TextFile *)JP_DataProvider)->End();
				break;
			case JE_Provider_Type_PCap:
				((JC_DataProvider_PCap *)JP_DataProvider)->End();
				break;
			default:
				JP_Logger->Log("DataProvider End method not implemented!");
		}
		delete JP_DataProvider;
	}

	if (MP_LogManager) {
		MP_LogManager->End();
		delete MP_LogManager;
	}

	if (MP_PreprocessorManager) {
		MP_PreprocessorManager->End();
		delete MP_PreprocessorManager;
	}

	if (JP_Logger) {
		JP_Logger->End();
		delete JP_Logger;
	}

	if (JP_Config) {
		JP_Config->End();
		delete JP_Config;
	}
}
//-----------------------------------------------------------------------------
bool JC_SOCLoggerEngine::DatabaseTest()
{
	MI_Database * db = 0;
	if (JP_Config->GetConfig()->Logger.DatabaseWrite)
	{
		db = GetDataBase(JP_Config, JP_Logger, JE_Database_Type_Oracle);
		if (!db)
			return false;
		db->End();
		delete db;
		db = 0;
	}
	return true;
}
