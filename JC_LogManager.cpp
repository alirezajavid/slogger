#include "JC_LogManager.h"
#include "JC_Logger_DatabaseWriter.h"

JC_LogManager::JC_LogManager() {
}
//-----------------------------------------------------------------------------
bool JC_LogManager::Log(MS_PreparedDataForLogger*  preparedBuffer)
{
	std::vector<MI_Logger*>::iterator it;
	for(it=JP_Loggers.begin();it!=JP_Loggers.end();it++)
		(*it)->Log(preparedBuffer);
	ClearProcessBuffer(preparedBuffer);
	return false;
}
//-----------------------------------------------------------------------------
void JC_LogManager::AddLogger(MI_Logger* in)
{
	JP_Loggers.push_back(in);
}
//-----------------------------------------------------------------------------
bool JC_LogManager::Start()
{
	return true;
}
//-----------------------------------------------------------------------------
void JC_LogManager::End()
{
	std::vector<MI_Logger*>::iterator it;
	for(it=JP_Loggers.begin();it!=JP_Loggers.end();it++)
	{
		switch ((*it)->GetType())
		{
			case JE_Logger_Type_FileWriter:
				((JC_Logger_FileWriter *)(*it))->End();
				break;
			case JE_Logger_Type_DatabaseWriter:
				((JC_Logger_DatabaseWriter *)(*it))->End();
				break;
			default:
				throw "Logger End method not implemented (LogManager)!";
				break;
		}
		delete *it;
	}
	JP_Loggers.clear();
}
//-----------------------------------------------------------------------------
void JC_LogManager::ClearProcessBuffer(MS_PreparedDataForLogger*  buff)
{

}
