#ifndef JC_STATISTICERTOP_H_
#define JC_STATISTICERTOP_H_

#include "JC_Hash.h"
#include <ocilib.h>
#include "Common.h"

#define JSCONST_SORTED_BLUCK 100

struct JS_Statisticer {
	int  Key;
	long Packets;
	long Bytes;
	long Count;
	JC_Hash * D_SIP;
	JC_Hash * D_DIP;
	JC_Hash * D_MID;
	JC_Hash * D_DPort;

	unsigned char D_SCountry[1400]; // For 40 * 8  = 320 SRCCountry ID
	unsigned char D_DCountry[1400]; // For 40 * 8  = 320 DSTCountry ID
	unsigned char D_Actions[130];   // For  3 * 8  = 24 Action
	unsigned char D_SesorID[100];   // For 10 * 24 = 80 SensorID


	int 	  ND_Actions;
	int 	  ND_SCountry;
	int 	  ND_DCountry;
	int 	  ND_SensorID;
};

enum JE_Statistic_Type {
	JE_Statistic_Type_MID,
	JE_Statistic_Type_Action,
	JE_Statistic_Type_SensorID,
	JE_Statistic_Type_SIP,
	JE_Statistic_Type_DIP,
	JE_Statistic_Type_SPort,
	JE_Statistic_Type_DPort,
	JE_Statistic_Type_SCountry,
	JE_Statistic_Type_DCountry
};

struct JS_Statistic_DB_Logs {
	JE_Statistic_Type * Type;
	int  * Key;
	big_int  * Packets;
	big_int  * Bytes;
	big_int  * Count;

	int * N_MID;
	int * N_Actions;
	int * N_DPort;
	int * N_SCountry;
	int * N_DCountry;
	int * N_SIP;
	int * N_DIP;
};

class JC_TopTrender {
	public:
		JC_TopTrender();
		bool Start(JI_CgiLogger * );
		void End();
		void Process(JS_DIDS * log);
		JS_Statistic_DB_Logs * GetLists();
		void FreeDBLists();
		unsigned int GetDBLogSize() { return JP_DBLog_Size; }
		static void OnFreeHash(void * p);
	private:
		JC_Hash * JP_Hash_MID;
		JC_Hash * JP_Hash_SIP;
		JC_Hash * JP_Hash_DIP;
		JC_Hash * JP_Hash_DPort;
		JC_Hash * JP_Hash_SCountry;
		JC_Hash * JP_Hash_DCountry;
		JC_Hash * JP_Hash_Action;
		JC_Hash * JP_Hash_SensorID;
		JI_CgiLogger * JP_Logger;
		JS_Statistic_DB_Logs * JP_DBLog;
		unsigned int JP_DBLog_Size;
		void FreeHash(JC_Hash * hash);
		void FreeHashs();
		bool Process_Hash(JS_DIDS * log, int Key, JC_Hash * hash);
};

#endif /* JC_STATISTICER_H_ */
