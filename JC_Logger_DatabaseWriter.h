#ifndef JC_LOGGER_DATABASEWRITER_H_
#define JC_LOGGER_DATABASEWRITER_H_

#include "Common.h"
#include "JC_Config.h"
#include "JC_DatabseFactory.h"
#include "ocilib.h"
#include "JC_TopTrender.h"
#include "JC_RatesStatistics.h"

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
		void CheckTasks();
		unsigned long JP_LastTime;
		void DeleteOldTxtFiles();
		char JP_NOW[21];
		MS_PreparedDataForLogger* JP_Row;

	private:
		bool prepareQuery_Openports();
		bool prepareQuery_Vulnerability();
		bool prepareQuery_Sophos();
		bool prepareQuery_ACS();
		bool prepareQuery_Cisco();
		bool prepareQuery_DIDS();
		bool prepareQuery_Tripwire();
		bool LogIDSlogToFile();
		bool prepareQuery_DIDS_RateStatistics();
		bool prepareQuery_DIDS_TopStatistics();
		//-----------------------------------------------------------------------------

		MI_Database * JP_db;

		struct {
			OCI_Date   **DT;
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
		} JP_IDS_Vector;
		int JP_IDS_Vector_Index;

		unsigned int JP_RatesStatisticsLastTime;
		unsigned int JP_TopStatisticsLastTime;
		///JC_TopTrender * JP_Trends;
		JC_RatesStatistics * JP_RatesStatistics;
};

#endif /* JC_LOGGER_DATABASEWRITER_H_ */
