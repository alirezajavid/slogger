#ifndef JC_IDSACTIONMANAGER_H_
#define JC_IDSACTIONMANAGER_H_

#include "Common.h"
#include "JC_Config.h"
#include "JC_DatabseFactory.h"
#include "JC_Hash.h"

struct JS_IDS_Action {
	char Action[20];
	int id;
} ;

class JC_IDSActionManager {
	public:
		JC_IDSActionManager();
		bool Start(JC_Config * config, JI_CgiLogger * logger);
		void End();
		int GetAction(const char *);
		int AddNewAction(const char * action);
	private:
		void LoadLibrary();
		JC_Config * JP_Config;
		JI_CgiLogger * JP_Logger;
		JC_Hash * JP_IDSActions;

};

#endif /* JC_IDSRULEMANAGER_H_ */
