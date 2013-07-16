/*
 * JC_IDSRuleManager.h
 *
 *  Created on: Jul 6, 2013
 *      Author: root
 */

#ifndef JC_IDSRULEMANAGER_H_
#define JC_IDSRULEMANAGER_H_

#include "Common.h"
#include "JC_Config.h"
#include "JC_DatabseFactory.h"
#include "JC_Hash.h"

struct JS_IDS_Messages {
	char Message[200];
	int sid;
	int mid;
} ;

class JC_IDSRuleManager {
	public:
		JC_IDSRuleManager();
		bool Start(JC_Config * config, JI_CgiLogger * logger);
		void End();
		int GetMID(const char * , int sid, const char * module);
		int AddNewMessage(const char * message, int sid, const char * module);
	private:
		void LoadLibrary();
		JC_Config * JP_Config;
		JI_CgiLogger * JP_Logger;
		JC_Hash * JP_IDSMessages;

};

#endif /* JC_IDSRULEMANAGER_H_ */
