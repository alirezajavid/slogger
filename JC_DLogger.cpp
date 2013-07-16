#include "JC_DLogger.h"
//------------------------------------------------------------------------------------------------------
JC_DLogger::JC_DLogger(void)
{
}
//------------------------------------------------------------------------------------------------------
bool JC_DLogger::Start()
{
	return true;
}
//------------------------------------------------------------------------------------------------------
bool JC_DLogger::End()
{
	return true;
}
//------------------------------------------------------------------------------------------------------
char * JC_DLogger::DT()
{
	memset(JP_Now, 0, 20);
	time_t curtime;
	struct tm *loctime;
	curtime = time (NULL);
	loctime = localtime (&curtime);
	strftime (JP_Now, 20, "%Y-%m-%d %H:%M:%S", loctime);
	return JP_Now;
}
//------------------------------------------------------------------------------------------------------
void JC_DLogger::Log(const char * message,...)
{
    va_list arg;

    printf("[%s] ", DT());
    va_start(arg, message);
    vfprintf(stdout, message, arg);
    va_end(arg);
    fflush(stdout);
    fsync(fileno(stdout));
    printf("\n");
	fflush(stdout);
}
//------------------------------------------------------------------------------------------------------
void JC_DLogger::Log(const char *message, const char*  FileName,const char*  FuncName, int Line)
{
	fprintf(stderr, "[%s][%s::%s():%d] %s\n", DT(), basename(FileName),  FuncName  , Line, message);
	fflush(stderr);

}
