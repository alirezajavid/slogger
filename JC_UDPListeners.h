#ifndef JC_UDPLISTENERS_H_
#define JC_UDPLISTENERS_H_

#include "Common.h"
#include "JC_BufferQueue.h"

class JC_UDPListeners
{
	public:
		JC_UDPListeners();
		bool Start(
				pthread_mutex_t * cs_mutex,
				JC_BufferQueue * buffers,
				sem_t * sem,
				unsigned int aPort,
				ME_ProviderLogType processorType,
				MI_DataProvider * DataProvider_UDP
		);
		void End();
	private:
		MI_DataProvider * JP_DataProvider_UDP;
		pthread_mutex_t * JP_Mutext;
		sem_t * JP_Semaphore;
		JC_BufferQueue * JP_Buffers;
		ME_ProviderLogType JP_ProcessorType;
		void Bind(unsigned short int aPort);

};

#endif
