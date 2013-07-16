/*
 * JC_Logger_FileWriter.h
 *
 *  Created on: Sep 27, 2011
 *      Author: javid
 */

#ifndef JC_Logger_FileWriter_H_
#define JC_Logger_FileWriter_H_

#include <unistd.h>
#include "Common.h"
#include "JC_Config.h"
#include "JC_DatabseFactory.h"
#include "MC_UTMProgramExecuter.h"

class JC_Logger_FileWriter: public MI_Logger {
	public:
		JC_Logger_FileWriter();
		bool Log(MS_PreparedDataForLogger* preProcessResult);
		JE_Logger_Type GetType() { return JE_Logger_Type_FileWriter; };
		bool Start(JI_CgiLogger * logger, JC_Config * config);
		void End();
	private:
		JI_CgiLogger * JP_Logger;
		JC_Config* JP_Config;
		const char * Now();
		unsigned long int GetTickCount();
		void FinalScript();
		char JP_NOW[20];
		char JP_DestinationFile[MCONST_MAXCHARBUF];
		FILE * JP_fp;

	private:
		void SetNOW();
		void WriteFile();
};

#endif /* JC_Logger_FileWriter_H_ */
