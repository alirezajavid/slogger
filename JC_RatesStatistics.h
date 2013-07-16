#ifndef JC_RATESTATISTICER_H_
#define JC_RATESTATISTICER_H_

#include "JC_Hash.h"
#include "Common.h"

struct JS_Rate_Statistic {
	int SensorID;
	int ssize;
	int size;
	int pcount;
	int tcprate;
	int udprate;
	int icmprate;
	int orate;
	int serate;
	int usrate;
	int secount;
	int uscount;
	int tcpratep;
	int udpratep;
	int icmpratep;
	int otherratep;
};

struct JS_Rate_Statistic_DB_Logs {
	int * SensorID;
	int * ssize;
	int * size;
	int * pcount;
	int * tcprate;
	int * udprate;
	int * icmprate;
	int * orate;
	int * serate;
	int * usrate;
	int * secount;
	int * uscount;
	int * tcpratep;
	int * udpratep;
	int * icmpratep;
	int * otherratep;
};


class JC_RatesStatistics {
	public:
		JC_RatesStatistics();
		bool Start(JI_CgiLogger * );
		void End();
		void Process(JS_DIDS * log);
		JS_Rate_Statistic_DB_Logs * GetLists();
		void FreeDBLists();
		unsigned int GetDBLogSize() { return JP_DBLog_Size; }
	private:
		JC_Hash * JP_Hash;
		JI_CgiLogger * JP_Logger;
		JS_Rate_Statistic_DB_Logs * JP_DBLog;
		unsigned int JP_DBLog_Size;
		bool Process_Hash(JS_DIDS * log, int Key, JC_Hash * hash);
};

#endif /* JC_STATISTICER_H_ */
