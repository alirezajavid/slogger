#ifndef JC_DATABSEFACTORY_H_
#define JC_DATABSEFACTORY_H_

#include "Common.h"
#include "JC_Config.h"
#include "MC_MySQL.h"
#include "MC_Oracle.h"


class JC_DatabseFactory {
	public:
 		virtual	MI_Database * createDB()=0;
};

class JC_DatabseFactoryMySQL: public JC_DatabseFactory {
	public:
 		MI_Database * createDB(){
 			return new MC_MySQL();
 		};
};

class JC_DatabseFactoryOracle: public JC_DatabseFactory {
	public:
 		virtual	MI_Database * createDB(){
 			return new MC_Oracle();
 		};
};

MI_Database * GetDataBase(JC_Config* config, JI_CgiLogger * logger, JE_Database_Type dbType);

enum JE_DataProvider_FieldsType {
	JE_DataProvider_FieldsTyeText,
	JE_DataProvider_FieldsTyeNumber,
	JE_DataProvider_FieldsTyeDateTime,
	JE_DataProvider_FieldsTyeDate,
	JE_DataProvider_FieldsTyeIP,
	JE_DataProvider_FieldsTyeCountry,
	JE_DataProvider_FieldsTyeApplication,
	JE_DataProvider_FieldsTyeProtocol,
	JE_DataProvider_FieldsTyeMAC,
	JE_DataProvider_FieldsTyeBool,
	JE_DataProvider_FieldsTyeMAX
};


#endif /* JC_DATABSEFACTORY_H_ */
