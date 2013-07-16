#include "JC_CISCO.h"

JC_CISCO::JC_CISCO() {
	JP_Log = 0;
	JP_Log_Info = 0;
}
//-----------------------------------------------------------------------------
bool JC_CISCO::Start(const char * str)
{
	const char * p;
	JP_LogLength = str?strlen(str):0;
	if ((JP_LogLength > 6) && (str[0]!='<'||str[4]!='>'||str[9]!=':'||str[10]!=' '||str[31]!=':'||str[32]!=' '))
		return false;

	p = str + 33;
	JP_LogLength = strlen(p)+1;
	JP_Log = (char *)JGetMem(JP_LogLength);
	memset(JP_Log, 0, JP_LogLength);
	strncpy(JP_Log, p, JP_LogLength);
	if (!Parse())
		return false;
	return true;
}
//-----------------------------------------------------------------------------
void JC_CISCO::End()
{
	JFreeMem(JP_Log, JP_LogLength);
}
//-----------------------------------------------------------------------------
bool JC_CISCO::Parse()
{
	char * p = JP_Log;
	while (p[0] != ':')
		p++;
	p[0]=0;
	JP_Log_Info = p+2;
	return true;
}
