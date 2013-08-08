#include "JC_CEF.h"

JC_CEF::JC_CEF() {
	memset(JP_CEFItems, 0, sizeof(JP_CEFItems));
	JP_LogLength = 0;
	JP_Log = 0;
}
//-----------------------------------------------------------------------------
bool JC_CEF::Start(const char * str)
{
	if ((str && (strlen(str) > 4)) && (str[0]!='C'||str[1]!='E'||str[2]!='F'||str[3]!=':'))
		return false;

	JP_LogLength = strlen(str) + 1;
	JP_Log = (char *)JGetMem(JP_LogLength);
	strncpy(JP_Log, str, strlen(str));
	if (!Parse())
		return false;
	return true;
}
//-----------------------------------------------------------------------------
void JC_CEF::End()
{
	for (int i=JP_Extension.size() - 1; i >= 0; i--)
	{
		if (JP_Extension.at(i)->Value)
			JFreeMem(JP_Extension.at(i)->Value, JP_Extension.at(i)->SizeOfValue);
		JFreeMem(JP_Extension.at(i), sizeof(JP_CEFExtentionPair));
	}
	JP_Extension.clear();
	if (JP_Log)
		JFreeMem(JP_Log, JP_LogLength);
}
//-----------------------------------------------------------------------------
int JC_CEF::GetVersion()
{
	return atoi(JP_CEFItems[0]+4);
}
//-----------------------------------------------------------------------------
const char * JC_CEF::DeviceVendor()
{
	return JP_CEFItems[1];

}
//-----------------------------------------------------------------------------
const char * JC_CEF::DeviceProduct()
{
	return JP_CEFItems[2];
}
//-----------------------------------------------------------------------------
const char * JC_CEF::DeviceVersion()
{
	return JP_CEFItems[3];

}
//-----------------------------------------------------------------------------
const char * JC_CEF::SignatureID()
{
	char * p = JP_CEFItems[4];
	return p + 8;
}
//-----------------------------------------------------------------------------
const char * JC_CEF::Name()
{
	return JP_CEFItems[5];
}
//-----------------------------------------------------------------------------
const char * JC_CEF::Severity()
{
	return JP_CEFItems[6];
}
//-----------------------------------------------------------------------------
const char * JC_CEF::GetExtensionByName(const char * name)
{
	for (unsigned int i=0; i< JP_Extension.size(); i++)
		if (!strcmp(name, JP_Extension.at(i)->Name))
			return JP_Extension.at(i)->Value;
	return "";
}
//-----------------------------------------------------------------------------
const char * JC_CEF::GetExtensionKeyByValue(const char * value)
{
	for (unsigned int i=0; i< JP_Extension.size(); i++)
		if (!strcmp(value, JP_Extension.at(i)->Value))
			return JP_Extension.at(i)->Name;
	return "";
}
//-----------------------------------------------------------------------------
bool JC_CEF::Parse()
{
	JP_CEFExtentionPair * pair;
	int i = 1;
	char * p = JP_Log;
	char * q;

	JP_CEFItems[0] = JP_Log;
	while ((p = strstr(p, "|")) != NULL)
	{
		if (p[-1] == '\\')
		{
			p +=2;
			continue;
		}
		p[0] = 0;
		p++;
		JP_CEFItems[i] = p;
		i++;
		if (i == 8)
			break;
	}
	if (i!=8)
		return false;


	for (int j=0; j<8; j++)
		while ((p = strstr(JP_CEFItems[j], "\\|")) != NULL)
			strcpy(p, p+1);


	p = JP_CEFItems[7];
	bool endofstring;
	while ((p = strstr(p, "=")) != NULL)
	{
		if (p[-1] == '\\')
		{
			p +=2;
			continue;
		}

		pair = (JP_CEFExtentionPair *)JGetMem(sizeof(JP_CEFExtentionPair));
		memset(pair, 0, sizeof(JP_CEFExtentionPair));

		p[0] = 0;
		q = p - 1;
		while (q[0] != 0 && q[0] != ' ' && strcmp(q + 1, JP_CEFItems[7]))
			q--;
		strcpy(pair->Name, (q[0]== ' ' || q[0]== 0)?q+1:q);
		q = p + 1;

		while ((q[0] != 0)  &&  !((q[0] == '=') && (q[-1] != '\\')))
			q++;

		endofstring = q[0] == 0;
		if (!endofstring)
		{
			while (q[0] != ' ')
				q--;
			q[0] = 0;
		}

		pair->SizeOfValue = strlen(p + 1) + 1;
		pair->Value = (char *)JGetMem(pair->SizeOfValue);
		memset(pair->Value, 0, strlen(p + 1) + 1);
		strcpy(pair->Value, p + 1);
		JP_Extension.push_back(pair);

		if (endofstring)
			break;
		else
			p = q + 1;
	}
	return true;
}
//-----------------------------------------------------------------------------
const char * JC_CEF::GetValueFromLabeledProperty(const char * value)
{
	char Label[100];
	char * p;
	memset(Label, 0, 100);
	strcpy(Label, GetExtensionKeyByValue(value));
	p = strstr(Label, "Label");
	if (!p)
		return "";
	p[0] = 0;
	return GetExtensionByName(Label);
}
