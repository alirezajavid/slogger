/*
 * JC_DataProviderActor_UDP.h
 *
 *  Created on: Sep 28, 2011
 *      Author: javid
 */

#ifndef JC_DATAPROVIDERACTOR_UDP_H_
#define JC_DATAPROVIDERACTOR_UDP_H_

#include "Common.h"
#include "JC_UDPListeners.h"

class JC_DataProvider_UDP: public MI_DataProvider {
	public:
		JC_DataProvider_UDP();
		JE_Provider_Type GetType() { return JE_Provider_Type_UDP;};
		bool Start(MI_DataProviderActor* actor);
		void End();
		bool BeginRecive();
	private:
		MI_DataProviderActor* MP_Actor;
		pthread_mutex_t JP_cs_mutex;
		sem_t JP_sem;

		unsigned int JP_Port;
		ME_ProviderLogType JP_ProcessorType;
		MS_DataProviderBuffer_List * JP_Buffers;




		static void* thread_Consumer(void* data)
		{
		    JC_DataProvider_UDP *_this=(JC_DataProvider_UDP *)data;
		    pthread_mutex_t * mutex = &_this->JP_cs_mutex;
		    MS_DataProviderBuffer_List * buffers, *q;


		    bool terminate = false;
		    while(!terminate)
			{
				sem_wait(&_this->JP_sem);
				pthread_mutex_lock(mutex);
				buffers = _this->JP_Buffers;
				_this->JP_Buffers = 0;
				pthread_mutex_unlock(mutex);

				int o=0;
				while (buffers)
				{
					o++;
					_this->MP_Actor->OnReciveData(&buffers->Node);
					q = buffers;
					buffers = buffers->Next;
					JFreeMem(q, sizeof(MS_DataProviderBuffer_List));
				}
				if (o>1)
					printf("==>%d\n",o);
				if(CHECK_PROTGRAM_TERMINATE)
					terminate = true;
			}
			printf("End of consumers.\n");
			return 0;
		}

		static void* thread_Producer(void* data)
		{
			JC_DataProvider_UDP *_this=(JC_DataProvider_UDP*)data;
			JC_UDPListeners lisetener;

			unsigned int * port = (unsigned int *)JGetMem(sizeof(unsigned int));
			if (!port)
				return 0;

			ME_ProviderLogType * prepType = (ME_ProviderLogType *)JGetMem(sizeof(ME_ProviderLogType));
			if (!prepType)
				return 0;

			*port = _this->JP_Port;
			*prepType = _this->JP_ProcessorType;

			lisetener.Start(
					&_this->JP_cs_mutex,
					_this->JP_Buffers,
					&_this->JP_sem,
					*port,
					*prepType,
					_this);
			lisetener.End();
			JFreeMem(port, sizeof(*port));
			port = 0;
			JFreeMem(prepType, sizeof(*prepType));
			prepType = 0;
			return 0;
		}

};

#endif /* JC_DATAPROVIDERACTOR_UDP_H_ */
