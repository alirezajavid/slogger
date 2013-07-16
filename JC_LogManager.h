/*
 * JI_LogManager.h
 *
 *  Created on: Sep 27, 2011
 *      Author: javid
 */

#ifndef JC_LOGMANAGER_H_
#define JC_LOGMANAGER_H_

#include "Common.h"
#include "JC_Logger_DatabaseWriter.h"
#include "JC_Logger_FileWriter.h"

class JC_LogManager: public MI_LogManager
{
	private:
		std::vector<MI_Logger*> JP_Loggers;
	public:
		JC_LogManager();
		bool Start();
		void End();
		void AddLogger(MI_Logger* in);
		bool Log(MS_PreparedDataForLogger*  preProcessResult);
		void ClearProcessBuffer(MS_PreparedDataForLogger*  buffer);
};

#endif /* JC_LOGMANAGER_H_ */
