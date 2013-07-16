/*
 * main.cpp
 *
 *  Created on: Sep 27, 2011
 *      Author: javid
 */
#include "ip.h"
#include "Common.h"
#include "JC_SOCLoggerEngine.h"
#include "MC_UTMProgramExecuter.h"

struct sigaction act;

void sighandler(int signum, siginfo_t *info, void *ptr)
{
	printf("Finish.\n");
    FILE * fp = fopen("/var/log/soc_logger/Logger.kill", "w");
    fclose(fp);
}

int main(int argc, char *argv[])
{
	memset(&act, 0, sizeof(act));
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGKILL, &act, NULL);
    unlink("/var/log/soc_logger/Logger.kill");
    printf("pid: %d \n", getpid());
    fflush(stdout);
    if (argc > 1 && !strcmp("-daemon", argv[argc - 1]))
    {
	    pid_t old_pid = getpid();
	    fork();
	    if  (getpid() == old_pid)
		{
		    return false;
		}
	    if (setsid() == -1)
		    return false;

	    MC_UTMProgramExecuter * executer = new MC_UTMProgramExecuter();
	    executer->Start();
	    struct stat st;
	    while (true)
	    {
	    	if (CHECK_PROTGRAM_TERMINATE)
	    		break;
		    if (!executer->GetProgramIsRunning())
		    {
			    executer->ExecuteProgramNoWait("/var/log/soc_logger/soc_logger.bin", "");
			    printf("Logger Running with \n");
			    printf("pid:%d \n", executer->GetPid());
		    }
		    usleep(8000000);
	    }
	    executer->End();
	    delete executer;
    }
    else
    {
    	printf("Start=%lu\n", UsedMemory());
		JC_SOCLoggerEngine engine;
		engine.Start();
		engine.End();
    	printf("End=%lu\n", UsedMemory());
    }
    return 0;
}


