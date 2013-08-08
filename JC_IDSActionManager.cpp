#include "JC_IDSActionManager.h"

JC_IDSActionManager::JC_IDSActionManager() {
}
//-----------------------------------------------------------------------------
bool JC_IDSActionManager::Start(JC_Config * config, JI_CgiLogger * logger)
{
	JP_Config = config;
	JP_Logger = logger;

	JP_IDSActions = new JC_Hash();
	if (!JP_IDSActions->Start(20, JP_Logger, false))
	{
		JP_Logger->Log("Could not create Hash List");
		return false;
	}
	LoadLibrary();
	return true;
}
//-----------------------------------------------------------------------------
void JC_IDSActionManager::End()
{
	if (JP_IDSActions)
	{
		JP_IDSActions->End();
		delete JP_IDSActions;
	}
}
//-----------------------------------------------------------------------------
bool Compare_IDS_Alerts(void * p1, void * p2)
{
	JS_IDS_Action *_p1, *_p2;
	_p1 = (JS_IDS_Action *)p1;
	_p2 = (JS_IDS_Action *)p2;
	return (strcmp(_p1->Action, _p2->Action) == 0);
}
void JC_IDSActionManager::LoadLibrary()
{
	MI_Database * db = GetDataBase(JP_Config, JP_Logger, JE_Database_Type_Oracle);
	MI_DataBaseRow * row;
	JS_IDS_Action *p, tmp;
	char KeyStr[20];
	long k;
	db->Query("SELECT id,action FROM dids_actions ");
	while ((row = db->Read()) != NULL)
	{
		memset(KeyStr, 0, sizeof(KeyStr));
		strncpy(KeyStr, row->GetFieldText(2), sizeof(KeyStr) - 1);

		k = JP_IDSActions->Jhash(KeyStr, sizeof(KeyStr), 100);
		strcpy(tmp.Action, row->GetFieldText(2));
		tmp.id = row->GetFieldInt(1);

		p = (JS_IDS_Action *)JP_IDSActions->Get(k, sizeof(JS_IDS_Action), &tmp, Compare_IDS_Alerts);
		memcpy(p, &tmp, sizeof(JS_IDS_Action));
	}
	db->End();
	delete db;
}
//-----------------------------------------------------------------------------
int JC_IDSActionManager::GetAction(const char * action)
{
	if (strcmp(action, "") == 0)
		return 2;

	JS_IDS_Action *p, tmp;
	char KeyStr[20];
	long k;
	memset(KeyStr, 0, sizeof(KeyStr));
	strncpy(KeyStr, action, sizeof(KeyStr) - 1);

	k = JP_IDSActions->Jhash(KeyStr, sizeof(KeyStr), 100);
	strcpy(tmp.Action, action);
	p = (JS_IDS_Action *)JP_IDSActions->Get(k, &tmp, Compare_IDS_Alerts);
	if (p)
		return p->id;
	else
		return AddNewAction(action);
}
//-----------------------------------------------------------------------------
int JC_IDSActionManager::AddNewAction(const char * action)
{
	MI_Database * db = GetDataBase(JP_Config, JP_Logger, JE_Database_Type_Oracle);
	MI_DataBaseRow * row;
	JS_IDS_Action *p, tmp;
	char KeyStr[20];
	long k;
	db->PreperSQL("INSERT INTO dids_actions (action) VALUES (?)");
	db->BindText(0, action);
	if (!db->Execute())
	{
		JP_Logger->Log("Could not Add New Action");
		return 0;
	}
	db->Finalize();

	db->PreperSQL("SELECT id FROM dids_actions WHERE (action = ?) ");
	db->BindText(0, action);
	if (!db->Execute())
	{
		JP_Logger->Log("Could not Add New ACTION");
		return 0;
	}
	row = db->Read();
	int id = row->GetFieldInt(1);
	db->Finalize();

	db->End();
	delete db;
	memset(KeyStr, 0, sizeof(KeyStr));
	strncpy(KeyStr, action, sizeof(KeyStr) - 1);
	k = JP_IDSActions->Jhash(KeyStr, sizeof(KeyStr), 100);
	strcpy(tmp.Action, action);
	tmp.id = id;
	p = (JS_IDS_Action *)JP_IDSActions->Get(k, sizeof(JS_IDS_Action), &tmp, Compare_IDS_Alerts);
	memcpy(p, &tmp, sizeof(JS_IDS_Action));
	return tmp.id;
}
