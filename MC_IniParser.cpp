#include "MC_IniParser.h"
//-----------------------------------------------------------------------------------------------
MC_IniParser::MC_IniParser(void)
{

}
//-----------------------------------------------------------------------------------------------
MC_IniParser::~MC_IniParser(void)
{
}
//-----------------------------------------------------------------------------------------------
bool MC_IniParser::Start(const char* fileName)
{
	if(!ParseFile(fileName))
		return false;

	return true;
}
//-----------------------------------------------------------------------------------------------
const char* MC_IniParser::GetVal(const char* sectionName,const char* valName, const char * defaultValue)
{
	std::vector<MS_IniVal*>::iterator it;
	char* name=0;
	char* seName=0;
	for(it=MP_Vals.begin();it!=MP_Vals.end();it++)
	{
		name=(*it)->name;
		seName=(*it)->sectionName;
		if(!strcmp(sectionName,seName) && !strcmp(valName,name))
			return (*it)->value;
	}
	return defaultValue;
}
//-----------------------------------------------------------------------------------------------
bool MC_IniParser::End()
{
	std::vector<MS_IniVal*>::iterator it;
	for(it=MP_Vals.begin();it!=MP_Vals.end();it++)
	{
		JFreeMem(*it, sizeof(MS_IniVal));
		(*it) = 0;
	}

	MP_Vals.clear();
	return true;
}
//-----------------------------------------------------------------------------------------------
bool MC_IniParser::ParseFile(const char* fileName)
{
	bool b = true;
	strcpy(MP_FileName, fileName);
	FILE* MP_File;
	MP_File =fopen(fileName,"r");
	if(!MP_File)
		return false;
	int size=MCONST_MAXCHARBUF;
	char* buf=(char*)JGetMem(size);
	if(!buf)
		return false;

	while(b && fgets(buf,size,MP_File))
		b = b && ReadData(buf,strlen(buf));

	JFreeMem(buf, size);
	buf = 0;
	fclose(MP_File);
	return b;
}
//-----------------------------------------------------------------------------------------------
bool MC_IniParser::ReadData(char* buf,int size)
{
	char* eqPos=0;
	char* tempPos=0;
	MS_IniVal* val=0;

	int jumpSize=0;

	while(*buf!='\n')
	{
		if(*buf=='[')
		{
			tempPos=strstr(buf,"]");
			if(!tempPos)
				return false;

			jumpSize=tempPos - buf;
			memset(MP_CurrentSectionName,'\0',MCONST_MINCHARBUF );
			strncpy(MP_CurrentSectionName,buf+1,jumpSize-1);

			break;
		}

		if(*buf >='0' && *buf <='z')
		{
			val=(MS_IniVal*)JGetMem(sizeof(MS_IniVal));
			if (!val)
				return false;

			memset(val,'\0',sizeof(MS_IniVal));
			strcpy(val->sectionName ,MP_CurrentSectionName);
			ReadWord(buf,val->name);

			MP_Vals.push_back(val);
			//read value
			eqPos=strstr(buf,"=");
			if(!eqPos)
				break;

			//remove front space
			do
				eqPos++;
			while(*eqPos == ' ');

			ReadWord(eqPos,val->value);
			break;
		}
		buf++;
	}

	return true;
}
//-----------------------------------------------------------------------------------------------
void MC_IniParser::WriteValue(const char * section,const char * name,const char * value)
{
	bool Exists=false;
	std::vector<MS_IniVal*>::iterator it;
	for(it=MP_Vals.begin();it!=MP_Vals.end();it++)
	{
		if(!strcmp((*it)->sectionName,section) && !strcmp((*it)->name, name))
		{
			strcpy ((*it)->value, value);
			Exists = true;
		}
	}

	if (!Exists)
	{
		MS_IniVal * val=(MS_IniVal*)JGetMem(sizeof(MS_IniVal));
		if (!val)
			return ;
		memset(val,'\0',sizeof(MS_IniVal));
		strcpy(val->sectionName ,section);
		strcpy(val->name ,name);
		strcpy(val->value,value);
		MP_Vals.push_back(val);
	}
}
//-----------------------------------------------------------------------------------------------
void MC_IniParser::WriteFile()
{
	FILE* file;
	file =fopen(MP_FileName, "w");
	if(!file)
		return ;

	char * section=0;
	std::vector<MS_IniVal*>::iterator it;
	std::vector<MS_IniVal*>::iterator nt;
	for(it=MP_Vals.begin();it!=MP_Vals.end();it++)
	{
		if (!section || strcmp(section, (*it)->sectionName))
		{
			bool old=false;
			for(nt= MP_Vals.begin();nt != it; nt++)
				if (!strcmp((*it)->sectionName, (*nt)->sectionName))
					old=true;
			if (old)
				continue;

			section = (*it)->sectionName;
			fprintf(file, "[%s]\n", section);
			for(nt= it;nt != MP_Vals.end(); nt++)
				if (!strcmp(section, (*nt)->sectionName))
					fprintf(file, "%s=%s\n", (*nt)->name, (*nt)->value);
			fprintf(file, "\n");
		}

	}
	fclose(file);
	return ;
}
//-----------------------------------------------------------------------------------------------
void MC_IniParser::ReadWord(char* buf,char* outBuf)
{
	char* temp=outBuf;
	while( *buf!='=' && *buf!= 13 && *buf!= 10 && *buf!='\0')
	{
		*outBuf=*buf;
		buf++;
		outBuf++;
	}

	RemoveSpace(temp);
}
//-----------------------------------------------------------------------------------------------
void MC_IniParser::RemoveSpace(char* buf)
	{
		int size=strlen(buf);
		char temp[MCONST_MAXCHARBUF]={'\0'};

//remove front space
		for(int i=0;i<size;i++)
		{
			if(buf[i]!=' ' )
			{
				strcpy(temp,&buf[i]);
				break;
			}
		}

//remove back space

		for(int i=size-1;i>=0;i--)
		{
			temp[i+1]='\0';
			if(buf[i]!=' ')
				break;
		}
		strcpy(buf,temp);
	}
//-----------------------------------------------------------------------------------------------

