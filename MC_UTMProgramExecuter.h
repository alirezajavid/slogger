#include "Common.h"
#ifndef MC_UTMPROGRAMEXECUTER_H_
#define MC_UTMPROGRAMEXECUTER_H_

class MC_UTMProgramExecuter

{
	public:
		MC_UTMProgramExecuter();
		bool Start();
		bool End();
		bool SendSignal(unsigned int id);

		bool GetProgramIsRunning();

		bool ExecuteProgramNoWait(const char* programName, const char* arg);

		bool TerminateProgram();
		int GetPid(){return MP_PId;}
	private://vars
		bool MP_InExecuteMode;
		int MP_ErrorPipes[2];
		int MP_OutPipes[2];
		int MP_PId;
		int MP_Stat;
	private://functins
		bool ExecuteProgram(const char* name,
					const char* arg,
					bool wait,
					char* outBuf,
					int outBufSize,
					char* errBuf,
					int errBufSize,
					bool createThread=true);

		int Split(char* in,char** out);
};

#endif
