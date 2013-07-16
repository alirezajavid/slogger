/*
 * MC_Oracle.cpp
 *
 *  Created on: Oct 11, 2011
 *      Author: javid
 *
*/

#include "MC_Oracle.h"

MC_Oracle::MC_Oracle(void)
{
	memset(JP_bind, 0, sizeof(JP_bind));
	JP_Logger = 0;
	if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
	    printf("Could not Initialize ORACLE");
}
//------------------------------------------------------------------------
MC_Oracle::~MC_Oracle(void){
	OCI_Cleanup();
}
//------------------------------------------------------------------------
void MC_Oracle::GetFunction(const char* IN_Functio, char * OUT_Field)
{

}
//------------------------------------------------------------------------
void MC_Oracle::err_handler(OCI_Error *err)
{
    int ErrCode = OCI_ErrorGetOCICode(err);

    switch (ErrCode)
    {
    	case 0		:
    		break;
    	case 21561	:
    		printf("\n\nDear Douran Support\n\tCan`nt connect to Oracle. to fix this problem you should to add the hostname of this machine to /etc/hosts. \n\n");
    		break;
    	default		:
    		printf("code  : ORA-%05i\n\tmsg   : %s\n\tsql   : %s\n", ErrCode, OCI_ErrorGetString(err), OCI_GetSql(OCI_ErrorGetStatement(err)));
    		break;
    }
}
//------------------------------------------------------------------------
bool MC_Oracle::Start(const char* db, const char* user,const char* pwd, JI_CgiLogger * logger)
{
	JP_Logger = logger;
	for (int i=0; i<JCONST_MAX_COLUMN_INTABLE; i++)
	    if (JP_bind[i])
	    	JFreeMem(JP_bind[i], JP_bindSize[i]);
	JP_Connection  = OCI_ConnectionCreate(db, user, pwd, OCI_SESSION_DEFAULT);
	if (!JP_Connection)
		return false;
	return true;
}
//------------------------------------------------------------------------
void MC_Oracle::End()
{
	if (JP_Connection)
		OCI_ConnectionFree(JP_Connection);

}
//------------------------------------------------------------------------
bool MC_Oracle::IsTableExistist(const char* tableName)
{
	return false;
}
//------------------------------------------------------------------------
int MC_Oracle::Query(const char* sql)
{
	int i;
	JP_Statement = OCI_StatementCreate(JP_Connection);
	if (JP_Statement == NULL)
	{
		sprintf(buff, "%s", GetMessage());
		Log(buff, 1);
		return 0;
	}
	OCI_ExecuteStmt(JP_Statement, sql);
	i = OCI_GetAffectedRows(JP_Statement);
	JP_NotFetchedYet = false;
	return i;
}
//------------------------------------------------------------------------
const char* MC_Oracle::GetMessage()
{
	return "";
}
//------------------------------------------------------------------------
MI_DataBaseRow* MC_Oracle::Read()
{
	if (!JP_NotFetchedYet)
	{
		JP_Result = OCI_GetResultset(JP_Statement);
		JP_NotFetchedYet = true;
	}
	return OCI_FetchNext(JP_Result)?this:0;
}
//------------------------------------------------------------------------
bool MC_Oracle::PreperSQL(const char* sql)
{
	char Query[MCONST_MAXCHARBUF];
	char St[2]={0,0};
	char Param[8];
	bool b;
	int j=0;
	JP_NotFetchedYet = false;
	memset(Query, 0, sizeof(Query));
	for(unsigned int i=0; i<strlen(sql); i++)
	{
		if (sql[i] == '?')
		{
			sprintf(Param, ":p%d", j);
			j++;
			strcat(Query, Param);
		}
		else
		{
			St[0]=sql[i];
			strcat(Query, St);
		}
	}
	JP_Statement  = OCI_StatementCreate(JP_Connection);
	b = OCI_Prepare(JP_Statement, Query);
	return b;
}
//------------------------------------------------------------------------
bool MC_Oracle::Finalize()
{
	OCI_Commit(JP_Connection);
	if(JP_Statement)
		OCI_StatementFree(JP_Statement);

	JP_Statement=0;
	for (int i=0; i<JCONST_MAX_COLUMN_INTABLE; i++)
	    if (JP_bind[i])
	    {
			JFreeMem(JP_bind[i], JP_bindSize[i]);
			JP_bind[i] = 0;
	    }
	return true;
}
//------------------------------------------------------------------------
int MC_Oracle::GetFiledCount()
{
	return OCI_GetColumnCount(JP_Result);
}
//------------------------------------------------------------------------
const char* MC_Oracle::GetFieldName(int col)
{
	return OCI_ColumnGetName(OCI_GetColumn(JP_Result, col));
}
//------------------------------------------------------------------------
ME_DataBaseFieldType MC_Oracle::GetFieldType(int col)
{
	return ME_DataBaseFieldType_UnKnown;
}
//------------------------------------------------------------------------
int MC_Oracle::GetFieldInt(int col)
{
	return OCI_GetInt(JP_Result, col);
}
//------------------------------------------------------------------------
const char* MC_Oracle::GetFieldText(int col)
{
	return OCI_GetString(JP_Result, col);
}
//------------------------------------------------------------------------
double MC_Oracle::GetFieldDouble(int col)
{
	return OCI_GetFloat(JP_Result, col);
}
//------------------------------------------------------------------------
long long int MC_Oracle::GetLastRowInsertedId()
{
	return 0;
}
//------------------------------------------------------------------------
bool MC_Oracle::BeginTransaction()
{
	return true;
}
//------------------------------------------------------------------------
bool MC_Oracle::EndTransaction()
{
	return OCI_Commit(JP_Connection);
}
//------------------------------------------------------------------------
bool MC_Oracle::Roollback()
{
	return OCI_Rollback(JP_Connection);
}
//------------------------------------------------------------------------
ME_DataBaseFieldType MC_Oracle::GetFieldType(const char* name)
{
	return ME_DataBaseFieldType_Max;
}
//------------------------------------------------------------------------
int MC_Oracle::GetFieldInt(const char* name)
{
	int index=GetFieldIndex(name);
	return OCI_GetInt(JP_Result, index);
}
//------------------------------------------------------------------------
const char* MC_Oracle::GetFieldText(const char* name)
{
	int index=GetFieldIndex(name);
	return OCI_GetString(JP_Result, index);
}
//------------------------------------------------------------------------
double MC_Oracle::GetFieldDouble(const char* name)
{
	int index=GetFieldIndex(name);
	return OCI_GetDouble(JP_Result, index);
}
//------------------------------------------------------------------------
const void* MC_Oracle::GetFieldBlob(const char* name, int size)
{
	//int index=GetFieldIndex(name);
	return 0;
}
//------------------------------------------------------------------------
bool MC_Oracle::BeginBulkTransaction(const char* sql)
{
	if(!PreperSQL(sql))
	    return false;
	return BeginTransaction();
}
//------------------------------------------------------------------------
bool MC_Oracle::EndBulkTransaction()
{
	EndTransaction();
	Finalize();
	return true;
}
//------------------------------------------------------------------------
bool MC_Oracle::Execute()
{
	if (!OCI_Execute(JP_Statement))
	  return false;
	return true;
}
//------------------------------------------------------------------------
bool MC_Oracle::BindInt(int index,int value)
{
	int * v;
	if (!JP_bind[index])
	{
	    char p[6];
	    JP_bindSize[index] = sizeof(int);
	    sprintf(p, ":p%d", index);
	    v = (int*)JGetMem(JP_bindSize[index]);
	    OCI_BindInt(JP_Statement, p, v);
	    JP_bind[index] = v;
	}
	else
	    v = (int*)JP_bind[index];
	*v = value;
	return true;
}
//------------------------------------------------------------------------
bool MC_Oracle::BindLong(int index,long value)
{
	big_uint * v;
	if (!JP_bind[index])
	{
	    char p[6];
	    sprintf(p, ":p%d", index);
	    JP_bindSize[index] = sizeof(big_uint);
	    v = (big_uint*)JGetMem(JP_bindSize[index]);
	    OCI_BindUnsignedBigInt(JP_Statement, p, v);
	    JP_bind[index] = v;
	}
	else
	    v = (big_uint*)JP_bind[index];
	*v = value;
	return true;
}
//------------------------------------------------------------------------
bool MC_Oracle::BindText(int index,const char* value)
{
	char * v;
	if (!JP_bind[index])
	{
	    char p[6];
	    sprintf(p, ":p%d", index);
	    JP_bindSize[index] = 600;
	    v = (char *)JGetMem(JP_bindSize[index]);
	    OCI_BindString(JP_Statement, p, v, 600);
	    JP_bind[index] = v;
	}
	else
	    v = (char*)JP_bind[index];
	strcpy(v, value);
	return true;
}
//------------------------------------------------------------------------
bool MC_Oracle::BindIP(int index,IP value)
{
	unsigned int * v;
	if (!JP_bind[index])
	{
	    char p[6];
	    JP_bindSize[index] = sizeof(IP);
	    sprintf(p, ":p%d", index);
	    v = (unsigned int*)JGetMem(JP_bindSize[index]);
	    OCI_BindUnsignedInt(JP_Statement, p, v);
	    JP_bind[index] = v;
	}
	else
	    v = (unsigned int*)JP_bind[index];
	*v = value;
	return true;
}
//------------------------------------------------------------------------
IP MC_Oracle::GetFieldIP(int index)
{
	return OCI_GetUnsignedInt(JP_Result, index);
}
//------------------------------------------------------------------------
bool MC_Oracle::BindDateTime(int index,const char* value)
{
	return false;
}
//------------------------------------------------------------------------
bool MC_Oracle::BindBlob(int index,void* data,int size)
{
	return false;
}
//------------------------------------------------------------------------
bool MC_Oracle::BindDouble(int index,double value)
{
	double * v;
	if (!JP_bind[index])
	{
	    char p[6];
	    sprintf(p, ":p%d", index);
	    JP_bindSize[index] = sizeof(double);
	    v = (double*)JGetMem(JP_bindSize[index]);
	    OCI_BindDouble(JP_Statement, p, v);
	    JP_bind[index] = v;
	}
	else
	    v = (double*)JP_bind[index];
	*v = value;
	return true;
}
//------------------------------------------------------------------------
bool MC_Oracle::BindNull(int index)
{
	return false;
}
//-----------------------------------------------------------------------------
unsigned long int MC_Oracle::GetTickCount()
{
	timeb tb;
	ftime( &tb );
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}
//-----------------------------------------------------------------------------
void MC_Oracle::Log(const char * str, int i)
{
    printf("%s\n", str);
}
//------------------------------------------------------------------------
int MC_Oracle::GetFieldIndex(const char* name)
{
	int size=GetFiledCount();
	for(int i=0;i<size;i++)
	{
		if(!strcmp(name,GetFieldName(i)))
			return i;
	}
	return -1;
}
//------------------------------------------------------------------------
const void* MC_Oracle::GetFieldBlob(int col, int size)
{
    return 0;
}
/**/
