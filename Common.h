/*
 * Common.h
 *
 *  Created on: Sep 27, 2011
 *      Author: javid
 */

#ifndef COMMON_H_
#define COMMON_H_
#pragma pack(1)
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include <queue>
#include <vector>
#include <setjmp.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <wait.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include "ip.h"

#define MCONST_PREPARED_BUFF_SIZE1 40000
#define MCONST_PROVIDER_BUFF_SIZE 1024

#define MCONST_MAXCHARBUF 500
#define MCONST_MAXBUF 1025*300
#define MCONST_MAX_FILEPATH 1024
#define MCONST_MINCHARBUF 50
#define IN
#define OUT
#define MCONST_BUFF_SIZE 1024
#define MCONST_SHORTSTRING 50

#define MCONST_CONFIG_FILE 		 "/var/log/soc_logger/config.ini"
#define MCONST_DB_INTERVAL 10
#define JCONST_MAX_PROTOCLNUMBER	100
#define JCONST_MAX_GEONUMBER		300
#define JCONST_MAX_RULES		100
#define JCONST_MAX_DEVICEINSECTION	10
#define JCONST_MAX_NODE_NUMBER		30
#define CHECK_PROTGRAM_TERMINATE	stat("/var/log/soc_logger/Logger.kill",&st) != -1

#define DB_VALTYPE_BYTE_RECEIVE		1
#define DB_VALTYPE_BYTE_SEND		2
#define DB_VALTYPE_BYTE_TOTAL		3
#define DB_VALTYPE_PACKET_RECEIVE	4
#define DB_VALTYPE_PACKET_SEND		5
#define DB_VALTYPE_PACKET_TOTAL		6
#define DB_VALTYPE_DETAIL		7
#define DB_VALTYPE_SESSIONCOUNT		8

#define DB_VALTYPE_IP_SRC		1
#define DB_VALTYPE_IP_DST		0

#define BUFLEN 1800
#define EXP __FILE__,__FUNCTION__,__LINE__

#define handle_error_en(en, msg) \
	   do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
	   do { perror(msg); exit(EXIT_FAILURE); } while (0)

enum ME_DataBaseFieldType {
	ME_DataBaseFieldType_Int,
	ME_DataBaseFieldType_Double,
	ME_DataBaseFieldType_Text,
	ME_DataBaseFieldType_Blob,
	ME_DataBaseFieldType_Bool,
	ME_DataBaseFieldType_UnKnown,
	ME_DataBaseFieldType_DateTime,
	ME_DataBaseFieldType_Max
};
//----------------------------------------------------------------------------------------
enum JE_Database_Type {
	JE_Database_Type_None,
	JE_Database_Type_MySQL,
	JE_Database_Type_Oracle
};
//----------------------------------------------------------------------------------------
struct  JS_Database_Properties {
	struct {
		char Username[MCONST_MINCHARBUF];
		char Password[MCONST_MINCHARBUF];
		char Hostname[MCONST_MINCHARBUF];
		char DBName[MCONST_MINCHARBUF];
	} MySQL;
	struct {
		char Username[MCONST_MINCHARBUF];
		char Password[MCONST_MINCHARBUF];
		char Hostname[MCONST_MINCHARBUF];
	} Oracle;
};
//----------------------------------------------------------------------------------------
enum JE_Provider_Type {
	JE_Provider_Type_None,
	JE_Provider_Type_UDP,
	JE_Provider_Type_File,
	JE_Provider_Type_PCap,
	JE_Provider_Type_MAX
};
enum JE_Logger_Type {
	JE_Logger_Type_None,
	JE_Logger_Type_FileWriter,
	JE_Logger_Type_DatabaseWriter,
	JE_Logger_Type_MAX
};

struct JS_App_Config {
	struct {
	    JE_Provider_Type Type;
	    char SourcePath[MCONST_MAXCHARBUF];
	    char PCapFileName[MCONST_MAXCHARBUF];
	} Provider;

	struct {
		bool Syslog;
	} PreProcessors;

	struct {
		bool FileWrite;
		bool DatabaseWrite;
		char DestinationPath[MCONST_MAXCHARBUF];
		int  BulkSize;
		unsigned int  WriteRateStatisticsInterval;
		unsigned int  WriteTopStatisticsInterval;
	} Logger;

	struct {
		int DeleteBeforeHours;
	} General;
};
//----------------------------------------------------------------------------------------
class MI_DataBaseRow
{
	public:
		virtual int GetFiledCount()=0;
		virtual const char* GetFieldName(int col)=0;
		virtual int GetFieldIndex(const char* name)=0;
		virtual ME_DataBaseFieldType GetFieldType(int col)=0;
		virtual ME_DataBaseFieldType GetFieldType(const char* name)=0;

		virtual int 		GetFieldInt(int col)=0;
		virtual int 		GetFieldInt(const char* name)=0;
		virtual const char* GetFieldText(int col)=0;
		virtual const char* GetFieldText(const char* name)=0;
		virtual double 		GetFieldDouble(int col)=0;
		virtual double 		GetFieldDouble(const char* name)=0;
		virtual const void* GetFieldBlob(int col, int size)=0;
		virtual const void* GetFieldBlob(const char* name, int size)=0;
		virtual const void* GetFieldDateTime(int col, int size)=0;
		virtual const void* GetFieldDateTime(const char* name, int size)=0;
		virtual IP 			GetFieldIP(int index)=0;
};

class MI_DataBaseEngine
{
	public:
		virtual void End()=0;
		virtual const char* GetMessage()=0;
		virtual long long int GetLastRowInsertedId()=0;
		virtual bool PreperSQL(const char* sql)=0;
		virtual int Query(const char* buf)=0;//non bulk execute command
		virtual MI_DataBaseRow * Read()=0;
		virtual bool Finalize()=0;
		virtual bool IsTableExistist(const char* tableName)=0;
		virtual bool Execute()=0;//bulk execute command
		virtual bool Roollback()=0;
		virtual bool BindInt(int index,int value)=0;
		virtual bool BindText(int index,const char* value)=0;
		virtual bool BindBlob(int index,void* data,int size)=0;
		virtual bool BindNull(int index)=0;
		virtual bool BindDouble(int index,double value)=0;
		virtual bool BindDateTime(int index,const char* value)=0;
		virtual bool BindIP(int index,IP value)=0;
		virtual bool BindLong(int index,long value)=0;
		virtual bool BeginBulkTransaction(const char* sql) = 0;
		virtual bool EndBulkTransaction() = 0;

		virtual bool BeginTransaction() = 0;
		virtual bool EndTransaction() = 0;
};

class MI_Database: public MI_DataBaseRow, MI_DataBaseEngine {
	public:
		// engine
		virtual void End()=0;
		virtual const char* GetMessage()=0;
		virtual long long int GetLastRowInsertedId()=0;
		virtual bool PreperSQL(const char* sql)=0;
		virtual int Query(const char* buf)=0;//non bulk execute command
		virtual MI_DataBaseRow * Read()=0;
		virtual bool Finalize()=0;
		virtual bool IsTableExistist(const char* tableName)=0;
		virtual bool Execute()=0;//bulk execute command
		virtual bool Roollback()=0;
		virtual bool BindInt(int index,int value)=0;
		virtual bool BindText(int index,const char* value)=0;
		virtual bool BindBlob(int index,void* data,int size)=0;
		virtual bool BindNull(int index)=0;
		virtual bool BindDouble(int index,double value)=0;
		virtual bool BindDateTime(int index,const char* value)=0;
		virtual bool BindIP(int index,IP value)=0;
		virtual bool BindLong(int index,long value) = 0;

		virtual bool BeginBulkTransaction(const char* sql) = 0;
		virtual bool EndBulkTransaction() = 0;

		virtual bool BeginTransaction() = 0;
		virtual bool EndTransaction() = 0;


		// row
		virtual int GetFiledCount()=0;
		virtual const char* GetFieldName(int col)=0;
		virtual int GetFieldIndex(const char* name)=0;
		virtual ME_DataBaseFieldType GetFieldType(int col)=0;
		virtual ME_DataBaseFieldType GetFieldType(const char* name)=0;

		virtual int 		GetFieldInt(int col)=0;
		virtual int 		GetFieldInt(const char* name)=0;
		virtual const char* GetFieldText(int col)=0;
		virtual const char* GetFieldText(const char* name)=0;
		virtual double 		GetFieldDouble(int col)=0;
		virtual double 		GetFieldDouble(const char* name)=0;
		virtual const void* GetFieldBlob(int col, int size)=0;
		virtual const void* GetFieldBlob(const char* name, int size)=0;
		virtual const void* GetFieldDateTime(int col, int size)=0;
		virtual const void* GetFieldDateTime(const char* name, int size)=0;
		virtual IP 			GetFieldIP(int index)=0;

};
//----------------------------------------------------------------------------------------
class JI_CgiLogger
{
	public:
		virtual void Log(const char* message,...)=0;
		virtual void Log(const char *message, const char*  FileName,const char*  FuncName, int Line)=0;
};
//----------------------------------------------------------------------------------------
enum ME_DataProviderType
{
	ME_DataProviderType_UDP,
	ME_DataProviderType_Pcap,
	ME_DataProviderType_File,
	ME_DataProviderType_None
};
//----------------------------------------------------------------------------------------
enum ME_ProviderLogType
{
	ME_ProviderLogType_Syslog,
	ME_ProviderLogType_None
};
//----------------------------------------------------------------------------------------
enum JE_Preprocessor_Type {
	JE_Preprocessor_Type_Syslog,
	JE_Preprocessor_Type_None
};

enum JE_Log_Type
{
	JE_Log_Type_None,
	JE_Log_Type_MacAfee_Vulnerability,
	JE_Log_Type_MacAfee_Openport,
	JE_Log_Type_Tripwire,
	JE_Log_Type_Sophos,
	JE_Log_Type_ACS,
	JE_Log_Type_Cisco,
	JE_Log_Type_DIDS,
	JE_Log_Type_Max
};

struct JS_DIDS {
	int MID,SID,SENSORID,PROTOCOL,SIP,DIP,SPORT,DPORT,SCOUNTRY,DCOUNTRY,ACTIONID,PACKET,BYTE;
	int ssize;
	int size;
	int pcount;
	int tcprate;
	int udprate;
	int icmprate;
	int orate;
	int serate;
	int usrate;
	int secount;
	int uscount;
	int tcpratep;
	int udpratep;
	int icmpratep;
	int otherratep;
	char anomaliy;
	char direction;
};

union JS_Log_Arcsight {
	struct {
		char DeviceVendor[50];
		char DeviceProduct[50];
		char DeviceVersion[50];
		char SignatureID[50];
		char Name[50];
		char Severity[20];
		char dst[20];
		char dntdom[20];
		char object[40];
		char os[40];
		char oh[40];
		char ahost[20];
		char agt[20];
		char av[20];
		char ad_netbiosname[20];
		int CVECount;
		char CVE[10][20];
		unsigned char SeverityN;
	} vulnerability;

	struct {
		char DeviceVendor[MCONST_MINCHARBUF];
		char DeviceProduct[MCONST_MINCHARBUF];
		char DeviceVersion[MCONST_MINCHARBUF];
		char SignatureID[MCONST_MINCHARBUF];
		char Name[MCONST_MINCHARBUF];
		char Severity[20];
		char dst[20];
		char dntdom[20];
		char object[40];
		char os[40];
		char oh[40];
		char ahost[20];
		char agt[20];
		char av[20];
		char ad_netbiosname[20];
		char prot[5];
		char dproc[50];
		unsigned int dport;
	} openport;

	struct {
		char Version[20];
		char Name[40];
		char Severity[15];
		char dHost[20];
		char Domain[20];
		char FilePath[200];
	} sophos;

	struct {
		char Version[20];
		char Severity[20];
		char Msg[200];
		char Cat[100];
		char DeviceSeverity[20];
		char Dst[20];
		char DntDom[50];
		char DUID[20];
		char CallerId[50];
		char AuthorFaulureCode[200];
		char AuthorData[200];
		char AHost[20];
		char Agt[20];
		char Av[20];
		char DVC[20];
	} ACS;

	struct {
		char Rulename[50];
		char Object[50];
		char dHost[40];
		char dst[20];
		char AHost[20];
		char Agt[20];
		char Av[20];
		char Message[200];
		char dvcHost[40];
		char dvc[40];
		char sHost[20];
		char sUser[20];
		char src[20];
		char deviceSeverity[20];
		char act[20];
		char cat[30];
	} Tripwire;

	struct {
		char IP[20];
		char Category[50];
		char Message[500];
	} Cisco;

	JS_DIDS DIDS;
};
//----------------------------------------------------------------------------------------
struct MS_DataProviderBuffer
{
	char buf[MCONST_PROVIDER_BUFF_SIZE];
	IP DeviceIP;
	ME_DataProviderType DataProviderType;
	ME_ProviderLogType LogType;
};
//----------------------------------------------------------------------------------------
struct MS_PreparedDataForLogger
{
	IP ip;
	JE_Log_Type LogType;
	JS_Log_Arcsight log;
};
//----------------------------------------------------------------------------------------
class JI_Config{
	public:
		virtual JS_Database_Properties * GetDBConfig() = 0;
		virtual JS_App_Config * GetConfig() = 0;
};
//----------------------------------------------------------------------------------------
class MI_DataProviderActor
{
	public:
		virtual JI_Config * Config() = 0;
		virtual JI_CgiLogger * Logger()= 0;
		virtual void OnReciveData(MS_DataProviderBuffer* buff)=0;
};
//----------------------------------------------------------------------------------------
class MI_DataProvider
{
	public:
	virtual JE_Provider_Type GetType()=0;
	virtual bool BeginRecive()=0;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class MI_Logger
{
	public:
		virtual JE_Logger_Type GetType()=0;
		virtual bool Log(MS_PreparedDataForLogger* preProcessResult)=0;
};
//----------------------------------------------------------------------------------------
class MI_LogManager
{
	public:
		virtual void AddLogger(MI_Logger* in)=0;
		virtual bool Log(MS_PreparedDataForLogger*  preProcessResult)=0;

};
//----------------------------------------------------------------------------------------
class MI_Preprocesseor
{
	public:
		virtual JE_Preprocessor_Type GetType() = 0;
		virtual bool Process(MS_DataProviderBuffer* inBuf,
				     OUT MS_PreparedDataForLogger* buffer,
				     OUT int & PreparedDataCount)=0;
};
//----------------------------------------------------------------------------------------
class JI_CEF {
	public:
		virtual int GetVersion() = 0;
		virtual const char * DeviceVendor() = 0;
		virtual const char * DeviceProduct() = 0;
		virtual const char * DeviceVersion() = 0;
		virtual const char * SignatureID() = 0;
		virtual const char * Name() = 0;
		virtual const char * Severity() = 0;
		virtual const char * GetExtensionByName(const char * name) = 0;
		virtual const char * GetExtensionKeyByValue(const char * value) = 0;
		virtual const char * GetValueFromLabeledProperty(const char * value) = 0;
};
//----------------------------------------------------------------------------------------
class JI_Preprocesseor_CEF
{
	public:
		virtual bool Convert(JI_CEF *, MS_PreparedDataForLogger *) = 0;
};

#endif /* COMMON_H_ */
