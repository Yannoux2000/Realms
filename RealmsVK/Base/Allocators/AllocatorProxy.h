#pragma once
#include "../../_Preprocess.h"

#ifndef RLMS_DEBUG
	#define BlockAllocatorProxy BlockAllocator
	//#define ProxyAllocator Allocator
#else
	namespace rlms
	{
		class BlockAllocatorProxy;
		class ProxyAllocator;
	};
#endif