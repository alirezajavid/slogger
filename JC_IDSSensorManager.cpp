#include "JC_IDSSensorManager.h"

JC_IDSSensorManager::JC_IDSSensorManager() {
}
//-----------------------------------------------------------------------------
bool JC_IDSSensorManager::Start(JC_Config * config, JI_CgiLogger * logger)
{
	JP_Config = config;
	JP_Logger = logger;

	JP_Vector = new JC_Hash();
	if (!JP_Vector->Start(100, JP_Logger, false))
	{
		JP_Logger->Log("Could not create Hash List");
		return false;
	}
	LoadLibrary();
	return true;
}
//-----------------------------------------------------------------------------
void JC_IDSSensorManager::End()
{
	if (JP_Vector)
	{
		JP_Vector->End();
		delete JP_Vector;
	}
}
//-----------------------------------------------------------------------------
bool Compare_IDS_Hosts(void * p1, void * p2)
{
	JS_IDS_Hosts *_p1, *_p2;
	_p1 = (JS_IDS_Hosts *)p1;
	_p2 = (JS_IDS_Hosts *)p2;
	return (strcmp(_p1->Hostname, _p2->Hostname) == 0) && (_p1->ip == _p2->ip);
}
void JC_IDSSensorManager::LoadLibrary()
{
	MI_Database * db = GetDataBase(JP_Config, JP_Logger, JE_Database_Type_Oracle);
	MI_DataBaseRow * row;
	JS_IDS_Hosts *p, tmp;
	long k;
	db->PreperSQL("SELECT IP, ID, HOSTNAME FROM DIDS_HOSTS");
	if (!db->Execute())
	{
		JP_Logger->Log("Could not get ids_hosts");
		return ;
	}
	while ((row = db->Read()) != NULL)
	{
		memset(KeyStr.Key, 0, sizeof(KeyStr.Key));
		tmp.id = row->GetFieldInt(2);
		KeyStr.Pair.ip = row->GetFieldIP(1);
		strncpy(KeyStr.Pair.HostName, row->GetFieldText(3), sizeof(KeyStr.Pair.HostName) - 1);
		k = JP_Vector->Jhash(KeyStr.Key, sizeof(KeyStr.Key), 100);

		tmp.id = row->GetFieldInt(2);
		tmp.ip = row->GetFieldIP(1);
		strcpy(tmp.Hostname, row->GetFieldText(3));

		p = (JS_IDS_Hosts *)JP_Vector->Get(k, sizeof(JS_IDS_Hosts), &tmp, Compare_IDS_Hosts);
		memcpy(p, &tmp, sizeof(JS_IDS_Hosts));
	}
	db->End();
	delete db;
}
//-----------------------------------------------------------------------------
int JC_IDSSensorManager::GetHostID(const char * host, IP ip)
{
	long k;
	JS_IDS_Hosts *p, tmp;
	memset(KeyStr.Key, 0, sizeof(KeyStr.Key));
	memset(&tmp, 0, sizeof(tmp));
	strncpy(KeyStr.Pair.HostName, host, sizeof(KeyStr.Pair.HostName) - 1);
	KeyStr.Pair.ip = ip;
	k = JP_Vector->Jhash(KeyStr.Key, sizeof(KeyStr.Key), 100);
	strcpy(tmp.Hostname, host);
	tmp.ip = ip;
	p = (JS_IDS_Hosts *)JP_Vector->Get(k, &tmp, Compare_IDS_Hosts);
	if (p)
		return p->id;
	else
		return AddNewHost(host, ip);
}
//-----------------------------------------------------------------------------
int JC_IDSSensorManager::AddNewHost(const char * host, IP ip)
{
	MI_Database * db = GetDataBase(JP_Config, JP_Logger, JE_Database_Type_Oracle);
	MI_DataBaseRow * row;
	JS_IDS_Hosts *p, tmp;
	long k;

	memset(&tmp, 0, sizeof(tmp));
	db->PreperSQL("INSERT INTO dids_HOSTS (HOSTNAME,IP) VALUES (?,?)");
	db->BindText(0, host);
	db->BindIP(1, ip);
	if (!db->Execute())
	{
		JP_Logger->Log("Could not Add New Host1");
		return 0;
	}
	db->Finalize();

	db->PreperSQL("SELECT ID FROM dids_HOSTS WHERE (HOSTNAME = ?) AND (IP = ?)");
	db->BindText(0, host);
	db->BindIP(1, ip);
	if (!db->Execute())
	{
		JP_Logger->Log("Could not Add New host2");
		return 0;
	}
	row = db->Read();
	int id = row->GetFieldInt(1);
	db->Finalize();

	db->End();
	delete db;

	memset(KeyStr.Key, 0, sizeof(KeyStr.Key));
	memset(&tmp, 0, sizeof(tmp));
	strncpy(KeyStr.Pair.HostName, host, sizeof(KeyStr.Pair.HostName) - 1);
	strncpy(tmp.Hostname, host, sizeof(KeyStr.Pair.HostName) - 1);
	tmp.ip = ip;
	KeyStr.Pair.ip = ip;
	k = JP_Vector->Jhash(KeyStr.Key, sizeof(KeyStr.Key), 100);
	p = (JS_IDS_Hosts *)JP_Vector->Get(k, sizeof(JS_IDS_Hosts), &tmp, Compare_IDS_Hosts);
	memcpy(p, &tmp, sizeof(JS_IDS_Hosts));
	p->id = id;
	return id;
}
