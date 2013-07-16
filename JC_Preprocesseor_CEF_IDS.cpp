#include "JC_Preprocesseor_CEF_IDS.h"

JC_Preprocesseor_CEF_IDS::JC_Preprocesseor_CEF_IDS()
{
	JP_RuleManage = 0;
}
bool JC_Preprocesseor_CEF_IDS::Start(JC_Config * config, JI_CgiLogger * logger)
{
	JP_Config = config;
	JP_Logger = logger;

	JP_RuleManage = new JC_IDSRuleManager();
	if (!JP_RuleManage->Start(JP_Config, JP_Logger))
	{
		JP_Logger->Log("Could not load IDS Rule Manager");
		return false;
	}
	JP_SensorManage = new JC_IDSSensorManager();
	if (!JP_SensorManage->Start(JP_Config, JP_Logger))
	{
		JP_Logger->Log("Could not load IDS Sensor Manager");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void JC_Preprocesseor_CEF_IDS::End()
{

	if (JP_SensorManage)
	{
		JP_SensorManage->End();
		delete JP_SensorManage;
	}

	if (JP_RuleManage)
	{
		JP_RuleManage->End();
		delete JP_RuleManage;
	}

}
//-----------------------------------------------------------------------------
bool JC_Preprocesseor_CEF_IDS::Convert(JI_CEF * cef, MS_PreparedDataForLogger * log)
{
	if (strcmp(cef->DeviceVendor(),  "DOURAN"))
		return false;

	if (!strcmp(cef->GetExtensionByName("proto"), "UDP"))
		log->log.DIDS.PROTOCOL = 17;
	else if (!strcmp(cef->GetExtensionByName("proto"), "TCP"))
		log->log.DIDS.PROTOCOL = 6;
	else
		log->log.DIDS.PROTOCOL = 0;

	log->log.DIDS.SIP = StrToIP(cef->GetExtensionByName("src"));
	log->log.DIDS.DIP = StrToIP(cef->GetExtensionByName("dst"));
	log->log.DIDS.DPORT = atoi(cef->GetExtensionByName("dpt"));
	log->log.DIDS.SPORT = atoi(cef->GetExtensionByName("spt"));
	log->log.DIDS.SCOUNTRY = atoi(cef->GetExtensionByName("sc"));
	log->log.DIDS.DCOUNTRY = atoi(cef->GetExtensionByName("dc")) ;
	log->log.DIDS.PACKET = atoi(cef->GetExtensionByName("pcount"));
	log->log.DIDS.BYTE = atoi(cef->GetExtensionByName("ssize"));

	log->log.DIDS.SID = atoi(cef->GetExtensionByName("sid"));
	log->log.DIDS.MID = GetMid(cef->GetExtensionByName("msg"), log->log.DIDS.SID, cef->GetExtensionByName("m"));
	log->log.DIDS.ACTIONID = ActionID(cef->GetExtensionByName("action"));

	log->log.DIDS.SENSORID = SensorID(cef->SignatureID(), log->ip);
	log->log.DIDS.SCOUNTRY = log->log.DIDS.SCOUNTRY > 300?300:log->log.DIDS.SCOUNTRY;
	log->log.DIDS.DCOUNTRY = log->log.DIDS.DCOUNTRY > 300?300:log->log.DIDS.DCOUNTRY;


	log->log.DIDS.ssize = atoi(cef->GetExtensionByName("ssize"));
	log->log.DIDS.size  = atoi(cef->GetExtensionByName("size"));
	log->log.DIDS.pcount = atoi(cef->GetExtensionByName("pcount"));
	log->log.DIDS.tcprate = atoi(cef->GetExtensionByName("tcprate"));
	log->log.DIDS.udprate = atoi(cef->GetExtensionByName("udprate"));
	log->log.DIDS.icmprate = atoi(cef->GetExtensionByName("icmprate"));
	log->log.DIDS.orate = atoi(cef->GetExtensionByName("orate"));
	log->log.DIDS.serate = atoi(cef->GetExtensionByName("serate"));
	log->log.DIDS.usrate = atoi(cef->GetExtensionByName("usrate"));
	log->log.DIDS.secount = atoi(cef->GetExtensionByName("secount"));
	log->log.DIDS.uscount = atoi(cef->GetExtensionByName("uscount"));
	log->log.DIDS.tcpratep = atoi(cef->GetExtensionByName("tcpratep"));
	log->log.DIDS.udpratep = atoi(cef->GetExtensionByName("udpratep"));
	log->log.DIDS.icmpratep = atoi(cef->GetExtensionByName("icmpratep"));
	log->log.DIDS.otherratep = atoi(cef->GetExtensionByName("otherratep"));
	log->log.DIDS.anomaliy = atoi(cef->GetExtensionByName("anomaliy"));
	log->log.DIDS.direction = atoi(cef->GetExtensionByName("direction"));
	log->LogType = JE_Log_Type_DIDS;
	return true;
}
//-----------------------------------------------------------------------------
int JC_Preprocesseor_CEF_IDS::SensorID(const char * p, IP ip)
{
	return JP_SensorManage->GetHostID(p, ip);
}
//-----------------------------------------------------------------------------
int JC_Preprocesseor_CEF_IDS::ActionID(const char * p)
{
	if (!strcmp(p, "alert")) return 2;
	if (!strcmp(p, "")) 	 return 1;
	return 0;
}
//-----------------------------------------------------------------------------
int JC_Preprocesseor_CEF_IDS::GetMid(const char * message, int sid, const char * module)
{
	return JP_RuleManage->GetMID(message, sid, module);
}
