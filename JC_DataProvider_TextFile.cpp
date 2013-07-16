/*
 * JC_DataProvider_TextFile.cpp
 *
 *  Created on: Sep 28, 2011
 *      Author: javid
 */

#include "JC_DataProvider_TextFile.h"

JC_DataProvider_TextFile::JC_DataProvider_TextFile()
{

}
//-----------------------------------------------------------------------------
bool JC_DataProvider_TextFile::Start(MI_DataProviderActor* actor)
{
	MP_Actor=actor;
	return true;
}
//-----------------------------------------------------------------------------
void JC_DataProvider_TextFile::End()
{
}
//-----------------------------------------------------------------------------
bool JC_DataProvider_TextFile::BeginRecive()
{
    struct stat st;
    DIR *dp;
    struct dirent *dirp;
    char filename[MCONST_MAXCHARBUF];
    char buff[MCONST_MAXCHARBUF];

    while(true)
    {
		if((dp  = opendir(MP_Actor->Config()->GetConfig()->Provider.SourcePath)) == 0)
		{
			sprintf(filename, "Could not read source path [%s]", MP_Actor->Config()->GetConfig()->Provider.SourcePath);
			MP_Actor->Logger()->Log(filename, EXP);
			return false;
		}
        while ((dirp = readdir(dp)) != NULL) {
        	if (dirp->d_name[0] == '.')
        		continue;

        	sprintf(filename,"%s%s", MP_Actor->Config()->GetConfig()->Provider.SourcePath, dirp->d_name);
        	sprintf(buff,"%s%s", MP_Actor->Config()->GetConfig()->Provider.SourcePath, dirp->d_name);
        	MP_Actor->Logger()->Log(buff);

        	Prepare(filename);
        	unlink(filename);
        	usleep(100000);
        	if (CHECK_PROTGRAM_TERMINATE)
        	    break;
        }
        closedir(dp);
		if (CHECK_PROTGRAM_TERMINATE)
		{
			MS_DataProviderBuffer buffer;
			memset(&buffer, 0, sizeof(MS_DataProviderBuffer));
			buffer.DataProviderType = ME_DataProviderType_File;
			buffer.DeviceIP = 0;
			buffer.LogType = ME_ProviderLogType_Syslog;
			MP_Actor->OnReciveData(&buffer);
			break;
		}
		usleep(30000);

    }
    return true;
}
//-----------------------------------------------------------------------------
void JC_DataProvider_TextFile::Prepare(const char *filename)
{
	MS_DataProviderBuffer buffer;
	memset(&buffer, 0, sizeof(MS_DataProviderBuffer));
	buffer.DeviceIP = 0;
	buffer.DataProviderType = ME_DataProviderType_File;
	FILE * fp = fopen (filename, "r");
	while (!feof(fp))
	{
		memset(buffer.buf, 0, sizeof(buffer.buf));
		if (fgets(buffer.buf, sizeof(buffer.buf) - 1, fp))
			MP_Actor->OnReciveData(&buffer);
	}
	fclose(fp);
}

