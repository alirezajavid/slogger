#include "JC_DataProvider_UDP.h"

JC_DataProvider_UDP::JC_DataProvider_UDP() {
	//JP_cs_mutex = PTHREAD_MUTEX_INITIALIZER;
	memset(&JP_cs_mutex, 0, sizeof(JP_cs_mutex));
}
//-----------------------------------------------------------------------------
bool JC_DataProvider_UDP::Start(MI_DataProviderActor* actor)
{
	MP_Actor=actor;
	JP_Buffers = new JC_BufferQueue();
	if (!JP_Buffers->Start(10000))
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void JC_DataProvider_UDP::End() {
	JP_Buffers->End();
	delete JP_Buffers;
}
//-----------------------------------------------------------------------------
bool JC_DataProvider_UDP::BeginRecive()
{
	pthread_t thread_ids[2] = {0,1};
	void *res;
	if (sem_init(&JP_sem, 0, 0))
		perror("Semaphore init");
	pthread_create(&thread_ids[0], 0, &thread_Consumer, this);
	sleep(1);
	JP_Port = 514;
	JP_ProcessorType = ME_ProviderLogType_Syslog;
	pthread_create(&thread_ids[1], 0, &thread_Producer, this);
	//------------------------------------------------------------------------------
	pthread_join(thread_ids[0], &res);
	JFreeMem(res, sizeof(res));
	res = 0;
	pthread_join(thread_ids[1], &res);
	JFreeMem(res, sizeof(res));
	res = 0;
	sleep(1);
	return true;
}
//-----------------------------------------------------------------------------

