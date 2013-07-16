#include "Common.h"
#ifndef __MC_IniParser_Included__
#define __MC_IniParser_Included__
class MC_IniParser
{
	public:
		MC_IniParser(void);
		~MC_IniParser(void);
		bool Start(const char* fileName);
		bool End();
		const char* GetVal(const char* sectionName,const char* valName, const char * defaultValue);
		const char* GetVal(int index){return MP_Vals.at(index)->value;}
		char* GetName(int index){return MP_Vals.at(index)->name;}
		char* GetSectionName(int index){return MP_Vals.at(index)->sectionName;}
		void WriteValue(const char * section,const char * name,const char * value);
		int GetCount(){ return MP_Vals.size(); }
		void WriteFile();

	private://structs
		struct MS_IniVal{
			char name[MCONST_MINCHARBUF];
			char value[MCONST_MAXCHARBUF*2];
			char sectionName[MCONST_MINCHARBUF];
		};

	private: //vals
		std::vector<MS_IniVal*> MP_Vals;
		char MP_CurrentSectionName[MCONST_MINCHARBUF];
		char MP_FileName[MCONST_MAXCHARBUF];
	private://functions
		bool ParseFile(const char* fileName);
		bool ReadData(char* buf,int size);
		void AddDataToSection(MS_IniVal* val);
		void ReadWord(char* buf,char* outBuf);
		void RemoveSpace(char* buf);
	};
#endif
