#include "JC_UDPListeners.h"

JC_UDPListeners::JC_UDPListeners() {
}
//-----------------------------------------------------------------------------
bool JC_UDPListeners::Start(
							pthread_mutex_t * cs_mutex,
							MS_DataProviderBuffer_List *  &buffers,
							sem_t * sem,
							unsigned int aPort,
							ME_ProviderLogType processorType,
							MI_DataProvider * DataProvider_UDP)
{
	JP_DataProvider_UDP = DataProvider_UDP;
	JP_Mutext = cs_mutex;
	JP_Semaphore = sem;
	JP_ProcessorType = processorType;
	Bind(aPort, buffers);
	return true;
}
//-----------------------------------------------------------------------------
void JC_UDPListeners::End()
{
}
//-----------------------------------------------------------------------------
void JC_UDPListeners::Bind(unsigned short int aPort, MS_DataProviderBuffer_List * &Buffers)
{
	long n = 0, h = 0, lt = -1, ft;
	timeb tb;
	unsigned char IN_BUFF[MCONST_PROVIDER_BUFF_SIZE];
	MS_DataProviderBuffer_List * buf;
	struct sockaddr_in si_me, si_other;
	int s, slen=sizeof(si_other);
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		perror("Cannt open socket");
		return ;
	}
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = aPort;
	si_me.sin_port = htons(si_me.sin_port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	struct timeval tv;
	tv.tv_sec = 2;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors

	if (bind(s, (__CONST_SOCKADDR_ARG)&si_me, sizeof(si_me))==-1)
	{
		perror("bind");
	    close(s);
		return ;
	}
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
	printf("Binding successfully.[%d]\n", aPort);
	fflush(stdout);
	bool terminate = false;
	int i=0;
	ftime(&tb);
	lt = tb.millitm + (tb.time & 0xfffff) * 1000;
	ft = lt;
	while (!terminate)
	{
		ftime(&tb);
		if ((tb.millitm + (tb.time & 0xfffff) * 1000) - lt> 1000)
		{
			FILE * fp = fopen("udpstat.txt", "a");
			h += n;
			fprintf(fp, "%lu received in %lu ms  Total:%lu  Avg:%d\n", n, (tb.millitm + (tb.time & 0xfffff) * 1000) - lt, h, (int)((h*1000)/((tb.millitm + (tb.time & 0xfffff) * 1000) - ft)));
			fclose(fp);
			lt = (tb.millitm + (tb.time & 0xfffff) * 1000);
			n = 0;
		};

		memset(IN_BUFF, 0, MCONST_PROVIDER_BUFF_SIZE);
		i = recvfrom(s, IN_BUFF, MCONST_PROVIDER_BUFF_SIZE, 0,(__SOCKADDR_ARG)&si_other, (socklen_t *__restrict)&slen);
		if (i == -1)
		{
			//fprintf(stderr,"receive failed with error %d \n",errno);
			continue;
		}
		n++;

		buf = (MS_DataProviderBuffer_List *)JGetMem(sizeof(MS_DataProviderBuffer_List));
		if(!buf)
		{
			fprintf(stderr, "Queue buffer is full or cannt get memory\n");
			sem_post(JP_Semaphore);
			continue;
		}
		if(CHECK_PROTGRAM_TERMINATE)
			terminate = true;
		memcpy(buf->Node.buf, IN_BUFF, i);
		buf->Node.DataProviderType = ME_DataProviderType_UDP;
		buf->Node.DeviceIP= htonl(si_other.sin_addr.s_addr);
		buf->Node.LogType = JP_ProcessorType;

		pthread_mutex_lock(JP_Mutext);
		buf->Next = Buffers;
		Buffers = buf;
		pthread_mutex_unlock(JP_Mutext);
		sem_post(JP_Semaphore);
	}
	printf("Free port[%d].\n", aPort);
    close(s);
}
