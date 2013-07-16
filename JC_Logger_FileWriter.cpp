/*
 * JC_Logger_FileWriter.cpp
 *
 *  Created on: Sep 27, 2011
 *      Author: javid
 */

#include "JC_Logger_FileWriter.h"

JC_Logger_FileWriter::JC_Logger_FileWriter() {
}
//-----------------------------------------------------------------------------
bool JC_Logger_FileWriter::Start(JI_CgiLogger * logger, JC_Config * config)
{
	JP_Logger = logger;
	JP_Config = config;
	return true;
}
//-----------------------------------------------------------------------------
void JC_Logger_FileWriter::End() {
}
//-----------------------------------------------------------------------------
void JC_Logger_FileWriter::SetNOW()
{
	time_t rawtime;
	time(&rawtime);
	tm * timeinfo = localtime(&rawtime);
	sprintf(JP_NOW, "%04d%02d%02d_%02d%02d%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}
//-----------------------------------------------------------------------------
bool JC_Logger_FileWriter::Log(MS_PreparedDataForLogger* preProcessResult)
{
    SetNOW();
	sprintf(JP_DestinationFile, "%s%s_%s.txt", JP_Config->GetConfig()->Logger.DestinationPath, JP_NOW, "out");

    JP_Logger->Log("FileWriter: Start write [%s] ", JP_DestinationFile);
    JP_fp = fopen(JP_DestinationFile, "w");
    if (!JP_fp)
    {
		JP_Logger->Log("could not make file [%s]", JP_DestinationFile);
		return false;
    }

	WriteFile();
    fclose(JP_fp);

    JP_Logger->Log("FileWriter: End of write [%s]", JP_DestinationFile);
    return true;
}
//-----------------------------------------------------------------------------
const char * JC_Logger_FileWriter::Now()
{
	memset(JP_NOW, 0, 20);
	time_t curtime;
	struct tm *loctime;
	curtime = time (NULL);
	loctime = localtime (&curtime);
	strftime (JP_NOW, 20, "%Y-%m-%d %H:%M:%S", loctime);
	return JP_NOW;
}
//-----------------------------------------------------------------------------
unsigned long int JC_Logger_FileWriter::GetTickCount()
{
	timeb tb;
	ftime( &tb );
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}
//-----------------------------------------------------------------------------
void JC_Logger_FileWriter::WriteFile()
{
	    //fprintf(JP_fp, "%u\t%d\t%d\t%d\n",		    r[i].Duration);
}
