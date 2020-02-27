#include "BlockAllocatorProxy.h"

using namespace rlms;

#if _DEBUG

BlockAllocatorProxy::BlockAllocatorProxy(BlockAllocator& allocator)
	: ProxyAllocator(allocator), _allocator(allocator)
{
}

BlockAllocatorProxy::~BlockAllocatorProxy()
{
}

size_t BlockAllocatorProxy::getBlockSize() const
{
	return _allocator.getBlockSize();
}

uint8_t BlockAllocatorProxy::getAlignment() const
{
	return _allocator.getAlignment();
}

#endif