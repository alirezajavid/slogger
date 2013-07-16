/*
 * JC_IDSSensorManager.h
 *
 *  Created on: Jul 6, 2013
 *      Author: root
 */

#ifndef JC_IDSSENSORMANAGER_H_
#define JC_IDSSENSORMANAGER_H_

#include "Common.h"
#include "JC_Config.h"
#include "JC_DatabseFactory.h"
#include "JC_Hash.h"

struct JS_IDS_Hosts {
	char Hostname[40];
	IP ip;
	int id;
} ;

class JC_IDSSensorManager {
	public:
		JC_IDSSensorManager();
		bool Start(JC_Config * config, JI_CgiLogger * logger);
		void End();
		int GetHostID(const char * host, IP ip);
		int AddNewHost(const char * host, IP ip);
	private:
		void LoadLibrary();
		JC_Config * JP_Config;
		JI_CgiLogger * JP_Logger;
		JC_Hash * JP_Vector;
		union  {
			struct {
				char HostName[40];
				IP  ip;
			} Pair;
			char Key[48];
		} KeyStr;


};

#endif /* JC_IDSRULEMANAGER_H_ */
