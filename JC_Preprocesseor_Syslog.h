#ifndef JC_PREPROCESSEOR_UDP_H_
#define JC_PREPROCESSEOR_UDP_H_

#include "Common.h"
#include "JC_Config.h"
#include "JC_DLogger.h"
#include "JC_DIDS.h"
#include "JC_CISCO.h"
#include "JC_CEF.h"
#include "JC_Preprocesseor_CEF_MacAfee.h"
#include "JC_Preprocesseor_CEF_Sophos.h"
#include "JC_Preprocesseor_CEF_Acs.h"
#include "JC_Preprocesseor_CEF_Tripwire.h"
#include "JC_Preprocesseor_CEF_IDS.h"

class JC_Preprocesseor_Syslog: public MI_Preprocesseor {
	public:
		JC_Preprocesseor_Syslog();
		bool Start(JC_Config * Config, JI_CgiLogger * Logger);
		void End();
		JE_Preprocessor_Type GetType() { return JE_Preprocessor_Type_Syslog;};
		bool Process(MS_DataProviderBuffer* inBuf,
			     OUT MS_PreparedDataForLogger* buffer,
			     OUT int & PreparedDataCount);
	private:
		bool Parse_From_CEF_Queue(JI_CEF *, MS_PreparedDataForLogger *);

		char * JP_P[8];
		int JP_Size;
		int *JP_outCount;
		MS_PreparedDataForLogger* JP_OutBuffer;
		std::vector<JI_Preprocesseor_CEF *> CEF_ConverterManager;

		JC_Preprocesseor_CEF_IDS  * BiSHoorBazi;
};

#endif /* JC_PREPROCESSEOR_SYSLOG_H_ */
