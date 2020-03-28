#pragma once

#ifdef _DEBUG
#include <stdlib.h>
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC

#define MemCheck() int tmpFlag = _CrtSetDbgFlag (_CRTDBG_REPORT_FLAG); \
tmpFlag |= _CRTDBG_LEAK_CHECK_DF; \
tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;  \
_CrtSetDbgFlag (tmpFlag);

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type

#else 
#define MemCheck() do{} while(0)
#define DBG_NEW new
#endif // _DEBUG
