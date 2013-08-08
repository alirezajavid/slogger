#ifndef JC_LOGGER_DATABASEWRITER_H_
#define JC_LOGGER_DATABASEWRITER_H_

#include "Common.h"
#include "JC_Config.h"
#include "JC_DatabseFactory.h"
#include "ocilib.h"
#include "JC_RatesStatistics.h"
#include "JC_ActionStatistics.h"


struct JS_IDS_Vector {
	OCI_Date **DT;
	int *MID,
		*SENSORID,
		*PROTOCOL,
		*SIP,
		*DIP,
		*SPORT,
		*DPORT,
		*SCOUNTRY,
		*DCOUNTRY,
		*ACTIONID,
		*PACKET,
		*DIRECTION,
		*BYTE;
};

class JC_Logger_DatabaseWriter: public MI_Logger {
	public:
		JC_Logger_DatabaseWriter();
		bool Log(MS_PreparedDataForLogger* preProcessResult);
		JE_Logger_Type GetType() { return JE_Logger_Type_DatabaseWriter; };
		bool Start(JI_CgiLogger * logger, JC_Config * config);
		void End();
	private:
		JI_CgiLogger * JP_Logger;
		JC_Config* JP_Config;
		const char * SetNow(bool b);
		int GetCurrentHour();
		void CheckTasks();
		unsigned long JP_LastTime;
		void DeleteOldTxtFiles();
		char JP_NOW[21];
		MS_PreparedDataForLogger* JP_Row;

	private:
		void Create_IDS_Vector(JS_IDS_Vector * vector);
		void Clear_IDS_Vector(JS_IDS_Vector * vector);
		JS_IDS_Vector * Dump_DIDSVector();
		bool prepareQuery_Openports();
		bool prepareQuery_Vulnerability();
		bool prepareQuery_Sophos();
		bool prepareQuery_ACS();
		bool prepareQuery_Cisco();
		bool prepareQuery_DIDS();
		bool prepareQuery_Tripwire();
		bool LogIDSlogToFile();
		bool prepareQuery_DIDS_RateStatistics();
		bool prepareQuery_DIDS_RateActionStatistics();
		//-----------------------------------------------------------------------------
		void FastbitLogger();
		void OracleLogger();

		MI_Database * JP_db;
		JS_IDS_Vector * JP_IDS_Vector_Cach;
		JS_IDS_Vector JP_IDS_Vector;
	    char JP_FastbitStorageDirectory[100];
	    int JP_LastHour;

		int JP_IDS_Vector_Index;

		unsigned int JP_RatesStatisticsLastTime;
		unsigned int JP_ActionStatisticsLastTime;
		JC_RatesStatistics * JP_RatesStatistics;
		JC_ActionsStatistics * JP_RateActionStatistics;

		std::auto_ptr<ibis::tablex> ta;
		ibis::table::row nr;

/*
		static void* thread_Write_DIDS_TO_DB(void* data)
		{
			OCI_Statement	* st;
			OCI_Error		* err;
			JS_IDS_Vector *   vector;
			timeb tb;

			JC_Logger_DatabaseWriter *_this=(JC_Logger_DatabaseWriter*)data;
			MI_Database * db = GetDataBase(_this->JP_Config, _this->JP_Logger, JE_Database_Type_Oracle);

			vector = _this->JP_IDS_Vector_Cach;
			int size = _this->JP_Config->GetConfig()->Logger.BulkSize;
			_this->JP_IDS_Vector_Cach = 0;

			ftime(&tb);
			long t_start = tb.millitm + (tb.time & 0xfffff) * 1000;
			if (!db->PreperSQL("INSERT INTO dids_ATTACKS(DT,MID,SENSORID,PROTOCOL,SIP,DIP,SPORT,DPORT,SCOUNTRY,DCOUNTRY,ACTIONID,PACKET,BYTE,DIRECTION) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)"))
			{
				printf("Could not Prepare Statement\n");
				return 0;
			}

			st = ((MC_Oracle *)db)->Get_Statement();
			OCI_BindArraySetSize(st, size);

			OCI_BindArrayOfDates  (st, ":p0", (OCI_Date**)				vector->DT, 0);
			OCI_BindArrayOfInts   (st, ":p1", (int*)  					vector->MID, 0);
			OCI_BindArrayOfInts   (st, ":p2", (int*)  					vector->SENSORID, 0);
			OCI_BindArrayOfInts   (st, ":p3", (int*)  					vector->PROTOCOL, 0);
			OCI_BindArrayOfUnsignedInts   (st, ":p4", (unsigned int*)  	vector->SIP, 0);
			OCI_BindArrayOfUnsignedInts   (st, ":p5", (unsigned int*)  	vector->DIP, 0);
			OCI_BindArrayOfInts   (st, ":p6", (int*)  					vector->SPORT, 0);
			OCI_BindArrayOfInts   (st, ":p7", (int*)  					vector->DPORT, 0);
			OCI_BindArrayOfInts   (st, ":p8", (int*)  					vector->SCOUNTRY, 0);
			OCI_BindArrayOfInts   (st, ":p9", (int*)  					vector->DCOUNTRY, 0);
			OCI_BindArrayOfInts   (st, ":p10",(int*)  					vector->ACTIONID, 0);
			OCI_BindArrayOfInts   (st, ":p11",(int*)  					vector->PACKET, 0);
			OCI_BindArrayOfInts   (st, ":p12",(int*)  					vector->BYTE, 0);
			OCI_BindArrayOfInts   (st, ":p13",(int*)  					vector->DIRECTION, 0);

			if (!OCI_Execute(st))
			{
			    printf("Number of DML array errors : %d\n", OCI_GetBatchErrorCount(st));
				err = OCI_GetBatchError(st);
				while (err)
				{
				    printf("Error at row %d : %s\n", OCI_ErrorGetRow(err), OCI_ErrorGetString(err));
					err = OCI_GetBatchError(st);
				}
			}
			db->Finalize();
			ftime(&tb);
			long t_end = tb.millitm + (tb.time & 0xfffff) * 1000;
		    printf("logging %d records in %lu ms.\n", size, t_end - t_start);

			free(vector->DT);
			free(vector->MID);
			free(vector->SENSORID);
			free(vector->PROTOCOL);
			free(vector->SIP);
			free(vector->DIP);
			free(vector->SPORT);
			free(vector->DPORT);
			free(vector->SCOUNTRY);
			free(vector->DCOUNTRY);
			free(vector->ACTIONID);
			free(vector->PACKET);
			free(vector->BYTE);
			free(vector->DIRECTION);
			return 0;
		}
*/
};

#endif /* JC_LOGGER_DATABASEWRITER_H_ */
