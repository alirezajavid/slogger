/*
 * MC_Oracle.h
 *
 *  Created on: Oct 11, 2011
 *      Author: javid
 *
*/
#ifndef MC_Oracle_H_
#define MC_Oracle_H_

#include <ocilib.h>
#include "Common.h"
#define JCONST_MAX_COLUMN_INTABLE 30
class MC_Oracle : public MI_Database{
	public:
		MC_Oracle();
		~MC_Oracle();
		bool Start(const char* db, const char* user,const char* pwd, JI_CgiLogger * logger);
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

		int 			GetFieldInt(int col);
		int 			GetFieldInt(const char* name);
		const char* 	GetFieldText(int col);
		const char* 	GetFieldText(const char* name);
		double 			GetFieldDouble(int col);
		double 			GetFieldDouble(const char* name);
		const void* 	GetFieldBlob(int col, int size);
		const void* 	GetFieldBlob(const char* name, int size);
		const void* 	GetFieldDateTime(int col, int size){return 0;};
		const void* 	GetFieldDateTime(const char* name, int size){return 0;};
		IP 				GetFieldIP(int index);
		void GetFunction(const char* IN_Functio, char * OUT_Field);
		static void err_handler(OCI_Error *err);
		OCI_Statement * Get_Statement() {return JP_Statement; };

	private:
		void Log(const char * str, int i);
		unsigned long int GetTickCount();

		char buff[MCONST_MAXCHARBUF];
		char JP_Esrr[MCONST_MAXCHARBUF];
		bool JP_NotFetchedYet;

		OCI_Connection *JP_Connection;
		OCI_Statement  *JP_Statement;
		OCI_Resultset  *JP_Result;
		OCI_Transaction *JP_Transaction;

		void * JP_bind[JCONST_MAX_COLUMN_INTABLE];
		int    JP_bindSize[JCONST_MAX_COLUMN_INTABLE];

		JI_CgiLogger * JP_Logger;
};

#endif /* MC_Oracle_H_ */
