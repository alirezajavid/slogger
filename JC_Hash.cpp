#include "JC_Hash.h"

JC_Hash::JC_Hash() {
}
//-----------------------------------------------------------------------------
bool JC_Hash::Start(int size, JI_CgiLogger * ALogger, bool KeyInRange)
{
	JP_KeyInRange = KeyInRange;
	JP_Logger = ALogger;
	JP_VectorSize = size;
	JP_OnFree = 0;
	JP_Vector = (JS_Hash_Record **)JGetMem(sizeof(JS_Hash_Record *) * JP_VectorSize);
	if (!JP_Vector)
	{
		JP_Logger->Log("Memory ERROR");
		return false;
	}
	memset(JP_Vector, 0, sizeof(JS_Hash_Record *) * JP_VectorSize);
	JP_Size = 0;
	JP_Node_Pivot_Count = 0;
	JP_Node_Tail_Count = 0;
	JP_MaxTail = 0;
	JP_CurrentIndex = 0;
	JP_CurrentRecord = 0;
	return true;
}
//-----------------------------------------------------------------------------
void JC_Hash::End()
{
	Clear();
	JFreeMem(JP_Vector, sizeof(JS_Hash_Record *) * JP_VectorSize);
	JP_Vector = 0;
}
//-----------------------------------------------------------------------------
JS_Hash_Record * JC_Hash::GetFirst()
{
	JP_CurrentRecord = 0;
	for (JP_CurrentIndex=0; JP_CurrentIndex<JP_VectorSize; JP_CurrentIndex++)
		if (JP_Vector[JP_CurrentIndex])
		{
			JP_CurrentRecord = JP_Vector[JP_CurrentIndex];
			break;
		}
	return JP_CurrentRecord;
}
//-----------------------------------------------------------------------------
JS_Hash_Record * JC_Hash::GetNext()
{
	if (!JP_CurrentRecord)
		return 0;
	if (JP_CurrentRecord->next)
	{
		JP_CurrentRecord = JP_CurrentRecord->next;
		return JP_CurrentRecord;
	}
	JP_CurrentRecord = 0;
	for (JP_CurrentIndex++; JP_CurrentIndex<JP_VectorSize; JP_CurrentIndex++)
		if (JP_Vector[JP_CurrentIndex])
		{
			JP_CurrentRecord = JP_Vector[JP_CurrentIndex];
			break;
		}
	return JP_CurrentRecord;
}
//-----------------------------------------------------------------------------
void * JC_Hash::Get(unsigned long key, int sizeOfBlock,void* searchFactor,bool(*com)(void*,void*))
{
	unsigned int k;
	unsigned int till_count;
	if (!JP_KeyInRange)
	    k = Jhash(&key, sizeof(key), JCOONST_JHASH_INITVALUE);
	else
	    k = key;
	k = k % JP_VectorSize;

	JS_Hash_Record * q = JP_Vector[k];
	till_count = 0;
	while (q)
		if (q->key == key)
			if(com)
				if (com(q->p,searchFactor))
					return q->p;
				else
				{
				    q = q->next;
				    till_count++;
				}
			else
				return q->p;
		else
		{
		    q = q->next;
		    till_count++;
		}

	if (till_count> JP_MaxTail)
	    JP_MaxTail = till_count;

	if (!JP_Vector[k])
	    JP_Node_Pivot_Count++;
	else
	    JP_Node_Tail_Count++;

	JS_Hash_Record * p = (JS_Hash_Record *)JGetMem(sizeof(JS_Hash_Record));
	if (!p)
	{
		JP_Logger->Log("Memory ERROR");
		return 0;
	}

	p->key = key;
	if (sizeOfBlock)
	{
		p->SizeOfPointer = sizeOfBlock;
		p->p = (void *)JGetMem(sizeOfBlock);
		if (!p->p)
		{
			JP_Logger->Log("Memory ERROR");
			JFreeMem(p, sizeof(JS_Hash_Record));
			return 0;
		}
		memset(p->p, 0, sizeOfBlock);
	}
	else
		p->p = 0;

	p->next = JP_Vector[k];
	JP_Vector[k] = p;
	JP_Size++;
	return p->p;
}
//-----------------------------------------------------------------------------
void * JC_Hash::Get(unsigned long key, void* searchFactor,bool(*com)(void*,void*))
{
	unsigned int k = 0;
	k = Jhash(&key, sizeof(key), JCOONST_JHASH_INITVALUE);
	k = k % JP_VectorSize;
	JS_Hash_Record * q = 0;
	q = JP_Vector[k];
	while (q)
		if (q->key == key)
			if(com)
				if (com(q->p,searchFactor))
				{
					return q->p;
				}
				else
					q=q->next;
			else
			{
				return q->p;
			}
		else
			q = q->next;

	return 0;
}
//-----------------------------------------------------------------------------
void JC_Hash::Clear()
{
	JS_Hash_Record * p = GetFirst();
	JS_Hash_Record * q = 0;
	while (p) {
		if (JP_OnFree)
			(*JP_OnFree)(p->p);
		q = p;
		p = GetNext();
		JFreeMem(q->p, q->SizeOfPointer);
		JFreeMem(q, sizeof(JS_Hash_Record));
		q->p = 0;
		q = 0;
	}
	memset(JP_Vector, 0, sizeof(JS_Hash_Record *) * JP_VectorSize);
	JP_Size = 0;
	JP_Node_Pivot_Count = 0;
	JP_Node_Tail_Count = 0;
	JP_MaxTail = 0;
}
//-----------------------------------------------------------------------------
void JC_Hash::SetOnFreeNode(void * func)
{
	JP_OnFree =  (funcptr) func;
}
//-----------------------------------------------------------------------------
unsigned int JC_Hash::Jhash(const void *key, unsigned  int length, unsigned int initval)
{
        unsigned int  a, b, c, len;
        const unsigned char *k = (const unsigned char *)key;

        len = length;
        a = b = JHASH_GOLDEN_RATIO;
        c = initval;

        while (len >= 12) {
                a += (k[0] +((unsigned int )k[1]<<8) +((unsigned int )k[2]<<16) +((unsigned int )k[3]<<24));
                b += (k[4] +((unsigned int )k[5]<<8) +((unsigned int )k[6]<<16) +((unsigned int )k[7]<<24));
                c += (k[8] +((unsigned int )k[9]<<8) +((unsigned int )k[10]<<16)+((unsigned int )k[11]<<24));

                __jhash_mix(a,b,c);

                k += 12;
                len -= 12;
        }

        c += length;
        switch (len) {
        case 11: c += ((unsigned int )k[10]<<24);
        case 10: c += ((unsigned int )k[9]<<16);
        case 9 : c += ((unsigned int )k[8]<<8);
        case 8 : b += ((unsigned int )k[7]<<24);
        case 7 : b += ((unsigned int )k[6]<<16);
        case 6 : b += ((unsigned int )k[5]<<8);
        case 5 : b += k[4];
        case 4 : a += ((unsigned int )k[3]<<24);
        case 3 : a += ((unsigned int )k[2]<<16);
        case 2 : a += ((unsigned int )k[1]<<8);
        case 1 : a += k[0];
        };

        __jhash_mix(a,b,c);
        return c;
}
