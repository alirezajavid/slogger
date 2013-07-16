#include "MC_MySQL.h"

MC_MySQL::MC_MySQL(void){
	JP_stmt = 0;
}
//------------------------------------------------------------------------
void MC_MySQL::GetFunction(const char* IN_Functio, char * OUT_Field){}
//------------------------------------------------------------------------

bool MC_MySQL::Start(const char* ip, const char* user,const char* pwd,const char* dbName, JI_CgiLogger * logger)
{
	char buff[MCONST_MINCHARBUF];
	mysql_library_init(0, 0, 0);
	JP_Logger = logger;
	JP_mysql = mysql_init(NULL);
	JP_stmt = 0;
	if (!mysql_real_connect(JP_mysql, ip,user, pwd, dbName, 0, NULL, 0))
	{
		sprintf(buff, "%s", GetMessage());
		Log(buff);
		return false;
	}
	return true;
}
//------------------------------------------------------------------------
void MC_MySQL::End()
{
	Finalize();
	mysql_close(JP_mysql);
	mysql_library_end();
}
//------------------------------------------------------------------------
bool MC_MySQL::IsTableExistist(const char* tableName)
{
	return false;
}
//------------------------------------------------------------------------
int MC_MySQL::Query(const char* sql)
{
	int i = mysql_query(JP_mysql, sql);
	if (i)
	{
		Log(GetMessage());
		return false;
	}
	JP_result= mysql_store_result(JP_mysql);
	if (JP_result)  // there are rows
	{
	    JP_fields = mysql_fetch_fields(JP_result);
	    return 0;
	}
	else
	{
	    if(mysql_field_count(JP_mysql) == 0)
	    {
	    	return mysql_affected_rows(JP_mysql);
	    }
	    else
	    {
		    Log(GetMessage());
	    }
	}
	return i;
}
//------------------------------------------------------------------------
const char* MC_MySQL::GetMessage()
{
	return mysql_error(JP_mysql);
}
//------------------------------------------------------------------------
MI_DataBaseRow* MC_MySQL::Read()
{
	JP_row = mysql_fetch_row(JP_result);
	return JP_row?this:0;
}
//------------------------------------------------------------------------
bool MC_MySQL::PreperSQL(const char* sql)
{
	if(JP_stmt)
		mysql_stmt_close(JP_stmt);

	JP_stmt = mysql_stmt_init(JP_mysql);
	if (mysql_stmt_prepare(JP_stmt, sql, strlen(sql)))
	{
		Log(GetMessage());
		return false;
	}
	JP_param_count= mysql_stmt_param_count(JP_stmt);
	memset(bind_params, 0, sizeof(MYSQL_BIND) * MCONST_MINCHARBUF);
	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::Finalize()
{
	if(!JP_stmt)
	    return false;
	mysql_stmt_close(JP_stmt);
	JP_stmt=0;
	return true;
}
//------------------------------------------------------------------------
int MC_MySQL::GetFiledCount()
{
	return mysql_num_fields(JP_result);
}
//------------------------------------------------------------------------
const char* MC_MySQL::GetFieldName(int col)
{
	return JP_fields[col].name;
}
//------------------------------------------------------------------------
ME_DataBaseFieldType MC_MySQL::GetFieldType(int col)
{
	if(!JP_stmt)
		return ME_DataBaseFieldType_UnKnown;
	switch(JP_fields[col].type)
	{
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return ME_DataBaseFieldType_Int;

		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
			return ME_DataBaseFieldType_Double;

		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_YEAR:
			return ME_DataBaseFieldType_DateTime;

		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
			return ME_DataBaseFieldType_Text;

		case MYSQL_TYPE_BLOB:
			return ME_DataBaseFieldType_Blob;
		case MYSQL_TYPE_NULL:
			return ME_DataBaseFieldType_UnKnown;
		default:
			return ME_DataBaseFieldType_UnKnown;
	}
}
//------------------------------------------------------------------------
int MC_MySQL::GetFieldInt(int col)
{
	int val=0;
	val=JP_row[col]?atoi(JP_row[col]):0;
	return val;
}
//------------------------------------------------------------------------
const char* MC_MySQL::GetFieldText(int col)
{
	return JP_row[col]?JP_row[col]:"";
}
//------------------------------------------------------------------------
double MC_MySQL::GetFieldDouble(int col)
{
	return JP_row[col]?atof(JP_row[col]):0;
}
//------------------------------------------------------------------------
long long int MC_MySQL::GetLastRowInsertedId()
{
	return mysql_stmt_insert_id(JP_stmt);
}
//------------------------------------------------------------------------
bool MC_MySQL::BeginTransaction()
{
	return Query("START TRANSACTION;");
}
//------------------------------------------------------------------------
bool MC_MySQL::EndTransaction()
{
	char i = mysql_commit(JP_mysql);
	if (i)
	{
	    Log(GetMessage());
	    return false;
	}
	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::Roollback()
{
	return mysql_rollback(JP_mysql);
}
//------------------------------------------------------------------------
ME_DataBaseFieldType MC_MySQL::GetFieldType(const char* name)
{
	int index=GetFieldIndex(name);
	return GetFieldType(index);
}
//------------------------------------------------------------------------
int MC_MySQL::GetFieldInt(const char* name)
{
	int index=GetFieldIndex(name);
	return GetFieldInt(index);
}
//------------------------------------------------------------------------
const char* MC_MySQL::GetFieldText(const char* name)
{
	int index=GetFieldIndex(name);
	return GetFieldText(index);
}
//------------------------------------------------------------------------
double MC_MySQL::GetFieldDouble(const char* name)
{
	int index=GetFieldIndex(name);
	return GetFieldDouble(index);
}
//------------------------------------------------------------------------
const void* MC_MySQL::GetFieldBlob(const char* name, int size)
{
	int index=GetFieldIndex(name);
	return GetFieldBlob(index, size);
}
//------------------------------------------------------------------------
bool MC_MySQL::BeginBulkTransaction(const char* sql)
{
	if(!PreperSQL(sql))
	    return false;
	return (void*)JP_stmt;
}
//------------------------------------------------------------------------
bool MC_MySQL::EndBulkTransaction()
{
	Finalize();
	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::Execute()
{
	if (mysql_stmt_bind_param(JP_stmt, bind_params))
	{
	  Log(" mysql_stmt_bind_param() failed");
	  Log(mysql_stmt_error(JP_stmt));
	  return false;
	}
	int i = mysql_stmt_execute(JP_stmt);
	if (i)
		  Log(mysql_stmt_error(JP_stmt));
	return (i==0);
}
//------------------------------------------------------------------------
bool MC_MySQL::BindInt(int index,int value)
{
	values[index] = value;
	bind_params[index].buffer_type= MYSQL_TYPE_LONG;
	bind_params[index].buffer= (char *)&values[index];
	bind_params[index].is_null= 0;
	bind_params[index].length= 0;
	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::BindText(int index,const char* value)
{
	length[index] = strlen(value);
	bind_params[index].buffer_type= MYSQL_TYPE_STRING;
	bind_params[index].buffer= (char *)value;
	bind_params[index].buffer_length= MCONST_MAXCHARBUF;
	bind_params[index].is_null= 0;
	bind_params[index].length= length + index;
	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::BindDateTime(int index,const char* value)
{
	char v[MCONST_MINCHARBUF];
	strcpy(v, value);
	v[4] = v[7] = v[10] = v[13] = v[16] = 0;
	dt[index].year = atoi(v);
	dt[index].month = atoi(v+5);
	dt[index].day = atoi(v+8);
	dt[index].hour = atoi(v+11);
	dt[index].minute = atoi(v+14);
	dt[index].second = atoi(v+17);

	bind_params[index].buffer_type= MYSQL_TYPE_DATETIME;
	bind_params[index].buffer= (char *)&dt[index];
	bind_params[index].is_null= 0;
	bind_params[index].length= 0;
	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::BindIP(int index,IP value)
{
	values[index] = value;
	bind_params[index].buffer_type= MYSQL_TYPE_LONG;
	bind_params[index].buffer= (char *)&values[index];
	bind_params[index].is_null= 0;
	bind_params[index].length= 0;
	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::BindBlob(int index,void* data,int size)
{
	char chunk[2*1000*1024+1];
	mysql_real_escape_string(JP_mysql, chunk, (char *)data, size);
	length[index] = size;
	bind_params[index].buffer_type= MYSQL_TYPE_BLOB;
	bind_params[index].buffer= (char *)chunk;
	bind_params[index].buffer_length= MCONST_MAXCHARBUF;
	bind_params[index].is_null= 0;
	bind_params[index].length= length + index;
	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::BindDouble(int index,double value)
{
	bind_params[index].buffer_type= MYSQL_TYPE_DOUBLE;
	bind_params[index].buffer= (char *)&value;
	bind_params[index].is_null= 0;
	bind_params[index].length= 0;
   	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::BindLong(int index,long value)
{
	bind_params[index].buffer_type= MYSQL_TYPE_LONG;
	bind_params[index].buffer= (char *)&value;
	bind_params[index].is_null= 0;
	bind_params[index].length= 0;
   	return true;
}
//------------------------------------------------------------------------
bool MC_MySQL::BindNull(int index)
{
	bind_params[index].buffer_type= MYSQL_TYPE_NULL;
	bind_params[index].is_null=  is_null + index;
	bind_params[index].length= 0;
   	return true;
}
//-----------------------------------------------------------------------------
unsigned long int MC_MySQL::GetTickCount()
{
	timeb tb;
	ftime( &tb );
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}
//-----------------------------------------------------------------------------
void MC_MySQL::Log(const char * str)
{
	if (JP_Logger)
		JP_Logger->Log(str);
}
//------------------------------------------------------------------------
int MC_MySQL::GetFieldIndex(const char* name)
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
const void* MC_MySQL::GetFieldBlob(int col, int size)
{
	if(!JP_stmt)
		return 0;
	unsigned long *lengths = mysql_fetch_lengths(JP_result);
	size = lengths[col];
	return JP_row[col];
}
/**/
