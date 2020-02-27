#include "pch.h"
#include "pch_allocators.h"
/*
#include "Base/Allocators/LinearAllocator.h"
#include "Base/Allocators/LinearAllocator.cpp"

class TestLinearAllocator : public ::testing::Test {
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

void* TestLinearAllocator::large_mem;
void* TestLinearAllocator::small_mem;


TEST_F (TestLinearAllocator, Proxy_getStart) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, small_mem);

	LinearAllocator alloc (small_size, small_mem);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (small_mem, alloc.getStart ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_EQ (small_mem, alloc.getStart ());
	}

	alloc.clear ();
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (small_mem, alloc.getStart ());
	}
}

TEST_F (TestLinearAllocator, Proxy_getSize) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, small_mem);

	LinearAllocator alloc (small_mem, small_size);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (small_size, alloc.getSize ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_EQ (small_size, alloc.getSize ());
	}

	alloc.clear ();
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (small_size, alloc.getSize ());
	}
}

TEST_F (TestLinearAllocator, Proxy_getNumAllocations) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, small_mem);

	LinearAllocator alloc (small_mem, small_size);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (0, alloc.getNumAllocations ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_EQ (1, alloc.getNumAllocations ());
	}

	alloc.clear ();
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (0, alloc.getNumAllocations ());
	}
}

TEST_F (TestLinearAllocator, Proxy_getUsedMemory) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, small_mem);

	LinearAllocator alloc (small_mem, small_size);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (0, alloc.getUsedMemory ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_GE (data_obj_size, alloc.getUsedMemory ());
	}

	alloc.clear ();
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (0, alloc.getUsedMemory ());
	}
}

TEST_F (TestLinearAllocator, Proxy_data_obj) {
	data_obj* a = nullptr;

	ASSERT_NE (nullptr, small_mem);
	ASSERT_EQ (nullptr, a);

	LinearAllocator alloc (small_mem, small_size);
	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_NE (nullptr, a);
	}

	alloc.clear ();
}

TEST_F (TestLinearAllocator, Proxy_data_objs) {
	data_obj* a,* b,* c,* d;
	a = b = c = d = nullptr;

	ASSERT_NE (nullptr, large_mem);
	ASSERT_EQ (nullptr, a);
	ASSERT_EQ (nullptr, b);
	ASSERT_EQ (nullptr, c);
	ASSERT_EQ (nullptr, d);

	LinearAllocator alloc (large_mem, large_size);
	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_NE (nullptr, a);
	}

	b = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_NE (nullptr, b);
		ASSERT_NE (a, b);
	}

	c = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_NE (nullptr, c);
		ASSERT_NE (a, c);
		ASSERT_NE (b, c);
	}

	d = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_NE (nullptr, d);
		ASSERT_NE (a, d);
		ASSERT_NE (b, d);
		ASSERT_NE (c, d);
	}

	alloc.clear ();
}
*/