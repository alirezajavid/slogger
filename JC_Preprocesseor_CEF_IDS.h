#ifndef JC_PREPROCESSEOR_CEF_IDS_H_
#define JC_PREPROCESSEOR_CEF_IDS_H_

#include "Common.h"
#include "JC_IDSRuleManager.h"
#include "JC_IDSSensorManager.h"
#include "JC_IDSActionManager.h"

class JC_Preprocesseor_CEF_IDS: public JI_Preprocesseor_CEF {
	public:
		JC_Preprocesseor_CEF_IDS();
		bool Start(JC_Config * config, JI_CgiLogger * logger);
		void End();
		bool Convert(JI_CEF *, MS_PreparedDataForLogger *);
	private:
		JC_IDSRuleManager * JP_RuleManage;
		JC_IDSSensorManager * JP_SensorManage;
		JC_IDSActionManager * JP_ActionManage;
		int SensorID(const char * p, IP ip);
		int ActionID(const char * p);
		int GetModule(const char * module);
		int GetMid(const char * message, int sid, const char * module, int Severity, int anomaly);
		JI_CgiLogger * JP_Logger;
		JC_Config * JP_Config;
};

#endif /* JC_PREPROCESSEOR_CEF_IDS_H_ */
