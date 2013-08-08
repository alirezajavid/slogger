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
	JP_ActionManage = new JC_IDSActionManager();
	if (!JP_ActionManage->Start(JP_Config, JP_Logger))
	{
		JP_Logger->Log("Could not load IDS Action Manager");
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
void JC_Preprocesseor_CEF_IDS::End()
{

	if (JP_ActionManage)
	{
		JP_ActionManage->End();
		delete JP_ActionManage;
	}

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
	log->log.DIDS.PACKET = atoi(cef->GetExtensionByName("pcount"));
	log->log.DIDS.BYTE = atoi(cef->GetExtensionByName("ssize"));

	log->log.DIDS.SID = atoi(cef->GetExtensionByName("sid"));

	log->log.DIDS.anomaliy = atoi(cef->GetExtensionByName("anomaliy"));
	log->log.DIDS.SEVERITY = atoi(cef->Severity());
	log->log.DIDS.module = GetModule(cef->GetExtensionByName("m"));
	log->log.DIDS.MID = GetMid(cef->Name(), log->log.DIDS.SID, cef->GetExtensionByName("m"), atoi(cef->Severity()), log->log.DIDS.anomaliy);
	log->log.DIDS.ACTIONID = ActionID(cef->GetExtensionByName("action"));
	log->log.DIDS.SENSORID = SensorID(cef->SignatureID(), log->ip);

	log->log.DIDS.direction = atoi(cef->GetExtensionByName("direction"));
	if (log->log.DIDS.direction == 2)
	{
		log->log.DIDS.DCOUNTRY = atoi(cef->GetExtensionByName("sc"));
		log->log.DIDS.SCOUNTRY = atoi(cef->GetExtensionByName("dc")) ;
	}
	else
	{
		log->log.DIDS.SCOUNTRY = atoi(cef->GetExtensionByName("sc"));
		log->log.DIDS.DCOUNTRY = atoi(cef->GetExtensionByName("dc")) ;
	}
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
	return JP_ActionManage->GetAction(p);
}
//-----------------------------------------------------------------------------
int JC_Preprocesseor_CEF_IDS::GetMid(const char * message, int sid, const char * module, int Severity, int anomaly)
{
	return JP_RuleManage->GetMID(message, sid, module, Severity, anomaly);
}

int JC_Preprocesseor_CEF_IDS::GetModule(const char * module)
{
	if (!strcmp(module, "http_send")) 		return 0;
	if (!strcmp(module, "general")) 		return 15;
	if (!strcmp(module, "ip")) 				return 16;
	if (!strcmp(module, "ssl_send")) 		return 2;
	if (!strcmp(module, "dips")) 			return 1;
	if (!strcmp(module, "")) 				return 3;
	if (!strcmp(module, "scada")) 			return 4;
	if (!strcmp(module, "ftp")) 			return 5;
	if (!strcmp(module, "dns_tcp")) 		return 6;
	if (!strcmp(module, "netbios")) 		return 7;
	if (!strcmp(module, "telnet")) 			return 8;
	if (!strcmp(module, "dns_udp")) 		return 9;
	if (!strcmp(module, "db_guard")) 		return 10;
	if (!strcmp(module, "http_recv")) 		return 11;
	if (!strcmp(module, "imap")) 			return 12;
	if (!strcmp(module, "tftp")) 			return 13;
	if (!strcmp(module, "ssl_recv")) 		return 14;
	if (!strcmp(module, "ssh")) 			return 17;
	if (!strcmp(module, "smtp")) 			return 18;
	if (!strcmp(module, "icmp")) 			return 19;
	return 20;
}
