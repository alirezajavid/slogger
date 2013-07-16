#include "JC_Preprocesseor_CEF_MacAfee.h"

JC_Preprocesseor_CEF_MacAfee::JC_Preprocesseor_CEF_MacAfee() {
	// TODO Auto-generated constructor stub

}
//-----------------------------------------------------------------------------
bool JC_Preprocesseor_CEF_MacAfee::Convert(JI_CEF * cef, MS_PreparedDataForLogger * log)
{
	if (strcmp(cef->DeviceVendor(),   "McAfee"))
		return false;

	const char * categoryTechnique = cef->GetExtensionByName("categoryTechnique");
	if (!strcmp(categoryTechnique, "/scanner/device/vulnerability"))
		return Convert_vulnerability(cef, log);
	else if (!strcmp(categoryTechnique, "/scanner/device/openport"))
		return Convert_openport(cef, log);
	return false;
}
//-----------------------------------------------------------------------------
bool JC_Preprocesseor_CEF_MacAfee::Convert_vulnerability(JI_CEF * cef, MS_PreparedDataForLogger * log)
{
	char CVES[1000];
	memset(CVES, 0, sizeof(CVES));
	strncpy(CVES, cef->GetValueFromLabeledProperty("CVE"), sizeof(CVES) - 1);
	char * flds[10];

	log->LogType = JE_Log_Type_MacAfee_Vulnerability;
	strncpy(log->log.vulnerability.DeviceVendor, cef->DeviceVendor(), sizeof(log->log.vulnerability.DeviceVendor) - 1);
	strncpy(log->log.vulnerability.DeviceProduct, cef->DeviceProduct(), sizeof(log->log.vulnerability.DeviceProduct) - 1);
	strncpy(log->log.vulnerability.DeviceVersion, cef->DeviceVersion(), sizeof(log->log.vulnerability.DeviceVersion) - 1);
	strncpy(log->log.vulnerability.SignatureID, cef->SignatureID(), sizeof(log->log.vulnerability.SignatureID) - 1);
	strncpy(log->log.vulnerability.Name, cef->Name(), sizeof(log->log.vulnerability.Name) - 1);
	strncpy(log->log.vulnerability.Severity, cef->Severity(), sizeof(log->log.vulnerability.Severity) - 1);
	strncpy(log->log.vulnerability.dst, cef->GetExtensionByName("dst"), sizeof(log->log.vulnerability.dst) - 1);
	strncpy(log->log.vulnerability.dntdom, cef->GetExtensionByName("dntdom"), sizeof(log->log.vulnerability.dntdom) - 1);
	strncpy(log->log.vulnerability.object, cef->GetExtensionByName("categoryObject"), sizeof(log->log.vulnerability.object) - 1);
	strncpy(log->log.vulnerability.os, cef->GetValueFromLabeledProperty("OSName"), sizeof(log->log.vulnerability.os) - 1);
	strncpy(log->log.vulnerability.oh, cef->GetValueFromLabeledProperty("Organization Hierarchy"), sizeof(log->log.vulnerability.oh) - 1);
	strncpy(log->log.vulnerability.ahost, cef->GetExtensionByName("ahost"), sizeof(log->log.vulnerability.ahost) - 1);
	strncpy(log->log.vulnerability.agt, cef->GetExtensionByName("agt"), sizeof(log->log.vulnerability.agt) - 1);
	strncpy(log->log.vulnerability.av, cef->GetExtensionByName("av"), sizeof(log->log.vulnerability.av) - 1);
	strncpy(log->log.vulnerability.ad_netbiosname, cef->GetExtensionByName("ad.NetBiosName"), sizeof(log->log.vulnerability.ad_netbiosname) - 1);
	log->log.vulnerability.SeverityN = atoi(cef->GetExtensionByName("deviceSeverity"));

	log->log.vulnerability.CVECount = Split(CVES, flds, ',');
	for (int i=0; i<log->log.vulnerability.CVECount; i++)
		strncpy(log->log.vulnerability.CVE[i], flds[i],sizeof(log->log.vulnerability.CVE[i]) -1);
	return true;
}
//-----------------------------------------------------------------------------
bool JC_Preprocesseor_CEF_MacAfee::Convert_openport(JI_CEF * cef, MS_PreparedDataForLogger * log)
{
	log->LogType = JE_Log_Type_MacAfee_Openport;
	strncpy(log->log.openport.DeviceVendor, cef->DeviceVendor(), sizeof(log->log.openport.DeviceVendor) - 1);
	strncpy(log->log.openport.DeviceProduct, cef->DeviceProduct(), sizeof(log->log.openport.DeviceProduct) - 1);
	strncpy(log->log.openport.DeviceVersion, cef->DeviceVersion(), sizeof(log->log.openport.DeviceVersion) - 1);
	strncpy(log->log.openport.SignatureID, cef->SignatureID(), sizeof(log->log.openport.SignatureID) - 1);
	strncpy(log->log.openport.Name, cef->Name(), sizeof(log->log.openport.Name) - 1);
	strncpy(log->log.openport.Severity, cef->Severity(), sizeof(log->log.openport.Severity) - 1);
	strncpy(log->log.openport.dst, cef->GetExtensionByName("dst"), sizeof(log->log.openport.dst) - 1);
	strncpy(log->log.openport.dntdom, cef->GetExtensionByName("dntdom"), sizeof(log->log.openport.dntdom) - 1);
	strncpy(log->log.openport.object, cef->GetExtensionByName("categoryObject"), sizeof(log->log.openport.object) - 1);
	strncpy(log->log.openport.os, cef->GetValueFromLabeledProperty("OSName"), sizeof(log->log.openport.os) - 1);
	strncpy(log->log.openport.oh, cef->GetValueFromLabeledProperty("Organization Hierarchy"), sizeof(log->log.openport.oh) - 1);
	strncpy(log->log.openport.ahost, cef->GetExtensionByName("ahost"), sizeof(log->log.openport.ahost) - 1);
	strncpy(log->log.openport.agt, cef->GetExtensionByName("agt"), sizeof(log->log.openport.agt) - 1);
	strncpy(log->log.openport.av, cef->GetExtensionByName("av"), sizeof(log->log.openport.av) - 1);
	strncpy(log->log.openport.ad_netbiosname, cef->GetExtensionByName("ad.NetBiosName"), sizeof(log->log.openport.ad_netbiosname) - 1);


	strncpy(log->log.openport.prot, cef->GetExtensionByName("proto"), sizeof(log->log.openport.prot) - 1);
	log->log.openport.dport = atoi(cef->GetExtensionByName("dpt"));
	strncpy(log->log.openport.dproc, cef->GetExtensionByName("dproc"), sizeof(log->log.openport.dproc) - 1);


	return true;
}
//-----------------------------------------------------------------------------
