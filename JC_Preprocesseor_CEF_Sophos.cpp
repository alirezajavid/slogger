#include "JC_Preprocesseor_CEF_Sophos.h"

JC_Preprocesseor_CEF_Sophos::JC_Preprocesseor_CEF_Sophos() {
	// TODO Auto-generated constructor stub

}
//-----------------------------------------------------------------------------
bool JC_Preprocesseor_CEF_Sophos::Convert(JI_CEF * cef, MS_PreparedDataForLogger * log)
{
	if (strcmp(cef->DeviceVendor(),   "Sophos"))
		return false;

	log->LogType = JE_Log_Type_Sophos;
	strncpy(log->log.sophos.Version, cef->DeviceVersion(), sizeof(log->log.sophos.Version) - 1);
	strncpy(log->log.sophos.Name, cef->Name(), sizeof(log->log.sophos.Name) - 1);
	strncpy(log->log.sophos.Severity, cef->Severity(), sizeof(log->log.sophos.Severity) - 1);
	strncpy(log->log.sophos.dHost, cef->GetExtensionByName("dhost"), sizeof(log->log.sophos.dHost) - 1);
	strncpy(log->log.sophos.Domain, cef->GetExtensionByName("destinationDnsDomain"), sizeof(log->log.sophos.Domain) - 1);
	strncpy(log->log.sophos.FilePath, cef->GetExtensionByName("filePath"), sizeof(log->log.sophos.FilePath) - 1);
	return true;
}
//-----------------------------------------------------------------------------
