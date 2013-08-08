#include "JC_ActionStatistics.h"

JC_ActionsStatistics::JC_ActionsStatistics() {
	JP_DBLog = 0;
	JP_DBLog_Size = 0;
}
//-----------------------------------------------------------------------------
bool JC_ActionsStatistics::Start(JI_CgiLogger * logger)
{
	JP_Logger = logger;

	JP_Hash = new JC_Hash();
	if (!JP_Hash->Start(100, JP_Logger, true))
		return false;

	return true;
}
//-----------------------------------------------------------------------------
bool Compare(void * A, void * B)
{
	return
		((JS_Action_Statistic *) A)->ActionID == ((JS_Action_Statistic *) B)->ActionID &&
		((JS_Action_Statistic *) A)->SensorID == ((JS_Action_Statistic *) B)->SensorID;
}
//-----------------------------------------------------------------------------
void JC_ActionsStatistics::Process(JS_DIDS * log)
{
	JS_Action_Statistic * rec, tmp;
	long key = log->ACTIONID ^ log->SENSORID;

	tmp.ActionID = log->ACTIONID;
	tmp.SensorID = log->SENSORID;

	rec = (JS_Action_Statistic *)JP_Hash->Get(key, sizeof(JS_Action_Statistic), &tmp, &Compare);
	rec->SensorID 	= log->SENSORID;
	rec->ActionID 	= log->ACTIONID;
	rec->Bytes     += log->BYTE;
	rec->Count	   += 1;
	rec->Packets   += log->PACKET;
}
//-----------------------------------------------------------------------------
void JC_ActionsStatistics::End()
{
	if (JP_Hash){
		JP_Hash->End();
		delete JP_Hash;
	}
}
//-----------------------------------------------------------------------------
JS_Action_Statistic_DB_Logs * JC_ActionsStatistics::GetLists()
{
	int i = 0;
	JP_DBLog_Size = JP_Hash->Size();

	JP_DBLog = (JS_Action_Statistic_DB_Logs*)JGetMem(sizeof(JS_Action_Statistic_DB_Logs));
	JP_DBLog->SensorID = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->ActionID = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->Bytes    = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->Count    = (int *)JGetMem(JP_DBLog_Size * sizeof(int));
	JP_DBLog->Packets  = (int *)JGetMem(JP_DBLog_Size * sizeof(int));

	JS_Action_Statistic * rec = 0;
	JS_Hash_Record * node = 0;
	node = JP_Hash->GetFirst();
	while (node) {
		rec = (JS_Action_Statistic *)node->p;
		JP_DBLog->SensorID[i] = rec->SensorID;
		JP_DBLog->ActionID[i] = rec->ActionID;
		JP_DBLog->Bytes[i] = rec->Bytes;
		JP_DBLog->Packets[i] = rec->Packets;
		JP_DBLog->Count[i] = rec->Count;
		node = JP_Hash->GetNext();
		i++;
	}
	return JP_DBLog;
}
//-----------------------------------------------------------------------------
void JC_ActionsStatistics::FreeDBLists()
{
	JFreeMem(JP_DBLog->SensorID, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->ActionID, sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->Bytes, 	 sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->Packets,  sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog->Count, 	 sizeof(int) * JP_DBLog_Size);
	JFreeMem(JP_DBLog, 			 sizeof(JS_Action_Statistic_DB_Logs));
	JP_Hash->Clear();
	JP_DBLog_Size = 0;
}
