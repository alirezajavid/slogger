#include "JC_DatabseFactory.h"

MI_Database * GetDataBase(JC_Config* config, JI_CgiLogger * logger, JE_Database_Type  dbType){
	bool b = false;
	JC_DatabseFactory * r;
	MI_Database * db;

	const char * Host = 0;
	const char * User = 0;
	const char * Pass = 0;
	const char * DB = 0;


	switch (dbType)
	{
		case JE_Database_Type_MySQL:
			Host = config->GetDBConfig()->MySQL.Hostname;
			User = config->GetDBConfig()->MySQL.Username;
			Pass = config->GetDBConfig()->MySQL.Password;
			DB   = config->GetDBConfig()->MySQL.DBName;

			r = new JC_DatabseFactoryMySQL();
			db = r->createDB();
			b = ((MC_MySQL *)db)->Start(Host, User, Pass, DB, logger);
			break;
		case JE_Database_Type_Oracle:
			Host = config->GetDBConfig()->Oracle.Hostname;
			User = config->GetDBConfig()->Oracle.Username;
			Pass = config->GetDBConfig()->Oracle.Password;

			r = new JC_DatabseFactoryOracle();
			db = r->createDB();
			b = ((MC_Oracle *)db)->Start(Host, User, Pass, logger);
			break;
		default:;
	}

	if (r)
	{
		delete r;
		r = 0;
	}

	if (!b)
	{
		db->End();
		delete db;
		db = 0;
	}
	return db;
}
