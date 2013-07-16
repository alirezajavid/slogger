#ifndef IP_H_
#define IP_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>

typedef unsigned int IP;

IP StrToIP(const char * str);
bool IP_in_Range(IP ip, IP start, IP end);
void IPToStr(IP ip, char * str);
void * JGetMem(unsigned int i);
void JFreeMem(void * p, int i);
unsigned long int GetTickCount();
unsigned long  getKeyTime();
int Split(char * str, char ** result, char ch);
void swap(int &n1, int &n2);
unsigned long UsedMemory();
bool fileExists(const char * filename);

#endif /* IP_H_ */
