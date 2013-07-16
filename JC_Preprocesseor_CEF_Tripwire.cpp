#include "JC_Preprocesseor_CEF_Tripwire.h"

JC_Preprocesseor_CEF_Tripwire::JC_Preprocesseor_CEF_Tripwire() {
	// TODO Auto-generated constructor stub

}
//-----------------------------------------------------------------------------
bool JC_Preprocesseor_CEF_Tripwire::Convert(JI_CEF * cef, MS_PreparedDataForLogger * log)
{
	if (strcmp(cef->DeviceVendor(),   "Tripwire"))
		return false;

	log->LogType = JE_Log_Type_Tripwire;
	strncpy(log->log.Tripwire.Rulename, cef->GetValueFromLabeledProperty("Rule Name"),sizeof(log->log.Tripwire.Rulename) - 1);
	strncpy(log->log.Tripwire.Object, cef->GetValueFromLabeledProperty("Object Name"),sizeof(log->log.Tripwire.Object) - 1);
	strncpy(log->log.Tripwire.dHost, cef->GetExtensionByName("dhost"),sizeof(log->log.Tripwire.dHost) - 1);
	strncpy(log->log.Tripwire.dst, cef->GetExtensionByName("dst"),sizeof(log->log.Tripwire.dst) - 1);
	strncpy(log->log.Tripwire.AHost, cef->GetExtensionByName("ahost"),sizeof(log->log.Tripwire.AHost) - 1);
	strncpy(log->log.Tripwire.Agt, cef->GetExtensionByName("agt"),sizeof(log->log.Tripwire.Agt) - 1);
	strncpy(log->log.Tripwire.Av, cef->GetExtensionByName("av"),sizeof(log->log.Tripwire.Av) - 1);
	strncpy(log->log.Tripwire.Message, cef->GetExtensionByName("msg"),sizeof(log->log.Tripwire.Message) - 1);
	strncpy(log->log.Tripwire.dvcHost, cef->GetExtensionByName("dvchost"),sizeof(log->log.Tripwire.dvcHost) - 1);
	strncpy(log->log.Tripwire.dvc, cef->GetExtensionByName("dvc"),sizeof(log->log.Tripwire.dvc) - 1);
	strncpy(log->log.Tripwire.sHost, cef->GetExtensionByName("shost"),sizeof(log->log.Tripwire.sHost) - 1);
	strncpy(log->log.Tripwire.sUser, cef->GetExtensionByName("suser"),sizeof(log->log.Tripwire.sUser) - 1);
	strncpy(log->log.Tripwire.src, cef->GetExtensionByName("src"),sizeof(log->log.Tripwire.src) - 1);
	strncpy(log->log.Tripwire.deviceSeverity, cef->GetExtensionByName("deviceSeverity"),sizeof(log->log.Tripwire.deviceSeverity) - 1);
	strncpy(log->log.Tripwire.act, cef->GetExtensionByName("act"),sizeof(log->log.Tripwire.act) - 1);
	strncpy(log->log.Tripwire.cat, cef->GetExtensionByName("cat"),sizeof(log->log.Tripwire.cat) - 1);
	return true;
}
//-----------------------------------------------------------------------------
