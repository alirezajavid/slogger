#ifndef JC_CONFIG_H_
#define JC_CONFIG_H_

#include "Common.h"
#include "MC_IniParser.h"

class JC_Config:public JI_Config
{
	public:
		JC_Config();
		bool Start();
		void End();
		JS_Database_Properties * GetDBConfig() { return &JP_Config; };
		JS_App_Config * GetConfig() { return &JP_App_Config; };
	private:
		JS_Database_Properties JP_Config;
		JS_App_Config JP_App_Config;
	private:
		bool LoadConfig();
		bool LoadDatabseConfig();
};

#endif /* JC_CONFIG_H_ */
