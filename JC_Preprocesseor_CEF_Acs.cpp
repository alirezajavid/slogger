#include "JC_Preprocesseor_CEF_Acs.h"

JC_Preprocesseor_CEF_Acs::JC_Preprocesseor_CEF_Acs() {
	// TODO Auto-generated constructor stub

}
//-----------------------------------------------------------------------------
bool JC_Preprocesseor_CEF_Acs::Convert(JI_CEF * cef, MS_PreparedDataForLogger * log)
{
	if (strcmp(cef->DeviceVendor(),   "CISCO"))
		return false;

	log->LogType = JE_Log_Type_ACS;
	strncpy(log->log.ACS.AHost, cef->GetExtensionByName("ahost"), sizeof(log->log.ACS.AHost) - 1);
	strncpy(log->log.ACS.Agt, cef->GetExtensionByName("agt"), sizeof(log->log.ACS.Agt) - 1);
	strncpy(log->log.ACS.AuthorData, cef->GetValueFromLabeledProperty("Author-Data"), sizeof(log->log.ACS.AuthorData) - 1);
	strncpy(log->log.ACS.AuthorFaulureCode, cef->GetValueFromLabeledProperty("Author-Failure-Code"), sizeof(log->log.ACS.AuthorFaulureCode) - 1);
	strncpy(log->log.ACS.Av, cef->GetExtensionByName("av"), sizeof(log->log.ACS.Av) - 1);
	strncpy(log->log.ACS.Cat, cef->GetExtensionByName("cat"), sizeof(log->log.ACS.Cat) - 1);
	strncpy(log->log.ACS.CallerId, cef->GetValueFromLabeledProperty("Caller-ID"), sizeof(log->log.ACS.CallerId) - 1);
	strncpy(log->log.ACS.DUID, cef->GetExtensionByName("duid"), sizeof(log->log.ACS.DUID) - 1);
	strncpy(log->log.ACS.DVC, cef->GetExtensionByName("dvc"), sizeof(log->log.ACS.DVC) - 1);
	strncpy(log->log.ACS.DeviceSeverity, cef->GetExtensionByName("deviceSeverity"), sizeof(log->log.ACS.DeviceSeverity) - 1);
	strncpy(log->log.ACS.DntDom, cef->GetExtensionByName("dntdom"), sizeof(log->log.ACS.DntDom) - 1);
	strncpy(log->log.ACS.Dst, cef->GetExtensionByName("dst"), sizeof(log->log.ACS.Dst) - 1);
	strncpy(log->log.ACS.Msg, cef->GetExtensionByName("msg"), sizeof(log->log.ACS.Msg) - 1);
	strncpy(log->log.ACS.Severity, cef->Severity(), sizeof(log->log.ACS.Severity) - 1);
	strncpy(log->log.ACS.Version, cef->DeviceVersion(), sizeof(log->log.ACS.Version) - 1);
	return true;
}
//-----------------------------------------------------------------------------
