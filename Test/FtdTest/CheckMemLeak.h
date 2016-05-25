#pragma once
#ifdef CRTDBG
// boost�����õ�malloc��crt�ĺ궨���ͻ�������_CRTDBG_MAP_ALLOC���������
//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
class CrtSetDbgFlag
{
public:
	CrtSetDbgFlag(int flag = _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
	{
		_CrtSetDbgFlag(flag);
		//_CrtSetBreakAlloc(650);
		/*long bpGrp[] = { };
		for (size_t i = 0; i < _ARRAYSIZE(bpGrp); i++)
		{
		_CrtSetBreakAlloc(bpGrp[i]);
		}*/
	}

	~CrtSetDbgFlag()
	{
		_CrtDumpMemoryLeaks();
	}

private:
};
#endif
