#ifndef JC_DIDS_H_
#define JC_DIDS_H_

#include "Common.h"

class JC_DIDS {
	public:
		JC_DIDS();
		bool Start(const char * str);
		void End();

		int Get_Priority();
		const char * Get_Message();
		int Get_Protocol();
		IP Get_SourceIP();
		IP Get_DestinationIP();
		int Get_SourcePort();
		int Get_DestinationPort();
		int Get_SrcCountry();
		int Get_DstCountry();
		const char * Get_Hostname();
private:
		char JP_String[200];
		char * JP_items[15];
		bool Parse();
		int Split(char * str, char ** result, char ch, int max);
};

#endif /* JC_DIDS_H_ */
