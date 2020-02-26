#include "pch.h"
#include "pch_allocators.h"

#include "Base/Allocators/PoolAllocator.h"
#include "Base/Allocators/PoolAllocator.cpp"

class TestPoolAllocator : public ::testing::Test {
protected:

	class data_obj {
	public:
		unsigned long long data = 32Ui64;
		unsigned long long data_the_return = 32Ui64;
	};

	static constexpr unsigned long long data_obj_size = sizeof (data_obj);

	static constexpr size_t large_size = data_obj_size * 4;
	static void* large_mem;

	static constexpr size_t small_size = data_obj_size;
	static void* small_mem;

	virtual void SetUp () {
		small_mem = malloc (small_size);
		large_mem = malloc (large_size);
	}

	virtual void TearDown () {
		free (small_mem);
		free (large_mem);
	}
};

void* TestPoolAllocator::large_mem;
void* TestPoolAllocator::small_mem;

TEST_F (TestPoolAllocator, Proxy_getStart) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, small_mem);

	PoolAllocator alloc (sizeof(data_obj), alignof(data_obj), small_mem, small_size);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (small_mem, alloc.getStart ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_EQ (small_mem, alloc.getStart ());
	}

	allocator::deallocateDelete<data_obj> (alloc, a);
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (small_mem, alloc.getStart ());
	}
}

TEST_F (TestPoolAllocator, Proxy_getSize) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, small_mem);

	PoolAllocator alloc (sizeof (data_obj), alignof(data_obj), small_mem, small_size);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (small_size, alloc.getSize ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_EQ (small_size, alloc.getSize ());
	}

	allocator::deallocateDelete<data_obj> (alloc, a);
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (small_size, alloc.getSize ());
	}
}

TEST_F (TestPoolAllocator, Proxy_getNumAllocations) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, small_mem);

	PoolAllocator alloc (sizeof (data_obj), alignof(data_obj), small_mem, small_size);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (0, alloc.getNumAllocations ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_EQ (1, alloc.getNumAllocations ());
	}

	allocator::deallocateDelete<data_obj> (alloc, a);
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (0, alloc.getNumAllocations ());
	}
}

TEST_F (TestPoolAllocator, Proxy_getUsedMemory) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, small_mem);

	PoolAllocator alloc (sizeof (data_obj), alignof(data_obj), small_mem, small_size);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (0, alloc.getUsedMemory ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_GE (data_obj_size, alloc.getUsedMemory ());
	}

	allocator::deallocateDelete<data_obj> (alloc, a);
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (0, alloc.getUsedMemory ());
	}
}

TEST_F (TestPoolAllocator, Proxy_data_obj) {
	data_obj* a = nullptr;

	ASSERT_NE (nullptr, small_mem);
	ASSERT_EQ (nullptr, a);

	PoolAllocator alloc (sizeof (data_obj), alignof(data_obj), small_mem, small_size);
	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_NE (nullptr, a);
	}

	allocator::deallocateDelete<data_obj> (alloc, a);
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (nullptr, a);
	}
}

TEST_F (TestPoolAllocator, Proxy_data_objs) {
	data_obj* a, * b, * c, * d;
	a = b = c = d = nullptr;

	ASSERT_NE (nullptr, large_mem);
	ASSERT_EQ (nullptr, a);
	ASSERT_EQ (nullptr, b);
	ASSERT_EQ (nullptr, c);
	ASSERT_EQ (nullptr, d);

	PoolAllocator alloc (sizeof (data_obj), alignof(data_obj), small_mem, small_size);

	c = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_NE (nullptr, c);
	}

	d = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_NE (nullptr, d);
	}

	for (int i = 0; i < 200; i++) {
		a = allocator::allocateNew<data_obj> (alloc);
		{
			SCOPED_TRACE ("Alloc");
			ASSERT_NE (nullptr, a);
		}

		allocator::deallocateDelete<data_obj> (alloc, c);
		{
			SCOPED_TRACE ("Dealloc");
			ASSERT_EQ (nullptr, c);
		}

		b = allocator::allocateNew<data_obj> (alloc);
		{
			SCOPED_TRACE ("Alloc");
			ASSERT_NE (nullptr, b);
		}

		allocator::deallocateDelete<data_obj> (alloc, b);
		{
			SCOPED_TRACE ("Dealloc");
			ASSERT_EQ (nullptr, b);
		}

		allocator::deallocateDelete<data_obj> (alloc, d);
		{
			SCOPED_TRACE ("Dealloc");
			ASSERT_EQ (nullptr, d);
		}

		c = allocator::allocateNew<data_obj> (alloc);
		{
			SCOPED_TRACE ("Alloc");
			ASSERT_NE (nullptr, c);
		}

		d = allocator::allocateNew<data_obj> (alloc);
		{
			SCOPED_TRACE ("Alloc");
			ASSERT_NE (nullptr, d);
		}

		allocator::deallocateDelete<data_obj> (alloc, a);
		{
			SCOPED_TRACE ("Dealloc");
			ASSERT_EQ (nullptr, a);
		}
	}
}
