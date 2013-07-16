/*
 * MC_MySQL.h
 *
 *  Created on: Oct 11, 2011
 *      Author: javid
 *
*/
#ifndef MC_MYSQL_H_
#define MC_MYSQL_H_

#include <mysql/mysql.h>
#include "Common.h"

class MC_MySQL: public MI_Database {
	public:
		MC_MySQL();
		bool Start(const char* ip, const char* user,const char* pwd,const char* dbName, JI_CgiLogger * logger);
		void End();

		const char* GetMessage();
		long long int GetLastRowInsertedId();
		bool PreperSQL(const char* sql);
		int  Query(const char* buf);
		bool Execute();//bulk execute command

		MI_DataBaseRow * Read();
		bool Finalize();
		bool IsTableExistist(const char* tableName);

		bool BeginTransaction();
		bool EndTransaction();
		bool Roollback();

		bool BindInt(int index,int value);
		bool BindText(int index,const char* value);
		bool BindDateTime(int index,const char* value);
		bool BindBlob(int index,void* data,int size);
		bool BindNull(int index);
		bool BindDouble(int index,double value);
		bool BindIP(int index,IP value);
		bool BindLong(int index,long value);
		bool BeginBulkTransaction(const char* sql);
		bool EndBulkTransaction();

		int 		GetFiledCount();
		const char* GetFieldName(int col);
		int 		GetFieldIndex(const char* name);
		ME_DataBaseFieldType GetFieldType(int col);
		ME_DataBaseFieldType GetFieldType(const char* name);

		int 		GetFieldInt(int col);
		int 		GetFieldInt(const char* name);
		const char* GetFieldText(int col);
		const char* GetFieldText(const char* name);
		double 		GetFieldDouble(int col);
		double 		GetFieldDouble(const char* name);
		const void* GetFieldBlob(int col, int size);
		const void* GetFieldBlob(const char* name, int size);
		const void* GetFieldDateTime(int col, int size){return 0;};
		const void* GetFieldDateTime(const char* name, int size){return 0;};

		void GetFunction(const char* IN_Functio, char * OUT_Field);
		IP 			GetFieldIP(int index){ return 0;};

	private:
		void Log(const char * str);
		unsigned long int GetTickCount();

		JI_CgiLogger * JP_Logger;
		char * JP_BLB;
		char JP_PreParedSQL[MCONST_MAXBUF];

		int JP_param_count;
		MYSQL_BIND    bind_params[MCONST_MINCHARBUF];
		unsigned int JP_num_fields;
		MYSQL * JP_mysql;
		MYSQL_STMT * JP_stmt;
		MYSQL_RES * JP_result;
		MYSQL_ROW JP_row;
		MYSQL_FIELD *JP_fields;

		my_bool    		is_null[MCONST_MINCHARBUF];
		unsigned long 	length[MCONST_MINCHARBUF];
		unsigned long 	values[MCONST_MINCHARBUF];
		MYSQL_TIME 		dt[MCONST_MINCHARBUF];

};

#endif /* MC_MYSQL_H_ */
