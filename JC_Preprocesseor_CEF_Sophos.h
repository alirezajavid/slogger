#ifndef JC_PREPROCESSEOR_CEF_Sophos_H_
#define JC_PREPROCESSEOR_CEF_Sophos_H_

#include "Common.h"

class JC_Preprocesseor_CEF_Sophos: public JI_Preprocesseor_CEF {
	public:
		JC_Preprocesseor_CEF_Sophos();
		bool Convert(JI_CEF *, MS_PreparedDataForLogger *);
	private:
		bool Convert_vulnerability(JI_CEF * cef, MS_PreparedDataForLogger * log);
		bool Convert_openport(JI_CEF * cef, MS_PreparedDataForLogger * log);
		void Fill_General(JI_CEF * cef, MS_PreparedDataForLogger * log);

};

#endif /* JC_PREPROCESSEOR_CEF_Sophos_H_ */
