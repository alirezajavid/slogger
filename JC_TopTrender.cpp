#include "JC_TopTrender.h"

JC_TopTrender::JC_TopTrender() {
	JP_DBLog = 0;
	JP_DBLog_Size = 0;
}
//-----------------------------------------------------------------------------
bool JC_TopTrender::Start(JI_CgiLogger * logger)
{
	JP_Logger = logger;
	JP_Hash_MID= new JC_Hash();

	if (!JP_Hash_MID->Start(16000, JP_Logger, true))
		return false;

	JP_Hash_SIP = new JC_Hash();
	if (!JP_Hash_SIP->Start(65536, JP_Logger, false))
		return false;

	JP_Hash_DIP = new JC_Hash();
	if (!JP_Hash_DIP->Start(65536, JP_Logger, false))
		return false;

	JP_Hash_DPort = new JC_Hash();
	if (!JP_Hash_DPort->Start(65536, JP_Logger, true))
		return false;

	JP_Hash_SCountry = new JC_Hash();
	if (!JP_Hash_SCountry->Start(300, JP_Logger, true))
		return false;

	JP_Hash_DCountry = new JC_Hash();
	if (!JP_Hash_DCountry->Start(300, JP_Logger, true))
		return false;

	JP_Hash_Action = new JC_Hash();
	if (!JP_Hash_Action->Start(10, JP_Logger, true))
		return false;

	JP_Hash_SensorID = new JC_Hash();
	if (!JP_Hash_SensorID->Start(100, JP_Logger, true))
		return false;

	JP_Hash_MID->SetOnFreeNode((funcptr*)&OnFreeHash);
	JP_Hash_SIP->SetOnFreeNode((funcptr*)&OnFreeHash);
	JP_Hash_DIP->SetOnFreeNode((funcptr*)&OnFreeHash);
	JP_Hash_DPort->SetOnFreeNode((funcptr*)&OnFreeHash);
	JP_Hash_SCountry->SetOnFreeNode((funcptr*)&OnFreeHash);
	JP_Hash_DCountry->SetOnFreeNode((funcptr*)&OnFreeHash);
	JP_Hash_Action->SetOnFreeNode((funcptr*)&OnFreeHash);
	JP_Hash_SensorID->SetOnFreeNode((funcptr*)&OnFreeHash);

	return true;
}
//-----------------------------------------------------------------------------
void JC_TopTrender::Process(JS_DIDS * log)
{

	if (!Process_Hash(log, log->ACTIONID, 	JP_Hash_Action)) 	return ;
	if (!Process_Hash(log, log->SENSORID, 	JP_Hash_SensorID)) 	return ;
	if (!Process_Hash(log, log->MID, 		JP_Hash_MID)) 		return ;
	if (!Process_Hash(log, log->DPORT, 		JP_Hash_DPort)) 	return ;
	if (!Process_Hash(log, log->SCOUNTRY, 	JP_Hash_SCountry)) 	return ;
	if (!Process_Hash(log, log->DCOUNTRY, 	JP_Hash_DCountry)) 	return ;
	//if (!Process_Hash(log, log->SIP, 		JP_Hash_SIP)) 		return ;
	//if (!Process_Hash(log, log->DIP, 		JP_Hash_DIP)) 		return ;
}
//-----------------------------------------------------------------------------
bool JC_TopTrender::Process_Hash(JS_DIDS * log, int Key, JC_Hash * hash)
{
	JS_Statisticer * rec;
	rec = (JS_Statisticer *)hash->Get(Key, sizeof(JS_Statisticer), 0, 0);
	if (!rec)
	{
		JP_Logger->Log("Could not get memory.", EXP);
		return false;
	}

	rec->Key = Key;
	rec->Bytes += log->BYTE;
	rec->Packets += log->PACKET;
	rec->Count ++;

	if (rec->D_SIP == NULL) // Is new
	{
		rec->D_SIP = new JC_Hash();
		if (!rec->D_SIP->Start(16000, JP_Logger, false))
		{
			JP_Logger->Log("Could not get memory");
			return false;
		}

		rec->D_DIP = new JC_Hash();
		if (!rec->D_DIP->Start(16000, JP_Logger, false))
		{
			JP_Logger->Log("Could not get memory");
			return false;
		}

		rec->D_MID = new JC_Hash();
		if (!rec->D_MID->Start(1000, JP_Logger, false))
		{
			JP_Logger->Log("Could not get memory");
			return false;
		}

		rec->D_DPort = new JC_Hash();
		if (!rec->D_DPort->Start(8000, JP_Logger, false))
		{
			JP_Logger->Log("Could not get memory");
			return false;
		}
	}

	if (rec->D_Actions[log->ACTIONID / 8] & (1 << log->ACTIONID % 8) == 0)
		rec->ND_Actions++;
	rec->D_Actions[log->ACTIONID / 8] |= (1 << log->ACTIONID % 8);

	if (rec->D_DCountry[log->DCOUNTRY / 8] & (1 << log->DCOUNTRY % 8) == 0)
		rec->ND_DCountry++;
	rec->D_DCountry[log->DCOUNTRY / 8] |= (1 << log->DCOUNTRY % 8);

	if (rec->D_SCountry[log->SCOUNTRY / 8] & (1 << log->SCOUNTRY % 8) == 0)
		rec->ND_SCountry++;
	rec->D_SCountry[log->SCOUNTRY / 8] |= (1 << log->SCOUNTRY % 8);

	if (rec->D_SesorID[log->SENSORID / 8] & (1 << log->SENSORID % 8) == 0)
		rec->ND_SensorID++;
	rec->D_SesorID[log->SENSORID / 8] |= (1 << log->SENSORID % 8);

	//rec->D_SIP->Get(log->SIP, 0, 0, 0);
	//rec->D_DIP->Get(log->DIP, 0, 0, 0);
	rec->D_MID->Get(log->MID, 0, 0, 0);
	rec->D_DPort->Get(log->DPORT, 0, 0, 0);
	return true;
}
//-----------------------------------------------------------------------------
void JC_TopTrender::End()
{
	FreeHashs();
}
//-----------------------------------------------------------------------------
void JC_TopTrender::FreeHashs()
{
	FreeHash(JP_Hash_Action);
	FreeHash(JP_Hash_DCountry);
	FreeHash(JP_Hash_DIP);
	FreeHash(JP_Hash_DPort);
	FreeHash(JP_Hash_MID);
	FreeHash(JP_Hash_SCountry);
	FreeHash(JP_Hash_SIP);
	FreeHash(JP_Hash_SensorID);
}
//-----------------------------------------------------------------------------
void JC_TopTrender::OnFreeHash(void * p)
{
	JS_Statisticer * rec = (JS_Statisticer *)p;
	if (rec->D_DIP)
	{
		rec->D_DIP->End();
		delete rec->D_DIP;
	}

	if (rec->D_SIP)
	{
		rec->D_SIP->End();
		delete rec->D_SIP;
	}

	if (rec->D_MID)
	{
		rec->D_MID->End();
		delete rec->D_MID;
	}

	if (rec->D_DPort)
	{
		rec->D_DPort->End();
		delete rec->D_DPort;
	}

}
//-----------------------------------------------------------------------------
void JC_TopTrender::FreeHash(JC_Hash * hash)
{
	if (hash){
		hash->End();
		delete hash;
	}
}
//-----------------------------------------------------------------------------
int compare_JS_Statisticer (const void * a, const void * b)
{
  return ( ((JS_Statisticer*)a)->Bytes - ((JS_Statisticer*)b)->Bytes );
}
//-----------------------------------------------------------------------------
JS_Statistic_DB_Logs * JC_TopTrender::GetLists()
{
	struct JS_Statistic_Row {
		int Key;
		int N_MID;
		int N_Actions;
		int N_DPort;
		int N_SCountry;
		int N_DCountry;
		int N_SIP;
		int N_DIP;
		big_int  Packets;
		big_int  Bytes;
		big_int  Count;
	} * vForSort;


	int i = 0;
	JP_DBLog_Size = JP_Hash_Action->Size() +
					JP_Hash_DCountry->Size() +
					(JP_Hash_DIP->Size()>JSCONST_SORTED_BLUCK?JSCONST_SORTED_BLUCK:JP_Hash_DIP->Size()) +
					(JP_Hash_SIP->Size()>JSCONST_SORTED_BLUCK?JSCONST_SORTED_BLUCK:JP_Hash_SIP->Size()) +
					(JP_Hash_DPort->Size()>JSCONST_SORTED_BLUCK?JSCONST_SORTED_BLUCK:JP_Hash_DPort->Size()) +
					JP_Hash_MID->Size() +
					JP_Hash_SCountry->Size() +
					JP_Hash_SensorID->Size();
/*
	JP_Logger->Log(" Actions:: Size=%6d  Pivots=%6d/%6d  Tails=%6d MaxTail=%6d",JP_Hash_Action->Size(), 	JP_Hash_Action->Get_Dist_Pivot(),	JP_Hash_Action->Get_VectorSize(),	JP_Hash_Action->Get_Dist_Tails(), 	JP_Hash_Action->Get_MaxTailCnt());
	JP_Logger->Log("     MID:: Size=%6d  Pivots=%6d/%6d  Tails=%6d MaxTail=%6d",JP_Hash_MID->Size(), 	  	JP_Hash_MID->Get_Dist_Pivot(),		JP_Hash_MID->Get_VectorSize(),		JP_Hash_MID->Get_Dist_Tails(), 		JP_Hash_MID->Get_MaxTailCnt());
	JP_Logger->Log("SensorID:: Size=%6d  Pivots=%6d/%6d  Tails=%6d MaxTail=%6d",JP_Hash_SensorID->Size(), JP_Hash_SensorID->Get_Dist_Pivot(),	JP_Hash_SensorID->Get_VectorSize(),	JP_Hash_SensorID->Get_Dist_Tails(), JP_Hash_SensorID->Get_MaxTailCnt());
	JP_Logger->Log("     SIP:: Size=%6d  Pivots=%6d/%6d  Tails=%6d MaxTail=%6d",JP_Hash_SIP->Size(), 		JP_Hash_SIP->Get_Dist_Pivot(),		JP_Hash_SIP->Get_VectorSize(),		JP_Hash_SIP->Get_Dist_Tails(), 		JP_Hash_SIP->Get_MaxTailCnt());
	JP_Logger->Log("     DIP:: Size=%6d  Pivots=%6d/%6d  Tails=%6d MaxTail=%6d",JP_Hash_DIP->Size(), 		JP_Hash_DIP->Get_Dist_Pivot(),		JP_Hash_DIP->Get_VectorSize(),		JP_Hash_DIP->Get_Dist_Tails(), 		JP_Hash_DIP->Get_MaxTailCnt());
	JP_Logger->Log("SCountry:: Size=%6d  Pivots=%6d/%6d  Tails=%6d MaxTail=%6d",JP_Hash_SCountry->Size(), JP_Hash_SCountry->Get_Dist_Pivot(),	JP_Hash_SCountry->Get_VectorSize(),	JP_Hash_SCountry->Get_Dist_Tails(), JP_Hash_SCountry->Get_MaxTailCnt());
	JP_Logger->Log("DCountry:: Size=%6d  Pivots=%6d/%6d  Tails=%6d MaxTail=%6d",JP_Hash_DCountry->Size(), JP_Hash_DCountry->Get_Dist_Pivot(),	JP_Hash_DCountry->Get_VectorSize(),	JP_Hash_DCountry->Get_Dist_Tails(), JP_Hash_DCountry->Get_MaxTailCnt());
	JP_Logger->Log("   DPort:: Size=%6d  Pivots=%6d/%6d  Tails=%6d MaxTail=%6d",JP_Hash_DPort->Size(), 	JP_Hash_DPort->Get_Dist_Pivot(),	JP_Hash_DPort->Get_VectorSize(),	JP_Hash_DPort->Get_Dist_Tails(), 	JP_Hash_DPort->Get_MaxTailCnt());
*/
	JP_DBLog = 		(JS_Statistic_DB_Logs *)JGetMem(sizeof(JS_Statistic_DB_Logs));
	if (!JP_DBLog)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->Type 		= (JE_Statistic_Type *)JGetMem(sizeof(JE_Statistic_Type) * JP_DBLog_Size);
	if (!JP_DBLog->Type)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->Key 		= (int *)JGetMem(sizeof(int) * JP_DBLog_Size);
	if (!JP_DBLog->Key)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->N_MID 	= (int *)JGetMem(sizeof(int) * JP_DBLog_Size);
	if (!JP_DBLog->N_MID)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->N_Actions = (int *)JGetMem(sizeof(int) * JP_DBLog_Size);
	if (!JP_DBLog->N_Actions)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->N_SCountry= (int *)JGetMem(sizeof(int) * JP_DBLog_Size);
	if (!JP_DBLog->N_SCountry)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->N_DCountry= (int *)JGetMem(sizeof(int) * JP_DBLog_Size);
	if (!JP_DBLog->N_DCountry)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->Packets 	= (big_int *)JGetMem(sizeof(big_int) * JP_DBLog_Size);
	if (!JP_DBLog->Packets)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->Bytes 	= (big_int *)JGetMem(sizeof(big_int) * JP_DBLog_Size);
	if (!JP_DBLog->Bytes)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->Count 	= (big_int *)JGetMem(sizeof(big_int) * JP_DBLog_Size);
	if (!JP_DBLog->Count)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->N_SIP 	= (int *)JGetMem(sizeof(int) * JP_DBLog_Size);
	if (!JP_DBLog->N_SIP)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->N_DIP 	= (int *)JGetMem(sizeof(int) * JP_DBLog_Size);
	if (!JP_DBLog->N_DIP)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}
	JP_DBLog->N_DPort	= (int *)JGetMem(sizeof(int) * JP_DBLog_Size);
	if (!JP_DBLog->N_DPort)
	{
		FreeDBLists();
		JP_Logger->Log("Could not get memory", EXP);
		return 0;
	}

	JS_Statisticer * rec = 0;
	JS_Hash_Record * node = 0;

	node = JP_Hash_Action->GetFirst();
	while (node) {
		rec = (JS_Statisticer *)node->p;
		JP_DBLog->Bytes[i] = rec->Bytes;
		JP_DBLog->Count[i] = rec->Count;
		JP_DBLog->Key[i] = rec->Key;
		JP_DBLog->N_Actions[i] = rec->ND_Actions;
		JP_DBLog->N_DCountry[i] = rec->ND_DCountry;
		JP_DBLog->N_SCountry[i] = rec->ND_SCountry;
		JP_DBLog->Packets[i] = rec->Packets;
		JP_DBLog->Type[i] = JE_Statistic_Type_Action;

		JP_DBLog->N_DIP[i] = rec->D_DIP->Size();
		JP_DBLog->N_DPort[i] = rec->D_DPort->Size();
		JP_DBLog->N_MID[i] = rec->D_MID->Size();
		JP_DBLog->N_SIP[i] = rec->D_SIP->Size();

		node = JP_Hash_Action->GetNext();
		i++;
	}

	node = JP_Hash_DCountry->GetFirst();
	while (node) {
		rec = (JS_Statisticer *)node->p;
		JP_DBLog->Bytes[i] = rec->Bytes;
		JP_DBLog->Count[i] = rec->Count;
		JP_DBLog->Key[i] = rec->Key;
		JP_DBLog->N_Actions[i] = rec->ND_Actions;
		JP_DBLog->N_DCountry[i] = rec->ND_DCountry;
		JP_DBLog->N_DIP[i] = rec->D_DIP->Size();
		JP_DBLog->N_DPort[i] = rec->D_DPort->Size();
		JP_DBLog->N_MID[i] = rec->D_MID->Size();
		JP_DBLog->N_SIP[i] = rec->D_SIP->Size();
		JP_DBLog->N_SCountry[i] = rec->ND_SCountry;
		JP_DBLog->Packets[i] = rec->Packets;
		JP_DBLog->Type[i] = JE_Statistic_Type_DCountry;
		node = JP_Hash_DCountry->GetNext();
		i++;
	}
//-----------------------------------------------------------------------------
	int j=0;
	vForSort = (JS_Statistic_Row *)JGetMem(JP_Hash_DIP->Size() * sizeof(JS_Statistic_Row));
	node = JP_Hash_DIP->GetFirst();
	while (node) {
		rec = (JS_Statisticer *)node->p;
		vForSort[j].Bytes 		= rec->Bytes;
		vForSort[j].Count 		= rec->Count;
		vForSort[j].Packets		= rec->Packets;
		vForSort[j].Key   		= rec->Key;
		vForSort[j].N_Actions 	= rec->ND_Actions;
		vForSort[j].N_DCountry 	= rec->ND_DCountry;
		vForSort[j].N_DIP 		= rec->D_DIP->Size();
		vForSort[j].N_DPort 	= rec->D_DPort->Size();
		vForSort[j].N_MID 		= rec->D_MID->Size();
		vForSort[j].N_SCountry 	= rec->ND_SCountry;
		vForSort[j].N_SIP		= rec->D_SIP->Size();
		JP_Hash_DIP->GetNext();
		j++;
	}
	qsort(vForSort, j, sizeof(JS_Statisticer), compare_JS_Statisticer);
	for (j=0; j<(JP_Hash_DIP->Size()>(JSCONST_SORTED_BLUCK-1)?(JSCONST_SORTED_BLUCK-1):JP_Hash_DIP->Size()); j++)
	{
		JP_DBLog->Type[i] = JE_Statistic_Type_DIP;
		JP_DBLog->Key[i] = vForSort[j].Key;
		JP_DBLog->N_Actions[i] = vForSort[j].N_Actions;
		JP_DBLog->N_DCountry[i] = vForSort[j].N_DCountry;
		JP_DBLog->N_DIP[i] = vForSort[j].N_DIP;
		JP_DBLog->N_DPort[i] = vForSort[j].N_DPort;
		JP_DBLog->N_MID[i] = vForSort[j].N_MID;
		JP_DBLog->N_SIP[i] = vForSort[j].N_SIP;
		JP_DBLog->N_SCountry[i] = vForSort[j].N_SCountry;
		JP_DBLog->Bytes[i] = vForSort[j].Bytes;
		JP_DBLog->Count[i] = vForSort[j].Count;
		JP_DBLog->Packets[i] = vForSort[j].Packets;
		i++;
	}
	for (; j<JP_Hash_DIP->Size(); j++)
	{
		JP_DBLog->Key[i] = 0;
		JP_DBLog->Bytes[i] += vForSort[j].Bytes;
		JP_DBLog->Count[i] += vForSort[j].Count;
		JP_DBLog->N_Actions[i] += vForSort[j].N_Actions;
		JP_DBLog->N_DCountry[i] += vForSort[j].N_DCountry;
		JP_DBLog->N_DIP[i] += vForSort[j].N_DIP;
		JP_DBLog->N_DPort[i] += vForSort[j].N_DPort;
		JP_DBLog->N_MID[i] += vForSort[j].N_MID;
		JP_DBLog->N_SIP[i] += vForSort[j].N_SIP;
		JP_DBLog->N_SCountry[i] += vForSort[j].N_SCountry;
		JP_DBLog->Packets[i] += vForSort[j].Packets;
		JP_DBLog->Type[i] = JE_Statistic_Type_DIP;
	}
	i++;
	JFreeMem(vForSort, JP_Hash_DIP->Size() * sizeof(JS_Statistic_Row));
//-----------------------------------------------------------------------------
	j=0;
	vForSort = (JS_Statistic_Row *)JGetMem(JP_Hash_DPort->Size() * sizeof(JS_Statistic_Row));
	node = JP_Hash_DPort->GetFirst();
	while (node) {
		rec = (JS_Statisticer *)node->p;
		vForSort[j].Bytes 		= rec->Bytes;
		vForSort[j].Count 		= rec->Count;
		vForSort[j].Packets		= rec->Packets;
		vForSort[j].Key   		= rec->Key;
		vForSort[j].N_Actions 	= rec->ND_Actions;
		vForSort[j].N_DCountry 	= rec->ND_DCountry;
		vForSort[j].N_DIP 		= rec->D_DIP->Size();
		vForSort[j].N_DPort 	= rec->D_DPort->Size();
		vForSort[j].N_MID 		= rec->D_MID->Size();
		vForSort[j].N_SCountry 	= rec->ND_SCountry;
		vForSort[j].N_SIP		= rec->D_SIP->Size();
		JP_Hash_DPort->GetNext();
		j++;
	}
	qsort(vForSort, j, sizeof(JS_Statisticer), compare_JS_Statisticer);
	for (j=0; j<(JP_Hash_DPort->Size()>(JSCONST_SORTED_BLUCK-1)?(JSCONST_SORTED_BLUCK-1):JP_Hash_DPort->Size()); j++)
	{
		JP_DBLog->Type[i] = JE_Statistic_Type_DPort;
		JP_DBLog->Key[i] = vForSort[j].Key;
		JP_DBLog->N_Actions[i] = vForSort[j].N_Actions;
		JP_DBLog->N_DCountry[i] = vForSort[j].N_DCountry;
		JP_DBLog->N_DIP[i] = vForSort[j].N_DIP;
		JP_DBLog->N_DPort[i] = vForSort[j].N_DPort;
		JP_DBLog->N_MID[i] = vForSort[j].N_MID;
		JP_DBLog->N_SIP[i] = vForSort[j].N_SIP;
		JP_DBLog->N_SCountry[i] = vForSort[j].N_SCountry;
		JP_DBLog->Bytes[i] = vForSort[j].Bytes;
		JP_DBLog->Count[i] = vForSort[j].Count;
		JP_DBLog->Packets[i] = vForSort[j].Packets;
		i++;
	}
	for (; j<JP_Hash_DPort->Size(); j++)
	{
		JP_DBLog->Key[i] = 0;
		JP_DBLog->Bytes[i] += vForSort[j].Bytes;
		JP_DBLog->Count[i] += vForSort[j].Count;
		JP_DBLog->N_Actions[i] += vForSort[j].N_Actions;
		JP_DBLog->N_DCountry[i] += vForSort[j].N_DCountry;
		JP_DBLog->N_DIP[i] += vForSort[j].N_DIP;
		JP_DBLog->N_DPort[i] += vForSort[j].N_DPort;
		JP_DBLog->N_MID[i] += vForSort[j].N_MID;
		JP_DBLog->N_SIP[i] += vForSort[j].N_SIP;
		JP_DBLog->N_SCountry[i] += vForSort[j].N_SCountry;
		JP_DBLog->Packets[i] += vForSort[j].Packets;
		JP_DBLog->Type[i] = JE_Statistic_Type_DPort;
	}
	i++;
	JFreeMem(vForSort, JP_Hash_DPort->Size() * sizeof(JS_Statistic_Row));
//-----------------------------------------------------------------------------
	node = JP_Hash_MID->GetFirst();
	while (node) {
		rec = (JS_Statisticer *)node->p;
		JP_DBLog->Bytes[i] = rec->Bytes;
		JP_DBLog->Count[i] = rec->Count;
		JP_DBLog->Key[i] = rec->Key;
		JP_DBLog->N_Actions[i] = rec->ND_Actions;
		JP_DBLog->N_DCountry[i] = rec->ND_DCountry;
		JP_DBLog->N_DIP[i] = rec->D_DIP->Size();
		JP_DBLog->N_DPort[i] = rec->D_DPort->Size();
		JP_DBLog->N_MID[i] = rec->D_MID->Size();
		JP_DBLog->N_SIP[i] = rec->D_SIP->Size();
		JP_DBLog->N_SCountry[i] = rec->ND_SCountry;
		JP_DBLog->Packets[i] = rec->Packets;
		JP_DBLog->Type[i] = JE_Statistic_Type_MID;
		node = JP_Hash_MID->GetNext();
		i++;
	}

	node = JP_Hash_SCountry->GetFirst();
	while (node) {
		rec = (JS_Statisticer *)node->p;
		JP_DBLog->Bytes[i] = rec->Bytes;
		JP_DBLog->Count[i] = rec->Count;
		JP_DBLog->Key[i] = rec->Key;
		JP_DBLog->N_Actions[i] = rec->ND_Actions;
		JP_DBLog->N_DCountry[i] = rec->ND_DCountry;
		JP_DBLog->N_DIP[i] = rec->D_DIP->Size();
		JP_DBLog->N_DPort[i] = rec->D_DPort->Size();
		JP_DBLog->N_MID[i] = rec->D_MID->Size();
		JP_DBLog->N_SIP[i] = rec->D_SIP->Size();
		JP_DBLog->N_SCountry[i] = rec->ND_SCountry;
		JP_DBLog->Packets[i] = rec->Packets;
		JP_DBLog->Type[i] = JE_Statistic_Type_SCountry;
		node = JP_Hash_SCountry->GetNext();
		i++;
	}
//-----------------------------------------------------------------------------
	j=0;
	vForSort = (JS_Statistic_Row *)JGetMem(JP_Hash_SIP->Size() * sizeof(JS_Statistic_Row));
	node = JP_Hash_SIP->GetFirst();
	while (node) {
		rec = (JS_Statisticer *)node->p;
		vForSort[j].Bytes 		= rec->Bytes;
		vForSort[j].Count 		= rec->Count;
		vForSort[j].Packets		= rec->Packets;
		vForSort[j].Key   		= rec->Key;
		vForSort[j].N_Actions 	= rec->ND_Actions;
		vForSort[j].N_DCountry 	= rec->ND_DCountry;
		vForSort[j].N_DIP 		= rec->D_DIP->Size();
		vForSort[j].N_DPort 	= rec->D_DPort->Size();
		vForSort[j].N_MID 		= rec->D_MID->Size();
		vForSort[j].N_SCountry 	= rec->ND_SCountry;
		vForSort[j].N_SIP		= rec->D_SIP->Size();
		JP_Hash_SIP->GetNext();
		j++;
	}
	qsort(vForSort, j, sizeof(JS_Statisticer), compare_JS_Statisticer);
	for (j=0; j<(JP_Hash_SIP->Size()>(JSCONST_SORTED_BLUCK-1)?(JSCONST_SORTED_BLUCK-1):JP_Hash_SIP->Size()); j++)
	{
		JP_DBLog->Type[i] = JE_Statistic_Type_SIP;
		JP_DBLog->Key[i] = vForSort[j].Key;
		JP_DBLog->N_Actions[i] = vForSort[j].N_Actions;
		JP_DBLog->N_DCountry[i] = vForSort[j].N_DCountry;
		JP_DBLog->N_DIP[i] = vForSort[j].N_DIP;
		JP_DBLog->N_DPort[i] = vForSort[j].N_DPort;
		JP_DBLog->N_MID[i] = vForSort[j].N_MID;
		JP_DBLog->N_SIP[i] = vForSort[j].N_SIP;
		JP_DBLog->N_SCountry[i] = vForSort[j].N_SCountry;
		JP_DBLog->Bytes[i] = vForSort[j].Bytes;
		JP_DBLog->Count[i] = vForSort[j].Count;
		JP_DBLog->Packets[i] = vForSort[j].Packets;
		i++;
	}
	for (; j<JP_Hash_SIP->Size(); j++)
	{
		JP_DBLog->Key[i] = 0;
		JP_DBLog->Bytes[i] += vForSort[j].Bytes;
		JP_DBLog->Count[i] += vForSort[j].Count;
		JP_DBLog->N_Actions[i] += vForSort[j].N_Actions;
		JP_DBLog->N_DCountry[i] += vForSort[j].N_DCountry;
		JP_DBLog->N_DIP[i] += vForSort[j].N_DIP;
		JP_DBLog->N_DPort[i] += vForSort[j].N_DPort;
		JP_DBLog->N_MID[i] += vForSort[j].N_MID;
		JP_DBLog->N_SIP[i] += vForSort[j].N_SIP;
		JP_DBLog->N_SCountry[i] += vForSort[j].N_SCountry;
		JP_DBLog->Packets[i] += vForSort[j].Packets;
		JP_DBLog->Type[i] = JE_Statistic_Type_SIP;
	}
	i++;
	JFreeMem(vForSort, JP_Hash_SIP->Size() * sizeof(JS_Statistic_Row));
//-----------------------------------------------------------------------------
	node = JP_Hash_SensorID->GetFirst();
	while (node) {
		rec = (JS_Statisticer *)node->p;
		JP_DBLog->Bytes[i] = rec->Bytes;
		JP_DBLog->Count[i] = rec->Count;
		JP_DBLog->Key[i] = rec->Key;
		JP_DBLog->N_Actions[i] = rec->ND_Actions;
		JP_DBLog->N_DCountry[i] = rec->ND_DCountry;
		JP_DBLog->N_DIP[i] = rec->D_DIP->Size();
		JP_DBLog->N_DPort[i] = rec->D_DPort->Size();
		JP_DBLog->N_MID[i] = rec->D_MID->Size();
		JP_DBLog->N_SIP[i] = rec->D_SIP->Size();
		JP_DBLog->N_SCountry[i] = rec->ND_SCountry;
		JP_DBLog->Packets[i] = rec->Packets;
		JP_DBLog->Type[i] = JE_Statistic_Type_SensorID;
		node = JP_Hash_SensorID->GetNext();
		i++;
	}
	return JP_DBLog;
}
//-----------------------------------------------------------------------------
void JC_TopTrender::FreeDBLists()
{
	if (JP_DBLog->Type)
		JFreeMem(JP_DBLog->Type, sizeof(JE_Statistic_Type) * JP_DBLog_Size);
	if (JP_DBLog->Key)
		JFreeMem(JP_DBLog->Key, sizeof(int) * JP_DBLog_Size);
	if (JP_DBLog->Packets)
		JFreeMem(JP_DBLog->Packets, sizeof(big_int) * JP_DBLog_Size);
	if (JP_DBLog->Bytes)
		JFreeMem(JP_DBLog->Bytes, sizeof(big_int) * JP_DBLog_Size);
	if (JP_DBLog->Count)
		JFreeMem(JP_DBLog->Count, sizeof(big_int) * JP_DBLog_Size);
	if (JP_DBLog->N_MID)
		JFreeMem(JP_DBLog->N_MID, sizeof(int) * JP_DBLog_Size);
	if (JP_DBLog->N_Actions)
		JFreeMem(JP_DBLog->N_Actions, sizeof(int) * JP_DBLog_Size);
	if (JP_DBLog->N_DPort)
		JFreeMem(JP_DBLog->N_DPort, sizeof(int) * JP_DBLog_Size);
	if (JP_DBLog->N_SCountry)
		JFreeMem(JP_DBLog->N_SCountry, sizeof(int) * JP_DBLog_Size);
	if (JP_DBLog->N_DCountry)
		JFreeMem(JP_DBLog->N_DCountry, sizeof(int) * JP_DBLog_Size);
	if (JP_DBLog->N_SIP)
		JFreeMem(JP_DBLog->N_SIP, sizeof(int) * JP_DBLog_Size);
	if (JP_DBLog->N_SIP)
		JFreeMem(JP_DBLog->N_DIP, sizeof(int) * JP_DBLog_Size);
	if (JP_DBLog)
		JFreeMem(JP_DBLog, sizeof(JS_Statistic_DB_Logs));

	JP_DBLog->Type = 0;
	JP_DBLog->Key = 0;
	JP_DBLog->Packets = 0;
	JP_DBLog->Bytes = 0;
	JP_DBLog->Count = 0;
	JP_DBLog->N_MID = 0;
	JP_DBLog->N_Actions = 0;
	JP_DBLog->N_DPort = 0;
	JP_DBLog->N_SCountry = 0;
	JP_DBLog->N_DCountry = 0;
	JP_DBLog->N_SIP = 0;
	JP_DBLog->N_DIP = 0;
	JP_DBLog = 0;

	JP_Hash_MID->Clear();
	JP_Hash_SIP->Clear();
	JP_Hash_DIP->Clear();
	JP_Hash_DPort->Clear();
	JP_Hash_SCountry->Clear();
	JP_Hash_DCountry->Clear();
	JP_Hash_Action->Clear();
	JP_Hash_SensorID->Clear();

	JP_DBLog_Size = 0;
}
