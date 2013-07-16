#include "JC_Logger_DatabaseWriter.h"

JC_Logger_DatabaseWriter::JC_Logger_DatabaseWriter() {
	memset(&JP_IDS_Vector, 0, sizeof(JP_IDS_Vector));
	JP_IDS_Vector_Index = 0;
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::Start(JI_CgiLogger * logger, JC_Config * config)
{
	JP_Logger = logger;
	JP_Config = config;
	JP_db = GetDataBase(JP_Config, JP_Logger, JE_Database_Type_Oracle);

	//JP_Trends = new JC_TopTrender();
	//if (!JP_Trends->Start(JP_Logger))
	//{
	//	JP_Logger->Log("Could not make Top Trender List.");
	//	return false;
	//}

	JP_RatesStatistics = new JC_RatesStatistics();
	if (!JP_RatesStatistics->Start(JP_Logger))
	{
		JP_Logger->Log("Could not make JP_RatesStatistics List.");
		return false;
	}

	JP_IDS_Vector.ACTIONID 	= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.DT 		= (OCI_Date**)JGetMem(sizeof(OCI_Date*) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.MID 		= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.SENSORID 	= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.PROTOCOL 	= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.SIP 		= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.DIP 		= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.SPORT 	= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.DPORT 	= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.SCOUNTRY 	= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.DCOUNTRY 	= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.PACKET 	= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.BYTE 		= (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JP_IDS_Vector.DIRECTION = (int *)JGetMem(sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);

	for (int i=0; i<JP_Config->GetConfig()->Logger.BulkSize; i++)
		JP_IDS_Vector.DT[i] = OCI_DateCreate(NULL);

	JP_RatesStatisticsLastTime = GetTickCount();
	JP_TopStatisticsLastTime = GetTickCount();
	return true;
}
//-----------------------------------------------------------------------------
void JC_Logger_DatabaseWriter::End() {

	for (int i=0; i<JP_Config->GetConfig()->Logger.BulkSize; i++)
		 OCI_DateFree(JP_IDS_Vector.DT[i]);

	JFreeMem(JP_IDS_Vector.ACTIONID, 	sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.DT, 			sizeof(OCI_Date*) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.MID, 		sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.SENSORID, 	sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.PROTOCOL, 	sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.SIP, 		sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.DIP, 		sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.SPORT, 		sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.DPORT, 		sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.SCOUNTRY, 	sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.DCOUNTRY, 	sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.PACKET, 		sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.BYTE, 		sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);
	JFreeMem(JP_IDS_Vector.DIRECTION, 	sizeof(int) * JP_Config->GetConfig()->Logger.BulkSize);

	if (JP_RatesStatistics)
	{
		JP_RatesStatistics->End();
		delete JP_RatesStatistics;
	}

	//if (JP_Trends)
	//{
	//	JP_Trends->End();
	//	delete JP_Trends;
	//}

	if (JP_db)
	{
		JP_db->End();
		delete JP_db;
	}

}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::Log(MS_PreparedDataForLogger* preProcessResult)
{
	JP_Row = preProcessResult;

	if (!JP_db && JP_Row->LogType != JE_Log_Type_DIDS)
	{
		JP_Logger->Log("Could not prepare db connection");
		return false;
	}

	switch (JP_Row->LogType)
	{
		case JE_Log_Type_ACS:							prepareQuery_ACS();						break;
		case JE_Log_Type_MacAfee_Vulnerability:			prepareQuery_Vulnerability();			break;
		case JE_Log_Type_MacAfee_Openport:				prepareQuery_Openports();				break;
		case JE_Log_Type_Sophos:						prepareQuery_Sophos();					break;
		case JE_Log_Type_Tripwire:						prepareQuery_Tripwire();				break;
		case JE_Log_Type_Cisco:							prepareQuery_Cisco();					break;
		case JE_Log_Type_DIDS:							prepareQuery_DIDS();					break;

		case JE_Log_Type_None:
		case JE_Log_Type_Max:																	break;
	};

	return true;
}
//-----------------------------------------------------------------------------
const char * JC_Logger_DatabaseWriter::SetNow(bool b = false)
{
	if (b)
	{
		memset(JP_NOW, 0, 21);
		time_t curtime;
		struct tm *loctime;
		curtime = time (NULL);
		loctime = localtime (&curtime);
		strftime (JP_NOW, 21, "%Y_%m_%d_%H", loctime);
		return JP_NOW;
	}
	else
	{
		memset(JP_NOW, 0, 21);
		time_t curtime;
		struct tm *loctime;
		curtime = time (NULL);
		loctime = localtime (&curtime);
		strftime (JP_NOW, 21, "%Y-%m-%d %H:%M:%S", loctime);
		return JP_NOW;
	}
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::prepareQuery_Openports()
{
	MI_DataBaseRow * row;
	int id=0;
	char query[MCONST_MAXCHARBUF];
	sprintf(query, "INSERT INTO VULSC_McAfee VALUES(null,to_date(?, 'YYYY-MM-DD HH24:MI:SS'),?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	JP_db->BeginTransaction();

	if (!JP_db->PreperSQL(query))
	{
	    JP_Logger->Log("Could not Prepare Statement", EXP);
	    return false;
	}
	JP_db->BindText(0, SetNow());
	JP_db->BindText(1, JP_Row->log.openport.DeviceVendor);
	JP_db->BindText(2, JP_Row->log.openport.DeviceProduct);
	JP_db->BindText(3, JP_Row->log.openport.DeviceVersion);
	JP_db->BindText(4, JP_Row->log.openport.SignatureID);
	JP_db->BindText(5, JP_Row->log.openport.Name);
	JP_db->BindText(6, JP_Row->log.openport.Severity);
	JP_db->BindText(7, JP_Row->log.openport.dst);
	JP_db->BindText(8, JP_Row->log.openport.dntdom);
	JP_db->BindText(9, JP_Row->log.openport.object);
	JP_db->BindText(10, JP_Row->log.openport.oh);
	JP_db->BindText(11, JP_Row->log.openport.os);
	JP_db->BindText(12, JP_Row->log.openport.ahost);
	JP_db->BindText(13, JP_Row->log.openport.agt);
	JP_db->BindText(14, JP_Row->log.openport.av);
	JP_db->BindText(15, JP_Row->log.openport.ad_netbiosname);
	if(!JP_db->Execute())
	{
	    JP_db->Roollback();
	    return false;
	}
	JP_db->Finalize();

	JP_db->Query("SELECT max(id) FROM VULSC_McAfee");
	row = JP_db->Read();
	if (row)
		id = row->GetFieldInt(1);
	JP_db->Finalize();

	sprintf(query, "INSERT INTO VULSC_OPENPORTS VALUES(null,?,?,?,?)");
	if (!JP_db->PreperSQL(query))
	{
		JP_Logger->Log("Could not Prepare Statement", EXP);
		JP_db->Roollback();
		return false;
	}
	JP_db->BindInt(0, id);
	JP_db->BindText(1, JP_Row->log.openport.prot);
	JP_db->BindText(2, JP_Row->log.openport.dproc);
	JP_db->BindInt(3, JP_Row->log.openport.dport);
	JP_db->Execute();
	JP_db->EndTransaction();
	return true;
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::prepareQuery_Vulnerability()
{
	MI_DataBaseRow * row;
	int id=0;
	char query[MCONST_MAXCHARBUF];
	sprintf(query, "INSERT INTO VULSC_McAfee VALUES(null,to_date(?, 'YYYY-MM-DD HH24:MI:SS'),?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	JP_db->BeginTransaction();

	if (!JP_db->PreperSQL(query))
	{
	    JP_Logger->Log("Could not Prepare Statement", EXP);
	    return false;
	}
	JP_db->BindText(0, SetNow());
	JP_db->BindText(1, JP_Row->log.vulnerability.DeviceVendor);
	JP_db->BindText(2, JP_Row->log.vulnerability.DeviceProduct);
	JP_db->BindText(3, JP_Row->log.vulnerability.DeviceVersion);
	JP_db->BindText(4, JP_Row->log.vulnerability.SignatureID);
	JP_db->BindText(5, JP_Row->log.vulnerability.Name);
	JP_db->BindText(6, JP_Row->log.vulnerability.Severity);
	JP_db->BindText(7, JP_Row->log.vulnerability.dst);
	JP_db->BindText(8, JP_Row->log.vulnerability.dntdom);
	JP_db->BindText(9, JP_Row->log.vulnerability.object);
	JP_db->BindText(10, JP_Row->log.vulnerability.oh);
	JP_db->BindText(11, JP_Row->log.vulnerability.os);
	JP_db->BindText(12, JP_Row->log.vulnerability.ahost);
	JP_db->BindText(13, JP_Row->log.vulnerability.agt);
	JP_db->BindText(14, JP_Row->log.vulnerability.av);
	JP_db->BindText(15, JP_Row->log.vulnerability.ad_netbiosname);
	if(!JP_db->Execute())
	{
	    JP_db->Roollback();
	    return false;
	}
	JP_db->Finalize();

	JP_db->Query("SELECT max(id) FROM VULSC_McAfee");
	row = JP_db->Read();
	if (row)
		id = row->GetFieldInt(1);
	JP_db->Finalize();

	sprintf(query, "INSERT INTO VULSC_Vulnerability VALUES(null,?,?)");
	if (!JP_db->PreperSQL(query))
	{
		JP_Logger->Log("Could not Prepare Statement", EXP);
		JP_db->Roollback();
		return false;
	}
	JP_db->BindInt(0, id);
	JP_db->BindInt(1, JP_Row->log.vulnerability.SeverityN);
	if(!JP_db->Execute())
	{
		JP_db->Roollback();
		return false;
	}

	JP_db->Query("SELECT max(id) FROM VULSC_vulnerability");
	row = JP_db->Read();
	if (row)
		id = row->GetFieldInt(1);

	JP_db->Finalize();

	sprintf(query, "INSERT INTO VULSC_vulnerability_cve VALUES(?,?)");
	if (!JP_db->PreperSQL(query))
	{
		JP_Logger->Log("Could not Prepare Statement", EXP);
		JP_db->Roollback();
		return false;
	}
	for (int i=0; i< JP_Row->log.vulnerability.CVECount; i++)
	{
		JP_db->BindInt(0, id);
		JP_db->BindText(1, JP_Row->log.vulnerability.CVE[i]);
		if(!JP_db->Execute())
		{
			JP_db->Roollback();
			return false;
		}
	}
	JP_db->EndTransaction();
	return true;
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::prepareQuery_Sophos()
{
	char query[MCONST_MAXCHARBUF];
	sprintf(query, "INSERT INTO ANTIVIRUS_SOPHOS VALUES(null,to_date(?, 'YYYY-MM-DD HH24:MI:SS'),?,?,?,?,?,?)");
	JP_db->BeginTransaction();

	if (!JP_db->PreperSQL(query))
	{
	    JP_Logger->Log("Could not Prepare Statement", EXP);
	    return false;
	}
	JP_db->BindText(0, SetNow());
	JP_db->BindText(1, JP_Row->log.sophos.Version);
	JP_db->BindText(2, JP_Row->log.sophos.Name);
	JP_db->BindText(3, JP_Row->log.sophos.Severity);
	JP_db->BindText(4, JP_Row->log.sophos.dHost);
	JP_db->BindText(5, JP_Row->log.sophos.Domain);
	JP_db->BindText(6, JP_Row->log.sophos.FilePath);
	if(!JP_db->Execute())
	{
	    JP_db->Roollback();
	    return false;
	}
	JP_db->Finalize();
	JP_db->EndTransaction();
	return true;
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::prepareQuery_ACS()
{
	char query[MCONST_MAXCHARBUF];
	sprintf(query, "INSERT INTO acs_acs VALUES(null,to_date(?, 'YYYY-MM-DD HH24:MI:SS'),?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	JP_db->BeginTransaction();

	if (!JP_db->PreperSQL(query))
	{
	    JP_Logger->Log("Could not Prepare Statement", EXP);
	    return false;
	}
	JP_db->BindText(0, SetNow());
	JP_db->BindText(1, JP_Row->log.ACS.Version);
	JP_db->BindText(2, JP_Row->log.ACS.Severity);
	JP_db->BindText(3, JP_Row->log.ACS.Msg);
	JP_db->BindText(4, JP_Row->log.ACS.Cat);
	JP_db->BindText(5, JP_Row->log.ACS.DeviceSeverity);
	JP_db->BindText(6, JP_Row->log.ACS.Dst);
	JP_db->BindText(7, JP_Row->log.ACS.DntDom);
	JP_db->BindText(8, JP_Row->log.ACS.DUID);
	JP_db->BindText(9, JP_Row->log.ACS.CallerId);
	JP_db->BindText(10, JP_Row->log.ACS.AuthorFaulureCode);
	JP_db->BindText(11, JP_Row->log.ACS.AuthorData);
	JP_db->BindText(12, JP_Row->log.ACS.AHost);
	JP_db->BindText(13, JP_Row->log.ACS.Agt);
	JP_db->BindText(14, JP_Row->log.ACS.Av);
	JP_db->BindText(15, JP_Row->log.ACS.DVC);
	if(!JP_db->Execute())
	{
	    JP_db->Roollback();
	    return false;
	}
	JP_db->Finalize();
	JP_db->EndTransaction();
	return true;
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::prepareQuery_Tripwire()
{
	char query[MCONST_MAXCHARBUF];
	sprintf(query, "INSERT INTO TRIPWIRE_TRIPWIRE VALUES(null,to_date(?, 'YYYY-MM-DD HH24:MI:SS'),?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	JP_db->BeginTransaction();

	if (!JP_db->PreperSQL(query))
	{
	    JP_Logger->Log("Could not Prepare Statement", EXP);
	    return false;
	}
	JP_db->BindText(0, SetNow());
	JP_db->BindText(1, JP_Row->log.Tripwire.Rulename);
	JP_db->BindText(2, JP_Row->log.Tripwire.Object);
	JP_db->BindText(3, JP_Row->log.Tripwire.dHost);
	JP_db->BindText(4, JP_Row->log.Tripwire.dst);
	JP_db->BindText(5, JP_Row->log.Tripwire.AHost);
	JP_db->BindText(6, JP_Row->log.Tripwire.Agt);
	JP_db->BindText(7, JP_Row->log.Tripwire.Av);
	JP_db->BindText(8, JP_Row->log.Tripwire.Message);
	JP_db->BindText(9, JP_Row->log.Tripwire.dvcHost);
	JP_db->BindText(10, JP_Row->log.Tripwire.dvc);
	JP_db->BindText(11, JP_Row->log.Tripwire.sHost);
	JP_db->BindText(12, JP_Row->log.Tripwire.sUser);
	JP_db->BindText(13, JP_Row->log.Tripwire.src);
	JP_db->BindText(14, JP_Row->log.Tripwire.deviceSeverity);
	JP_db->BindText(15, JP_Row->log.Tripwire.act);
	JP_db->BindText(16, JP_Row->log.Tripwire.cat);

	if(!JP_db->Execute())
	{
	    JP_db->Roollback();
	    return false;
	}
	JP_db->Finalize();
	JP_db->EndTransaction();
	return true;
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::prepareQuery_Cisco()
{
	char query[MCONST_MAXCHARBUF];
	sprintf(query, "INSERT INTO CISCO_AUDIT VALUES(null,to_date(?, 'YYYY-MM-DD HH24:MI:SS'),?,?,?)");
	JP_db->BeginTransaction();

	if (!JP_db->PreperSQL(query))
	{
	    JP_Logger->Log("Could not Prepare Statement", EXP);
	    return false;
	}
	JP_db->BindText(0, SetNow());
	JP_db->BindText(1, JP_Row->log.Cisco.Category);
	JP_db->BindText(2, JP_Row->log.Cisco.Message);
	JP_db->BindText(3, JP_Row->log.Cisco.IP);

	if(!JP_db->Execute())
	{
	    JP_db->Roollback();
	    return false;
	}
	JP_db->Finalize();
	JP_db->EndTransaction();
	return true;
}
//-----------------------------------------------------------------------------
void JC_Logger_DatabaseWriter::DeleteOldTxtFiles()
{
	char filename[100];
	memset(filename, 0, 100);
	time_t curtime;
	struct tm *loctime;


	curtime = time (NULL) - 3600 * JP_Config->GetConfig()->General.DeleteBeforeHours;
	loctime = localtime (&curtime);
	strftime (filename, 100, "/var/log/ids_archive/%Y_%m_%d_%H.txt", loctime);

	while (remove(filename) == 0)
	{
		JP_Logger->Log("%s deleted!", filename);
		curtime = curtime - 3600 ;
		loctime = localtime (&curtime);
		strftime (filename, 100, "/var/log/ids_archive/%Y_%m_%d_%H.txt", loctime);
	}
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::LogIDSlogToFile()
{
	static int i = 9999;
	char file[MCONST_MINCHARBUF];
	char sip[20], dip[20];
	i++;
	if (i==10000){
		DeleteOldTxtFiles();
		i = 0;
	}

	sprintf(file, "/var/log/ids_archive/%s.txt", SetNow(true));
	FILE * fp = fopen(file, "a");
	if (!fp)
		return false;
	IPToStr(JP_Row->log.DIDS.SIP, sip);
	IPToStr(JP_Row->log.DIDS.DIP, dip);
	fprintf(fp, "%s,%d,%d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d\n",
				SetNow(),
				JP_Row->log.DIDS.MID,
				JP_Row->log.DIDS.ACTIONID,
				sip,
				dip,
				JP_Row->log.DIDS.SPORT,
				JP_Row->log.DIDS.DPORT,

				JP_Row->log.DIDS.SCOUNTRY,
				JP_Row->log.DIDS.DCOUNTRY,

				JP_Row->log.DIDS.PROTOCOL,
				JP_Row->log.DIDS.SENSORID,

				JP_Row->log.DIDS.PACKET,
				JP_Row->log.DIDS.BYTE
	);
	fclose(fp);
	return true;
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::prepareQuery_DIDS()
{
	long lng;
	OCI_Statement	* st;
	OCI_Error		* err;

	LogIDSlogToFile();

	/*
	if (JP_Config->GetConfig()->Logger.WriteTopStatisticsInterval)
    {
    	JP_Trends->Process(&JP_Row->log.DIDS);
		if ((GetTickCount() - JP_TopStatisticsLastTime) > (JP_Config->GetConfig()->Logger.WriteTopStatisticsInterval * 1000))
		{
			prepareQuery_DIDS_TopStatistics();
			JP_TopStatisticsLastTime = GetTickCount();
		}
    }
	*/

    if (JP_Config->GetConfig()->Logger.WriteRateStatisticsInterval)
    {
    	JP_RatesStatistics->Process(&JP_Row->log.DIDS);
		if ((GetTickCount() - JP_RatesStatisticsLastTime) > (JP_Config->GetConfig()->Logger.WriteRateStatisticsInterval * 1000))
		{
			prepareQuery_DIDS_RateStatistics();
			JP_RatesStatisticsLastTime = GetTickCount();
		}
    }


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	OCI_DateSysDate(JP_IDS_Vector.DT[JP_IDS_Vector_Index]);
	JP_IDS_Vector.MID[JP_IDS_Vector_Index] 		 	= JP_Row->log.DIDS.MID;
	JP_IDS_Vector.SENSORID[JP_IDS_Vector_Index] 	= JP_Row->log.DIDS.SENSORID;
	JP_IDS_Vector.PROTOCOL[JP_IDS_Vector_Index] 	= JP_Row->log.DIDS.PROTOCOL;
	JP_IDS_Vector.SIP[JP_IDS_Vector_Index] 			= JP_Row->log.DIDS.SIP;
	JP_IDS_Vector.DIP[JP_IDS_Vector_Index] 			= JP_Row->log.DIDS.DIP;
	JP_IDS_Vector.SPORT[JP_IDS_Vector_Index] 		= JP_Row->log.DIDS.SPORT;
	JP_IDS_Vector.DPORT[JP_IDS_Vector_Index] 		= JP_Row->log.DIDS.DPORT;
	JP_IDS_Vector.SCOUNTRY[JP_IDS_Vector_Index] 	= JP_Row->log.DIDS.SCOUNTRY;
	JP_IDS_Vector.DCOUNTRY[JP_IDS_Vector_Index] 	= JP_Row->log.DIDS.DCOUNTRY;
	JP_IDS_Vector.ACTIONID[JP_IDS_Vector_Index] 	= JP_Row->log.DIDS.ACTIONID;
	JP_IDS_Vector.PACKET[JP_IDS_Vector_Index] 		= JP_Row->log.DIDS.PACKET;
	JP_IDS_Vector.BYTE[JP_IDS_Vector_Index] 		= JP_Row->log.DIDS.BYTE;
	JP_IDS_Vector.DIRECTION[JP_IDS_Vector_Index] 		= JP_Row->log.DIDS.direction;
	JP_IDS_Vector_Index++;
	if (JP_IDS_Vector_Index == JP_Config->GetConfig()->Logger.BulkSize)
	{
		lng = GetTickCount();
		JP_IDS_Vector_Index = 0;

		if (!JP_db->PreperSQL("INSERT INTO dids_ATTACKS(DT,MID,SENSORID,PROTOCOL,SIP,DIP,SPORT,DPORT,SCOUNTRY,DCOUNTRY,ACTIONID,PACKET,BYTE,DIRECTION) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)"))
		{
			JP_Logger->Log("Could not Prepare Statement", EXP);
			return false;
		}

		st = ((MC_Oracle *)JP_db)->Get_Statement();
		OCI_BindArraySetSize(st, JP_Config->GetConfig()->Logger.BulkSize);

		OCI_BindArrayOfDates  (st, ":p0", (OCI_Date**)JP_IDS_Vector.DT, 0);
		OCI_BindArrayOfInts   (st, ":p1", (int*)  JP_IDS_Vector.MID, 0);
		OCI_BindArrayOfInts   (st, ":p2", (int*)  JP_IDS_Vector.SENSORID, 0);

		OCI_BindArrayOfInts   (st, ":p3", (int*)  JP_IDS_Vector.PROTOCOL, 0);
		OCI_BindArrayOfUnsignedInts   (st, ":p4", (unsigned int*)  JP_IDS_Vector.SIP, 0);
		OCI_BindArrayOfUnsignedInts   (st, ":p5", (unsigned int*)  JP_IDS_Vector.DIP, 0);
		OCI_BindArrayOfInts   (st, ":p6", (int*)  JP_IDS_Vector.SPORT, 0);
		OCI_BindArrayOfInts   (st, ":p7", (int*)  JP_IDS_Vector.DPORT, 0);
		OCI_BindArrayOfInts   (st, ":p8", (int*)  JP_IDS_Vector.SCOUNTRY, 0);
		OCI_BindArrayOfInts   (st, ":p9", (int*)  JP_IDS_Vector.DCOUNTRY, 0);
		OCI_BindArrayOfInts   (st, ":p10", (int*)  JP_IDS_Vector.ACTIONID, 0);
		OCI_BindArrayOfInts   (st, ":p11", (int*)  JP_IDS_Vector.PACKET, 0);
		OCI_BindArrayOfInts   (st, ":p12", (int*)  JP_IDS_Vector.BYTE, 0);
		OCI_BindArrayOfInts   (st, ":p13", (int*)  JP_IDS_Vector.DIRECTION, 0);

		if (!OCI_Execute(st))
		{
		    JP_Logger->Log("Number of DML array errors : %d\n", OCI_GetBatchErrorCount(st));
			err = OCI_GetBatchError(st);
			while (err)
			{
			    JP_Logger->Log("Error at row %d : %s\n", OCI_ErrorGetRow(err), OCI_ErrorGetString(err));
				err = OCI_GetBatchError(st);
			}
			return false;
		}
		JP_db->Finalize();
	    JP_Logger->Log("logging %d records in %lu ms.", JP_Config->GetConfig()->Logger.BulkSize, GetTickCount() - lng);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::prepareQuery_DIDS_RateStatistics()
{
	long lng;
	OCI_Statement	* st;

	JS_Rate_Statistic_DB_Logs * ls;
	lng = GetTickCount();
    if (!JP_db->PreperSQL("INSERT INTO dids_RATES (DT,SENSORID,SSIZE,ATTACKSIZE,PCOUNT,TCPRATE,UDPRATE,ICMPRATE,ORATE,SERATE,USRATE,SECOUNT,USCOUNT,TCPRATEP,UDPRATEP,ICMPRATEP,OTHERRATEP)VALUES(SYSDATE,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"))
	{
		JP_Logger->Log("Could not Prepare Statement", EXP);
		return false;
	}
	st = ((MC_Oracle *)JP_db)->Get_Statement();
	ls = JP_RatesStatistics->GetLists();

	OCI_BindArraySetSize(st, JP_RatesStatistics->GetDBLogSize());
	OCI_BindArrayOfInts   (st, ":p0", (int*)  ls->SensorID, 0);
	OCI_BindArrayOfInts   (st, ":p1", (int*)  ls->ssize, 0);
	OCI_BindArrayOfInts   (st, ":p2", (int*)  ls->size, 0);
	OCI_BindArrayOfInts   (st, ":p3", (int*)  ls->pcount, 0);
	OCI_BindArrayOfInts   (st, ":p4", (int*)  ls->tcprate, 0);
	OCI_BindArrayOfInts   (st, ":p5", (int*)  ls->udprate, 0);
	OCI_BindArrayOfInts   (st, ":p6", (int*)  ls->icmprate, 0);
	OCI_BindArrayOfInts   (st, ":p7", (int*)  ls->orate, 0);
	OCI_BindArrayOfInts   (st, ":p8", (int*)  ls->serate, 0);
	OCI_BindArrayOfInts   (st, ":p9", (int*)  ls->usrate, 0);
	OCI_BindArrayOfInts   (st, ":p10", (int*)  ls->secount, 0);
	OCI_BindArrayOfInts   (st, ":p11", (int*)  ls->uscount, 0);
	OCI_BindArrayOfInts   (st, ":p12", (int*)  ls->tcpratep, 0);
	OCI_BindArrayOfInts   (st, ":p13", (int*)  ls->udpratep, 0);
	OCI_BindArrayOfInts   (st, ":p14", (int*)  ls->icmpratep, 0);
	OCI_BindArrayOfInts   (st, ":p15", (int*)  ls->otherratep, 0);

	if (!JP_db->Execute())
	{
		JP_Logger->Log("Could not execute", EXP);
		return false;
	}
	JP_db->Finalize();
	JP_Logger->Log("logging %7d records Rate in %7lu ms.", JP_RatesStatistics->GetDBLogSize(), GetTickCount() - lng);
	JP_RatesStatistics->FreeDBLists();
	return true;
}
//-----------------------------------------------------------------------------
bool JC_Logger_DatabaseWriter::prepareQuery_DIDS_TopStatistics()
{
	/*

	long lng;
	OCI_Statement	* st;

	JS_Statistic_DB_Logs * ls;
	lng = GetTickCount();
    if (!JP_db->PreperSQL("INSERT INTO dids_TOPS (DT,TYPE,KEY,ACTIONS,MID,DCOUNTRY,SCOUNTRY,DIP,SIP,DPORT,PACKETS,BYTES,COUNT) VALUES(SYSDATE,?,?,?,?,?,?,?,?,?,?,?,?)"))
	{
		JP_Logger->Log("Could not Prepare Statement", EXP);
		return false;
	}
	st = ((MC_Oracle *)JP_db)->Get_Statement();
	ls = JP_Trends->GetLists();

	OCI_BindArraySetSize(st, JP_Trends->GetDBLogSize());
	OCI_BindArrayOfInts   (st, ":p0", (int*)  ls->Type, 0);
	OCI_BindArrayOfInts   (st, ":p1", (int*)  ls->Key, 0);
	OCI_BindArrayOfInts   (st, ":p2", (int*)  ls->N_Actions, 0);
	OCI_BindArrayOfInts   (st, ":p3", (int*)  ls->N_MID, 0);
	OCI_BindArrayOfInts   (st, ":p4", (int*)  ls->N_DCountry, 0);
	OCI_BindArrayOfInts   (st, ":p5", (int*)  ls->N_SCountry, 0);
	OCI_BindArrayOfUnsignedInts   (st, ":p6", (unsigned int*)  ls->N_DIP, 0);
	OCI_BindArrayOfUnsignedInts   (st, ":p7", (unsigned int*)  ls->N_SIP, 0);
	OCI_BindArrayOfInts   (st, ":p8", (int*)  ls->N_DPort, 0);
	OCI_BindArrayOfBigInts(st, ":p9", (big_int*)  ls->Packets, 0);
	OCI_BindArrayOfBigInts(st, ":p10", (big_int*)  ls->Bytes, 0);
	OCI_BindArrayOfBigInts(st, ":p11", (big_int*)  ls->Count, 0);

	if (!JP_db->Execute())
	{
		JP_Logger->Log("Could not execute", EXP);
		return false;
	}
	JP_db->Finalize();
	JP_Logger->Log("logging %7d records Tops in %7lu ms.", JP_Trends->GetDBLogSize(), GetTickCount() - lng);
	JP_Trends->FreeDBLists();
	*/
	return true;
}
