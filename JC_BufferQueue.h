/*
 * JC_BufferQueue.h
 *
 *  Created on: Jul 22, 2012
 *      Author: root
 */

#ifndef JC_BufferQueue_H_
#define JC_BufferQueue_H_

#include "Common.h"

class JC_BufferQueue {
	public:
		JC_BufferQueue();
		bool Start(int InitSize);
		void End();

		MS_DataProviderBuffer * Get_Tail();
		MS_DataProviderBuffer * Get_Head();
		bool pop();
	private:
		MS_DataProviderBuffer ** JP_Buffers;
		int JP_Head;
		int JP_Tail;
		int JP_Max;

};

#endif /* JC_BufferQueue_H_ */
