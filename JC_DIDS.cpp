/*
 * JC_DIDS.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: root
 */

#include "JC_DIDS.h"

JC_DIDS::JC_DIDS() {
}
//-----------------------------------------------------------------------------
bool JC_DIDS::Start(const char * str)
{
	memset(JP_String, 0, sizeof(JP_String));
	memset(JP_items, 0, sizeof(JP_items));
	strncpy(JP_String, str, sizeof(JP_String) - 1);
	return Parse();
}
//-----------------------------------------------------------------------------
void JC_DIDS::End()
{

}
//-----------------------------------------------------------------------------
bool JC_DIDS::Parse()
{
	int i = Split(JP_String, JP_items, ',', 15);
	return (i == 9) && (JP_String[0]== '<') && (JP_String[2]== '>') && (!strcmp(JP_items[1], "dids"));
}
//-----------------------------------------------------------------------------
int JC_DIDS::Split(char * str, char ** result, char ch, int max)
{
	int i = 1;
	char * p = str;
	char sp[2] = {0, 0};
	sp[0] = ch;

	result[0] = p;
	while ((p = strstr(p, sp)) != 0)
	{
		p[0] = 0;
		p++;
		result[i] = p;
		i++;
		if (i == max)
			break;
	}
	return i;
}
//-----------------------------------------------------------------------------
const char * JC_DIDS::Get_Hostname()
{
	char * p;
	p =  JP_items[0];
	return p + 3;
}
//-----------------------------------------------------------------------------
int JC_DIDS::Get_Priority()
{
	char p[100], * q;
	strcpy(p, JP_items[0]);
	q = p;
	q++;
	q[1] = 0;
	return atoi(q);
}
//-----------------------------------------------------------------------------
IP JC_DIDS::Get_SourceIP()
{
	char * p;
	char ip[30];
	memset(ip, 0, 30);
	strncpy(ip, JP_items[3], 30);
	p = strstr(ip, ":");
	if (p)
		p[0] = 0;
	return StrToIP(JP_items[3]);
}
//-----------------------------------------------------------------------------
IP JC_DIDS::Get_DestinationIP()
{
	char * p;
	char ip[30];
	memset(ip, 0, 30);
	strncpy(ip, JP_items[4], 30);
	p = strstr(ip, ":");
	if (p)
		p[0] = 0;
	return StrToIP(JP_items[4]);
}
//-----------------------------------------------------------------------------
int JC_DIDS::Get_SourcePort()
{
	char * p;
	char ip[30];
	memset(ip, 0, 30);
	strncpy(ip, JP_items[3], 30);
	p = strstr(ip, ":");
	return atoi(p + 1);
}
//-----------------------------------------------------------------------------
int JC_DIDS::Get_DestinationPort()
{
	char * p;
	char ip[30];
	memset(ip, 0, 30);
	strncpy(ip, JP_items[4], 30);
	p = strstr(ip, ":");
	return atoi(p + 1);
}
//-----------------------------------------------------------------------------
int JC_DIDS::Get_Protocol()
{
	return atoi(JP_items[5]);
}
//-----------------------------------------------------------------------------
int JC_DIDS::Get_SrcCountry()
{
	return atoi(JP_items[6]);
}
//-----------------------------------------------------------------------------
int JC_DIDS::Get_DstCountry()
{
	return atoi(JP_items[7]);
}
//-----------------------------------------------------------------------------
const char * JC_DIDS::Get_Message()
{
	return JP_items[8];
}

