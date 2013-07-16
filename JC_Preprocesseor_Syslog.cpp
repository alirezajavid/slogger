#include "JC_Preprocesseor_Syslog.h"

JC_Preprocesseor_Syslog::JC_Preprocesseor_Syslog() {
}
//--------------------------------------------------------------------------------------------------
bool JC_Preprocesseor_Syslog::Start(JC_Config * Config, JI_CgiLogger * Logger){
	JI_Preprocesseor_CEF * prp = 0;

	prp = new JC_Preprocesseor_CEF_MacAfee();
	CEF_ConverterManager.push_back(prp);

	prp = new JC_Preprocesseor_CEF_Sophos();
	CEF_ConverterManager.push_back(prp);

	prp = new JC_Preprocesseor_CEF_Acs();
	CEF_ConverterManager.push_back(prp);

	prp = new JC_Preprocesseor_CEF_Tripwire;
	CEF_ConverterManager.push_back(prp);

	BiSHoorBazi = new JC_Preprocesseor_CEF_IDS;
	BiSHoorBazi->Start(Config, Logger);
	prp = BiSHoorBazi;

	CEF_ConverterManager.push_back(prp);

	return true;
}
//-----------------------------------------------------------------------------
void JC_Preprocesseor_Syslog::End()
{
	JI_Preprocesseor_CEF * prp = 0;
	BiSHoorBazi->End();
	for (unsigned int i=0; i<CEF_ConverterManager.size(); i++)
	{
		prp = CEF_ConverterManager.at(i);
		delete prp;
		prp = 0;
	}
	CEF_ConverterManager.clear();
}
//-----------------------------------------------------------------------------
bool JC_Preprocesseor_Syslog::Process(MS_DataProviderBuffer* inBuf,
				      OUT MS_PreparedDataForLogger* buffer,
				      OUT int & PreparedDataCount)
{

	buffer->ip = inBuf->DeviceIP;
	PreparedDataCount = 0;
	if (inBuf->LogType != ME_ProviderLogType_Syslog)
		return false;

	if (!PreparedDataCount)
	{
		JC_CEF * cef = new JC_CEF();
		if (cef->Start(inBuf->buf))
			if (Parse_From_CEF_Queue(cef, buffer))
				PreparedDataCount = 1;
		cef->End();
		delete cef;
		cef = 0;
	}

	if (!PreparedDataCount)
	{
		JC_DIDS * csc = new JC_DIDS();
		if (csc->Start(inBuf->buf))
		{
			buffer->LogType = JE_Log_Type_DIDS;
			buffer->log.DIDS.PROTOCOL = csc->Get_Protocol();
			buffer->log.DIDS.DCOUNTRY = csc->Get_DstCountry();
			buffer->log.DIDS.SCOUNTRY = csc->Get_SrcCountry();
			buffer->log.DIDS.DIP = csc->Get_DestinationIP();
			buffer->log.DIDS.SIP = csc->Get_SourceIP();
			buffer->log.DIDS.DPORT = csc->Get_DestinationPort();
			buffer->log.DIDS.SPORT = csc->Get_SourcePort();
			if (buffer->log.DIDS.SCOUNTRY != 65534 && buffer->log.DIDS.DCOUNTRY != 65534 )
				PreparedDataCount = 1;
		}
		csc->End();
		delete csc;
		csc = 0;

		/*
		JC_CISCO * csc = new JC_CISCO();
		if (csc->Start(inBuf->buf))
		{
			buffer->LogType = JE_Log_Type_Cisco;
			strncpy(buffer->log.Cisco.Category, csc->GetType(), sizeof(buffer->log.Cisco.Category) - 1);
			strncpy(buffer->log.Cisco.Message, csc->GetMessage(), sizeof(buffer->log.Cisco.Message) - 1);
			IPToStr(JP_DeviceIP, buffer->log.Cisco.IP);
			PreparedDataCount = 1;
		}

		csc->End();
		delete csc;
		csc = 0;
*/
	}
	return PreparedDataCount;
}
//-----------------------------------------------------------------------------
bool JC_Preprocesseor_Syslog::Parse_From_CEF_Queue(JI_CEF * cef, MS_PreparedDataForLogger * buffer)
{
	JI_Preprocesseor_CEF * prp = 0;
	for (unsigned int i=0; i<CEF_ConverterManager.size(); i++)
	{
		prp = CEF_ConverterManager.at(i);
		if (prp->Convert(cef, buffer))
			return true;
	}
	return false;
}
