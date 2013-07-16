/*
 * JC_PreProcessorManager.h
 *
 *  Created on: Sep 27, 2011
 *      Author: javid
 */

#ifndef JC_PREPROCESSORMANAGER_H_
#define JC_PREPROCESSORMANAGER_H_

#include "Common.h"
#include "JC_Preprocesseor_Syslog.h"

class JC_PreProcessorManager {
	public:
		JC_PreProcessorManager();
		bool Start();
		void End();
		void AddPreprocesor(MI_Preprocesseor* in);
		bool Preprocess(MS_DataProviderBuffer* inBuf, OUT MS_PreparedDataForLogger* buffer, OUT int & count);

	private:
		std::vector<MI_Preprocesseor*> JP_Preprocesseors;

};

#endif /* JC_PREPROCESSORMANAGER_H_ */
