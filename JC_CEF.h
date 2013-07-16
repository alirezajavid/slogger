#ifndef CEF_H_
#define CEF_H_

#include "Common.h"

struct JP_CEFExtentionPair {
	char Name[50];
	int SizeOfValue;
	char * Value;
};

class JC_CEF: public JI_CEF {
		public:
				JC_CEF();
				bool Start(const char * str);
				void End();
				int GetVersion();
				const char * DeviceVendor();
				const char * DeviceProduct();
				const char * DeviceVersion();
				const char * SignatureID();
				const char * Name();
				const char * Severity();
				const char * GetExtensionByName(const char * name);
				const char * GetExtensionKeyByValue(const char * value);
				const char * GetValueFromLabeledProperty(const char * value);
		private:
				std::vector<JP_CEFExtentionPair *> JP_Extension;
				bool Parse();
				char * JP_Log;
				unsigned int JP_LogLength;
				char * JP_CEFItems[8];
};

#endif /* CEF_H_ */
