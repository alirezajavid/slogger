/*
 * JC_DSGEngine.h
 *
 *  Created on: Sep 27, 2011
 *      Author: javid
 */

#include "Common.h"
#include "JC_DLogger.h"
#include "JC_DataProvider_PCap.h"
#include "JC_DataProvider_UDP.h"
#include "JC_DataProvider_TextFile.h"
#include "JC_LogManager.h"
#include "JC_Logger_DatabaseWriter.h"
#include "JC_Logger_FileWriter.h"
#include "JC_PreProcessorManager.h"
#include "JC_Preprocesseor_Syslog.h"
#include "JC_Config.h"

#ifndef JC_SOCLoggerENGINE_H_
#define JC_SOCLoggerENGINE_H_

class JC_SOCLoggerEngine: public MI_DataProviderActor
{
	public:
		JC_SOCLoggerEngine();
		bool Start();
		void End();
		void OnReciveData(MS_DataProviderBuffer* buff);
		JC_Config * Config() { return JP_Config;};
		JC_DLogger * Logger() { return JP_Logger;};
	private:
		bool DatabaseTest();
		JC_PreProcessorManager * MP_PreprocessorManager;
		JC_LogManager * MP_LogManager;

		JC_Config * JP_Config;
		JC_DLogger * JP_Logger;

		MI_DataProvider * JP_DataProvider;

};

#endif /* JC_DSGLoggerENGINE_H_ */
