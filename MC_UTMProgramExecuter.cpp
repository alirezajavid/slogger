 #include "MC_UTMProgramExecuter.h"
//-----------------------------------------------------------------------------
MC_UTMProgramExecuter::MC_UTMProgramExecuter()
{
	MP_InExecuteMode=false;
	MP_OutPipes[0]=0;
	MP_OutPipes[1]=0;
	MP_ErrorPipes[0]=0;
	MP_ErrorPipes[1]=0;
	MP_PId=-1;
}
//-----------------------------------------------------------------------------
bool MC_UTMProgramExecuter::Start()
{
	MP_InExecuteMode=false;
	return true;
}
//-----------------------------------------------------------------------------
bool MC_UTMProgramExecuter::End()
{
	TerminateProgram();
	return true;
}
//-----------------------------------------------------------------------------
bool MC_UTMProgramExecuter::SendSignal(unsigned int id)
    {
	if(!MP_InExecuteMode)
	    return false;

	kill(MP_PId,id);
	return true;
    }
//-----------------------------------------------------------------------------
bool MC_UTMProgramExecuter::TerminateProgram()
    {
	if(MP_PId!=-1)
	    kill(MP_PId,SIGKILL);
	waitpid(-1,&MP_Stat,WNOHANG);
    	MP_InExecuteMode=false;
    	if(MP_OutPipes[0])
    	    {
		close(MP_OutPipes[0]);
		MP_OutPipes[0]=0;
    	    }

    	if(MP_ErrorPipes[0])
    	    {
		close(MP_ErrorPipes[0]);
		MP_ErrorPipes[0]=0;
    	    }
    	return true;
    }
//-----------------------------------------------------------------------------
bool MC_UTMProgramExecuter::ExecuteProgramNoWait(const char* programName,
						 const char* arg)
{
	return ExecuteProgram(programName,arg,false,NULL,0,NULL,0,false);
}
//-----------------------------------------------------------------------------
bool MC_UTMProgramExecuter::ExecuteProgram(const char* name,
					    const char* arg,
					    bool wait,
					    char* outBuf,
					    int outBufSize,
					    char* errBuf,
					    int errBufSize,
					    bool createThread)
{
	 char* argv[100]={0};
	 int size = strlen(arg)+strlen(name)+2;
	 char* temp=(char*)JGetMem(size);
	 if(!temp)
		 return false;
	 sprintf(temp,"%s %s",name,arg);
	 Split(temp,argv);
	 if(wait)
	 {
		 if(pipe(MP_OutPipes)==-1)
		     return false;
		 if(pipe(MP_ErrorPipes)==-1)
		     return false;
	 }
	 waitpid(-1,&MP_Stat,WNOHANG);
	 int oldPid=getpid();

	  MP_PId=fork();
	  MP_InExecuteMode=true;
	 //parent proc
	  if(oldPid!=getpid())
	  {
	      setsid();
	      if(wait)
		  {
		      close(MP_OutPipes[0]);
		      close(MP_ErrorPipes[0]);
		      dup2(MP_OutPipes[1],STDOUT_FILENO);
		      dup2(MP_ErrorPipes[1],STDERR_FILENO);
		  }
	      int stdoutFile=open("/var/log/soc_logger/stdout.txt",O_WRONLY|O_APPEND|O_CREAT);
	      int stderrFile=open("/var/log/soc_logger/stderr.txt",O_WRONLY|O_APPEND|O_CREAT);
	      dup2(stdoutFile,STDOUT_FILENO);
	      dup2(stderrFile,STDERR_FILENO);

	      for(int i=3;i<255;i++)
		  {
		      close(i);
		  }
	      if(execv(name,argv)==-1)
	      {
	         fprintf(stderr,"can not execute program %s with error %s\n",name,strerror(errno));
	         exit(0);
	      }
	  }
	  else
	  {
		if(wait)
		   {
			close(MP_OutPipes[1]);
			close(MP_ErrorPipes[1]);
			if(outBuf)
			    read(MP_OutPipes[0],outBuf,outBufSize);
			if(errBuf)
				read(MP_ErrorPipes[0],errBuf,errBufSize);
			waitpid(MP_PId,&MP_Stat,0);
			close(MP_OutPipes[0]);
			close(MP_ErrorPipes[0]);
			MP_InExecuteMode=false;
		  }
	  }
	  JFreeMem(temp, size);
	  temp = 0;
	  return true;
}
//-----------------------------------------------------------------------------
int MC_UTMProgramExecuter::Split(char* in,char** out)
{
	char *end;
	char *start=(char*)in;
	int size=0;
	for(size=0;;size++)
	{
		end=strstr(start," ");
		if(!end)
		{
			out[size]=start;
			break;
		}
		else
		{
			end[0]='\0';
			out[size]=start;
			start=end+1;
			if(start[0]=='\'')
			    {
				out[++size]=start;
				break;
			    }
		}
	}
	out[size+1]=0;
	return size;
}
//----------------------------------------------------------------------------------
bool MC_UTMProgramExecuter::GetProgramIsRunning()
    {
	struct stat s;
	FILE* f;
	char readBuf[500]={0};
	char buf[50]={0};

	sprintf(buf,"/proc/%d",MP_PId);
	if(stat(buf,&s)<0)
	    return false;

	sprintf(buf,"/proc/%d/status",MP_PId);
	f=fopen(buf,"r");
	if(!f)
	    return false;

	//read name
	if(fgets(readBuf,500,f)<0)
	    {
		fclose(f);
		return false;
	    }

	if(fgets(readBuf,500,f)<0)
	    {
		fclose(f);
		return false;
	    }
	fclose(f);
	//if state is in zombie then return false;

	if(strstr(readBuf,"Z (zombie)"))
	    return false;

	return true;
    }
//----------------------------------------------------------------------------------

