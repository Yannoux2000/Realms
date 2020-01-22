#pragma once
#include "_Preprocess.h"

#ifdef RLMS_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define MemCheck() int tmpFlag = _CrtSetDbgFlag (_CRTDBG_REPORT_FLAG); \
tmpFlag |= _CRTDBG_LEAK_CHECK_DF; \
_CrtSetDbgFlag (tmpFlag);
#else 
#define MemCheck() do{} while(0)
#endif // RLMS_DEBUG

