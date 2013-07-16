#include "JC_PreProcessorManager.h"

JC_PreProcessorManager::JC_PreProcessorManager(){
}
//-----------------------------------------------------------------------------
bool JC_PreProcessorManager::Start()
{
	return true;
}
//-----------------------------------------------------------------------------
void JC_PreProcessorManager::End()
{
	std::vector<MI_Preprocesseor*>::iterator it;
	for(it=JP_Preprocesseors.begin();it!=JP_Preprocesseors.end();it++)
	{
	    switch ((*it)->GetType())
	    {
			case JE_Preprocessor_Type_Syslog:
				((JC_Preprocesseor_Syslog *)(*it))->End();
				break;
			default:
				break;
	    }
	    if (*it)
	    	delete *it;
	}
	JP_Preprocesseors.clear();
}
//-----------------------------------------------------------------------------
void JC_PreProcessorManager::AddPreprocesor(MI_Preprocesseor* in)
{
	JP_Preprocesseors.push_back(in);
}
//-----------------------------------------------------------------------------
bool JC_PreProcessorManager::Preprocess(MS_DataProviderBuffer* inBuf, OUT MS_PreparedDataForLogger* buffer, OUT int & count)
{
	std::vector<MI_Preprocesseor*>::iterator it;
	for(it=JP_Preprocesseors.begin();it!=JP_Preprocesseors.end();it++)
		(*it)->Process(inBuf, buffer, count);
	return true;
}
