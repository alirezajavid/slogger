#include "Common.h"
#include "JC_BufferQueue.h"

JC_BufferQueue::JC_BufferQueue() {
	JP_Buffers = 0;
	JP_Head = 0;
	JP_Tail = -1;
	JP_Max = 0;
}
//-----------------------------------------------------------------------------
bool JC_BufferQueue::Start(int InitSize) {
	JP_Max = InitSize;
	JP_Buffers = (MS_DataProviderBuffer **)JGetMem(JP_Max * sizeof(MS_DataProviderBuffer*));
	if (!JP_Buffers)
	{
		fprintf(stderr, "Could not get memory[WIVX]\n");
		fflush(stderr);
		return false;
	}
	memset(JP_Buffers, 0, JP_Max * sizeof(MS_DataProviderBuffer*));
	return true;
}
//-----------------------------------------------------------------------------
void JC_BufferQueue::End()
{
	while (pop());
	JFreeMem(JP_Buffers, JP_Max * sizeof(MS_DataProviderBuffer*));
	JP_Buffers=0;
}
//-----------------------------------------------------------------------------
MS_DataProviderBuffer * JC_BufferQueue::Get_Tail()
{
	JP_Tail++;
	if (JP_Tail == JP_Max)
		JP_Tail = 0;
	if (JP_Tail == JP_Head && JP_Buffers[JP_Tail])
		return 0;

	MS_DataProviderBuffer * val = (MS_DataProviderBuffer *)JGetMem(sizeof(MS_DataProviderBuffer));
	if (!val)
	{
		fprintf(stderr, "Could not get memory[UKDW]\n");
		fflush(stderr);
		return 0;
	}
	memset(val, 0, sizeof(MS_DataProviderBuffer));
	JP_Buffers[JP_Tail] = val;
	return val;
}
//-----------------------------------------------------------------------------
MS_DataProviderBuffer * JC_BufferQueue::Get_Head()
{
	return JP_Buffers[JP_Head];
}
//-----------------------------------------------------------------------------
bool JC_BufferQueue::pop()
{
	if (JP_Buffers[JP_Head])
	{
		JFreeMem(JP_Buffers[JP_Head], sizeof(MS_DataProviderBuffer));
		JP_Buffers[JP_Head] = 0;
		JP_Head ++;
		JP_Head = (JP_Head == JP_Max)?0:JP_Head;
		return true;
	}
	return false;
}
