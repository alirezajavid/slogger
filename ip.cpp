#include "ip.h"

IP StrToIP(const char * str) {
    union {
    	unsigned char section[4];
    	IP ip;
    } ip;

    char temp[40]={'0'};
    int size=strlen(str);

    int i,j,k;
    if(!size)
    	return 0;

	for(i=0,j=0,k=3;i<=size;i++)
		if(str[i]!='.' && str[i]!='\0')
			temp[j++]=str[i];
		else
		{
			temp[j]='\0';
			ip.section[k--]=atoi(temp);
			j=0;
		}
    return  ip.ip;
};

void IPToStr(IP ip, char * str) {
	union {
		IP ip;
		struct {
			unsigned char a1,a2,a3,a4;
		};
	} v;
	v.ip = ip;
	sprintf(str, "%d.%d.%d.%d", v.a4, v.a3, v.a2, v.a1);
};

bool IP_in_Range(IP ip, IP start, IP end)
{
	return ip <= end && start <= ip;
}

unsigned long __UsedMemory = 0;

void * JGetMem(unsigned int i)
{
	if (i == 0)
	    return 0;
	void * r=0;
	r = malloc(i);
	if (r)
	{
		__UsedMemory +=i;
		memset(r, 0, i);
	}
	else
	{
		fprintf(stderr, "Could not get Memory UsedMemory  = %7.2fMB Need Memory = %u B\n", (double)(__UsedMemory/(1024*1024)), i);
		fflush(stderr);
	}
	return r;
}

void JFreeMem(void * p, int i)
{
	memset(p, 0, i);
	free(p);
	__UsedMemory -= i;
}

unsigned long int GetTickCount()
{
	timeb tb;
	ftime( &tb );
	return tb.millitm + (tb.time & 0xfffff) * 1000;
}

int Split(char * str, char ** result, char ch)
{
	int i = 1;
	char * p = str;
	char sp[2] = {0, 0};
	sp[0] = ch;

	result[0] = p;
	while ((p = strstr(p, sp)) != NULL)
	{
		p[0] = 0;
		p++;
		result[i] = p;
		i++;
	}
	return i;
}

void swap(int &n1, int &n2) // Note the &
{
     int temp=n1; // Note use the type T
     n1=n2;
     n2=temp;
}

unsigned long UsedMemory()
{
	return __UsedMemory;
}

