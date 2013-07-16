/*
 * JC_DataProvider_TextFile.h
 *
 *  Created on: Sep 28, 2011
 *      Author: javid
*/

#ifndef JC_DataProvider_TextFile_H_
#define JC_DataProvider_TextFile_H_

#include "Common.h"

class JC_DataProvider_TextFile: public MI_DataProvider {
	public:
		JC_DataProvider_TextFile();
		JE_Provider_Type GetType() { return JE_Provider_Type_File;};
		bool Start(MI_DataProviderActor* actor);
		void End();
		bool BeginRecive();
	private:
		MI_DataProviderActor* MP_Actor;
		void Prepare(const char *filename);
};

#endif /* JC_DataProvider_TextFile_H_ */
