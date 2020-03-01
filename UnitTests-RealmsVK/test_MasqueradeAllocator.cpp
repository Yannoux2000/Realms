#include "pch.h"
#include "pch_allocators.h"

#include "Base/Allocators/MasqueradeAllocator.h"
#include "Base/Allocators/MasqueradeAllocator.cpp"

class TestMasqueradeAllocator : public ::testing::Test {
protected:

	class data_obj {
	public:
		std::string data = "wow";
	};

	static constexpr unsigned long long data_obj_size = sizeof (data_obj);

	static constexpr size_t large_size = data_obj_size * 8;
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

void* TestMasqueradeAllocator::large_mem;
void* TestMasqueradeAllocator::small_mem;

TEST_F (TestMasqueradeAllocator, proxy_test) {
	ASSERT_TRUE (true);
}

TEST_F (TestMasqueradeAllocator, Proxy_init) {
	MasqueradeAllocator alloc (small_size, small_mem);
	ASSERT_TRUE (true);
}

TEST_F (TestMasqueradeAllocator, Proxy_alloc_desalloc) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, large_mem);
	MasqueradeAllocator alloc (large_size, large_mem);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (large_size, alloc.getSize ());
		ASSERT_EQ (0, alloc.getNumAllocations ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_EQ (large_size, alloc.getSize ());
		ASSERT_NE (0, alloc.getNumAllocations ());
	}

	allocator::deallocateDelete<data_obj> (alloc, a);

	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (large_size, alloc.getSize ());
		//ASSERT_EQ (0, alloc.getNumAllocations()); //does not dealloc on the go
	}
}

TEST_F (TestMasqueradeAllocator, Proxy_getSize) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, large_mem);

	MasqueradeAllocator alloc (large_size, large_mem);
	{
		SCOPED_TRACE ("Init");
		ASSERT_EQ (large_size, alloc.getSize ());
	}

	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_EQ (large_size, alloc.getSize ());
	}

	allocator::deallocateDelete<data_obj> (alloc, a);
	{
		SCOPED_TRACE ("Dealloc");
		ASSERT_EQ (large_size, alloc.getSize ());
	}
}

TEST_F (TestMasqueradeAllocator, Proxy_getNumAllocations) {
	data_obj* a = nullptr;
	ASSERT_NE (nullptr, large_mem);

	MasqueradeAllocator alloc (large_size, large_mem);
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
		//ASSERT_EQ (0, alloc.getNumAllocations()); //does not dealloc on the go
	}
}

TEST_F (TestMasqueradeAllocator, Proxy_data_obj) {
	data_obj* a = nullptr;

	ASSERT_NE (nullptr, large_mem);
	ASSERT_EQ (nullptr, a);

	MasqueradeAllocator alloc (large_size, large_mem);
	a = allocator::allocateNew<data_obj> (alloc);
	{
		SCOPED_TRACE ("Alloc");
		ASSERT_NE (nullptr, a);
	}

	allocator::deallocateDelete<data_obj> (alloc, a);
	{
		SCOPED_TRACE ("Dealloc");
	}
}

TEST_F (TestMasqueradeAllocator, Proxy_data_objs) {
	data_obj* a, * b, * c, * d;
	a = b = c = d = nullptr;

	ASSERT_NE (nullptr, large_mem);
	ASSERT_EQ (nullptr, a);
	ASSERT_EQ (nullptr, b);
	ASSERT_EQ (nullptr, c);
	ASSERT_EQ (nullptr, d);

	MasqueradeAllocator alloc (large_size, large_mem);

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
		}

		b = allocator::allocateNew<data_obj> (alloc);
		{
			SCOPED_TRACE ("Alloc");
			ASSERT_NE (nullptr, b);
		}

		allocator::deallocateDelete<data_obj> (alloc, b);
		{
			SCOPED_TRACE ("Dealloc");
		}

		allocator::deallocateDelete<data_obj> (alloc, d);
		{
			SCOPED_TRACE ("Dealloc");
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
		}
	}
}
