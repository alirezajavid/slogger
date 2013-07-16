#include "JC_RatesStatistics.h"

JC_RatesStatistics::JC_RatesStatistics() {
	JP_DBLog = 0;
	JP_DBLog_Size = 0;
}
//-----------------------------------------------------------------------------
bool JC_RatesStatistics::Start(JI_CgiLogger * logger)
{
	JP_Logger = logger;

	JP_Hash = new JC_Hash();
	if (!JP_Hash->Start(100, JP_Logger, true))
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void JC_RatesStatistics::Process(JS_DIDS * log)
{
	JS_Rate_Statistic * rec;
	rec = (JS_Rate_Statistic *)JP_Hash->Get(log->SENSORID, sizeof(JS_Rate_Statistic), 0, 0);
	rec->SensorID = log->SENSORID;
	rec->ssize = log->ssize;
	rec->size = log->size;
	rec->pcount = log->pcount;
	rec->tcprate = log->tcprate;
	rec->udprate = log->udprate;
	rec->icmprate = log->icmprate;
	rec->orate = log->orate;
	rec->serate = log->serate;
	rec->usrate = log->usrate;
	rec->secount = log->secount;
	rec->uscount = log->uscount;
	rec->tcpratep = log->tcprate;
	rec->udpratep = log->udprate;
	rec->icmpratep = log->icmpratep;
	rec->otherratep = log->otherratep;

}
//-----------------------------------------------------------------------------
void JC_RatesStatistics::End()
{
	if (JP_Hash){
		JP_Hash->End();
		delete JP_Hash;
	}
}
//-----------------------------------------------------------------------------
JS_Rate_Statistic_DB_Logs * JC_RatesStatistics::GetLists()
{
	int i = 0;
	JP_DBLog_Size = JP_Hash->Size();

	JP_DBLog = (JS_Rate_Statistic_DB_Logs*)JGetMem(sizeof(JS_Rate_Statistic_DB_Logs));
	JP_DBLog->SensorID = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->icmprate = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->icmpratep =  (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->orate = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->otherratep = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->pcount = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->secount = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->serate = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->size = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->ssize = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->tcprate = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->tcpratep = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->udprate = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->udpratep = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->uscount = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->usrate = (int *)JGetMem(JP_DBLog_Size * sizeof(int));

	JS_Rate_Statistic * rec = 0;
	JS_Hash_Record * node = 0;
	node = JP_Hash->GetFirst();
	while (node) {
		rec = (JS_Rate_Statistic *)node->p;
		JP_DBLog->SensorID[i] = rec->SensorID;
		JP_DBLog->icmprate[i] = rec->icmprate;
		JP_DBLog->icmpratep[i] = rec->icmpratep;
		JP_DBLog->orate[i] = rec->orate;
		JP_DBLog->otherratep[i] = rec->otherratep;
		JP_DBLog->pcount[i] = rec->pcount;
		JP_DBLog->secount[i] = rec->secount;
		JP_DBLog->serate[i] = rec->serate;
		JP_DBLog->size[i] = rec->size;
		JP_DBLog->ssize[i] = rec->ssize;
		JP_DBLog->tcprate[i] = rec->tcprate;
		JP_DBLog->tcpratep[i] = rec->tcpratep;
		JP_DBLog->udprate[i] = rec->udprate;
		JP_DBLog->udpratep[i] = rec->udpratep;
		JP_DBLog->uscount[i] = rec->uscount;
		JP_DBLog->usrate[i] = rec->usrate;
		node = JP_Hash->GetNext();
		i++;
	}
	return JP_DBLog;
}
//-----------------------------------------------------------------------------
void JC_RatesStatistics::FreeDBLists()
{
	JFreeMem(JP_DBLog->SensorID, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->icmprate, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->icmpratep, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->orate, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->otherratep, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->pcount, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->secount, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->serate, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->size, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->ssize, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->tcprate, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->tcpratep, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->udprate, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->udpratep, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->uscount, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->usrate, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog, sizeof(JS_Rate_Statistic_DB_Logs));
	JP_Hash->Clear();
	JP_DBLog_Size = 0;
}
