#ifndef JC_HASH_H_
#define JC_HASH_H_

#include "Common.h"
#define JCOONST_JHASH_INITVALUE 1
#define JHASH_GOLDEN_RATIO      0x9e3779b9
#define __jhash_mix(a, b, c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}


struct JS_Hash_Record {
	unsigned long key;
	void * p;
	int SizeOfPointer;
	JS_Hash_Record * next;
};

typedef void (*funcptr)(void *);

class JC_Hash {
	public:
		JC_Hash();
		bool Start(int size, JI_CgiLogger * ALogger, bool KeyInRange);
		void End();
		void * Get(unsigned long key, int sizeOfBlock,void* searchFactor,bool(*com)(void*,void*));
		void * Get(unsigned long key, void* searchFactor,bool(*com)(void*,void*));
		void Clear();
		JS_Hash_Record * GetFirst();
		JS_Hash_Record * GetNext();
		int Size() { return JP_Size; };
		void SetOnFreeNode(void * func);
		int Get_Dist_Pivot() { return JP_Node_Pivot_Count; }
		int Get_Dist_Tails() { return JP_Node_Tail_Count; }
		int Get_VectorSize() { return JP_VectorSize; }
		int Get_MaxTailCnt() { return JP_MaxTail; }
		unsigned int Jhash(const void *key, unsigned  int length, unsigned int initval);
	private:
		int JP_Size;
		int JP_VectorSize;
		JI_CgiLogger * JP_Logger;
		bool JP_KeyInRange;
		funcptr JP_OnFree;
		JS_Hash_Record ** JP_Vector;
		int JP_CurrentIndex;
		JS_Hash_Record * JP_CurrentRecord;

		int JP_Node_Pivot_Count;
		int JP_Node_Tail_Count;
		unsigned int JP_MaxTail;
};




#endif /* JC_HASH_H_ */
