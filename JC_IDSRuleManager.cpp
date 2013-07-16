#include "JC_IDSRuleManager.h"

JC_IDSRuleManager::JC_IDSRuleManager() {
}
//-----------------------------------------------------------------------------
bool JC_IDSRuleManager::Start(JC_Config * config, JI_CgiLogger * logger)
{
	JP_Config = config;
	JP_Logger = logger;

	JP_IDSMessages = new JC_Hash();
	if (!JP_IDSMessages->Start(3000, JP_Logger, false))
	{
		JP_Logger->Log("Could not create Hash List");
		return false;
	}
	LoadLibrary();
	return true;
}
//-----------------------------------------------------------------------------
void JC_IDSRuleManager::End()
{
	if (JP_IDSMessages)
	{
		JP_IDSMessages->End();
		delete JP_IDSMessages;
	}
}
//-----------------------------------------------------------------------------
bool Compare_IDS_Messages(void * p1, void * p2)
{
	JS_IDS_Messages *_p1, *_p2;
	_p1 = (JS_IDS_Messages *)p1;
	_p2 = (JS_IDS_Messages *)p2;
	return (strcmp(_p1->Message, _p2->Message) == 0) && (_p1->sid == _p2->sid);
}
void JC_IDSRuleManager::LoadLibrary()
{
	MI_Database * db = GetDataBase(JP_Config, JP_Logger, JE_Database_Type_Oracle);
	MI_DataBaseRow * row;
	JS_IDS_Messages *p, tmp;
	char KeyStr[200];
	long k;
	db->Query("SELECT distinct Message, sid, mid FROM dids_rules");
	while ((row = db->Read()) != NULL)
	{
		memset(KeyStr, 0, sizeof(KeyStr));
		strncpy(KeyStr, row->GetFieldText(1), sizeof(KeyStr) - 1);

		k = JP_IDSMessages->Jhash(KeyStr, sizeof(KeyStr), 100);
		strcpy(tmp.Message, row->GetFieldText(1));
		tmp.sid = row->GetFieldInt(2);
		tmp.mid = row->GetFieldInt(3);

		p = (JS_IDS_Messages *)JP_IDSMessages->Get(k, sizeof(JS_IDS_Messages), &tmp, Compare_IDS_Messages);
		memcpy(p, &tmp, sizeof(JS_IDS_Messages));
	}
	db->End();
	delete db;
}
//-----------------------------------------------------------------------------
int JC_IDSRuleManager::GetMID(const char * message, int sid, const char * module)
{
	JS_IDS_Messages *p, tmp;
	char KeyStr[200];
	long k;
	memset(KeyStr, 0, sizeof(KeyStr));
	strncpy(KeyStr, message, sizeof(KeyStr) - 1);

	k = JP_IDSMessages->Jhash(KeyStr, sizeof(KeyStr), 100);
	strcpy(tmp.Message, message);
	tmp.sid = sid;
	p = (JS_IDS_Messages *)JP_IDSMessages->Get(k, &tmp, Compare_IDS_Messages);
	if (p)
		return p->mid;
	else
		return AddNewMessage(message, sid, module);
}
//-----------------------------------------------------------------------------
int JC_IDSRuleManager::AddNewMessage(const char * message, int sid, const char * module)
{
	MI_Database * db = GetDataBase(JP_Config, JP_Logger, JE_Database_Type_Oracle);
	MI_DataBaseRow * row;
	JS_IDS_Messages *p, tmp;
	char KeyStr[200];
	long k;
	db->PreperSQL("INSERT INTO dids_rules (Message, sid, mid, MODULE) VALUES (?,?,RULES_MID_SEQ.NEXTVAL,?)");
	db->BindText(0, message);
	db->BindInt(1, sid);
	db->BindText(2, module);
	if (!db->Execute())
	{
		JP_Logger->Log("Could not Add New rule");
		return 0;
	}
	db->Finalize();

	db->PreperSQL("SELECT mid FROM dids_rules WHERE (Message = ?) AND (sid = ?)");
	db->BindText(0, message);
	db->BindInt(1, sid);
	if (!db->Execute())
	{
		JP_Logger->Log("Could not Add New rule");
		return 0;
	}
	row = db->Read();
	int mid = row->GetFieldInt(1);
	db->Finalize();

	db->End();
	delete db;
	memset(KeyStr, 0, sizeof(KeyStr));
	strncpy(KeyStr, message, sizeof(KeyStr) - 1);
	k = JP_IDSMessages->Jhash(KeyStr, sizeof(KeyStr), 100);
	strcpy(tmp.Message, message);
	tmp.sid = sid;
	tmp.mid = mid;
	p = (JS_IDS_Messages *)JP_IDSMessages->Get(k, sizeof(JS_IDS_Messages), &tmp, Compare_IDS_Messages);
	memcpy(p, &tmp, sizeof(JS_IDS_Messages));
	return tmp.mid;
}
