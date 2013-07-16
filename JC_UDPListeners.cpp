#include "JC_UDPListeners.h"

JC_UDPListeners::JC_UDPListeners() {
}
//-----------------------------------------------------------------------------
bool JC_UDPListeners::Start(
							pthread_mutex_t * cs_mutex,
							JC_BufferQueue * buffers,
							sem_t * sem,
							unsigned int aPort,
							ME_ProviderLogType processorType,
							MI_DataProvider * DataProvider_UDP)
{
	JP_DataProvider_UDP = DataProvider_UDP;
	JP_Buffers = buffers;
	JP_Mutext = cs_mutex;
	JP_Semaphore = sem;
	JP_ProcessorType = processorType;
	Bind(aPort);
	return true;
}
//-----------------------------------------------------------------------------
void JC_UDPListeners::End()
{
}
//-----------------------------------------------------------------------------
void JC_UDPListeners::Bind(unsigned short int aPort)
{
	long n = 0, h = 0, lt = -1;
	timeb tb;
	unsigned char IN_BUFF[MCONST_PROVIDER_BUFF_SIZE];
	MS_DataProviderBuffer * buf;
	struct stat st;
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
	while (!terminate)
	{
		ftime(&tb);
		if ((tb.millitm + (tb.time & 0xfffff) * 1000) - lt> 1000)
		{
			FILE * fp = fopen("udpstat.txt", "a");
			h += n;
			fprintf(fp, "%lu received in %lu ms  Total:%lu \n", n, (tb.millitm + (tb.time & 0xfffff) * 1000) - lt, h);
			fclose(fp);
			lt = (tb.millitm + (tb.time & 0xfffff) * 1000);
			n = 0;
		};

		memset(IN_BUFF, 0, sizeof(IN_BUFF));
		i = recvfrom(s, IN_BUFF, BUFLEN, 0,(__SOCKADDR_ARG)&si_other, (socklen_t *__restrict)&slen);
		if (i == -1)
			continue;
		n++;

		pthread_mutex_lock(JP_Mutext);
		if(CHECK_PROTGRAM_TERMINATE)
			terminate = true;
		buf = JP_Buffers->Get_Tail();
		if (buf)
		{
			memcpy(buf->buf, IN_BUFF, i);
			buf->DataProviderType = ME_DataProviderType_UDP;
			buf->DeviceIP= htonl(si_other.sin_addr.s_addr);
			buf->LogType = JP_ProcessorType;

			pthread_mutex_unlock(JP_Mutext);
			sem_post(JP_Semaphore);
		}
		else
		{
			pthread_mutex_unlock(JP_Mutext);
			printf("Queue buffer is full or cannt get memory\n");
		}
	}
	printf("Free port[%d].\n", aPort);
    close(s);
}
