#include "Common.h"

#ifndef __JC_DLogger_Included__
#define __JC_DLogger_Included__


class JC_DLogger: public JI_CgiLogger
{
	public:
		JC_DLogger(void);
		bool Start();
		bool End();

		void Log(const char* message,...);
		void Log(const char *message, const char*  FileName,const char*  FuncName, int Line);
	private:
		char * DT();
		char JP_Now[20];
};

#endif
