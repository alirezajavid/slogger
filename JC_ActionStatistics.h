#ifndef JC_ACTIONSTATISTICER_H_
#define JC_ACTIONSTATISTICER_H_

#include "JC_Hash.h"
#include "Common.h"

struct JS_Action_Statistic {
	int SensorID;
	int ActionID;
	int Packets;
	int Bytes;
	int Count;
};

struct JS_Action_Statistic_DB_Logs {
	int * SensorID;
	int * ActionID;
	int * Packets;
	int * Bytes;
	int * Count;
};


class JC_ActionsStatistics {
	public:
		JC_ActionsStatistics();
		bool Start(JI_CgiLogger * );
		void End();
		void Process(JS_DIDS * log);
		JS_Action_Statistic_DB_Logs * GetLists();
		void FreeDBLists();
		unsigned int GetDBLogSize() { return JP_DBLog_Size; }
	private:
		JC_Hash * JP_Hash;
		JI_CgiLogger * JP_Logger;
		JS_Action_Statistic_DB_Logs * JP_DBLog;
		unsigned int JP_DBLog_Size;
		bool Process_Hash(JS_DIDS * log, int Key, JC_Hash * hash);
};

#endif /* JC_STATISTICER_H_ */
