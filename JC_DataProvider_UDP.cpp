#include "JC_DataProvider_UDP.h"

JC_DataProvider_UDP::JC_DataProvider_UDP() {
	memset(&JP_cs_mutex, 0, sizeof(JP_cs_mutex));
	//JP_cs_mutex = PTHREAD_MUTEX_INITIALIZER;
}
//-----------------------------------------------------------------------------
bool JC_DataProvider_UDP::Start(MI_DataProviderActor* actor)
{
	JP_Buffers = 0;
	MP_Actor=actor;
	return true;
}
//-----------------------------------------------------------------------------
void JC_DataProvider_UDP::End() {
}
//-----------------------------------------------------------------------------
bool JC_DataProvider_UDP::BeginRecive()
{
	pthread_t thread_ids[2] = {0,1};
	void *res;
	if (sem_init(&JP_sem, 0, 0))
		perror("Semaphore init");
	pthread_create(&thread_ids[0], 0, &thread_Consumer, this);
	JP_Port = 514;
	JP_ProcessorType = ME_ProviderLogType_Syslog;
	pthread_create(&thread_ids[1], 0, &thread_Producer, this);
	//------------------------------------------------------------------------------

	pthread_join(thread_ids[0], &res);
	pthread_join(thread_ids[1], &res);
	return true;
}
//-----------------------------------------------------------------------------

