#ifndef CISCO_H_
#define CISCO_H_

#include "Common.h"

class JC_CISCO {
		public:
				JC_CISCO();
				bool Start(const char * str);
				void End();
				const char * GetType()  { return JP_Log + 1; };
				const char * GetMessage()  { return JP_Log_Info; };
		private:
				bool Parse();
				unsigned int JP_LogLength;
				char * JP_Log;
				char * JP_Log_Info;
};

#endif /* CISCO_H_ */
