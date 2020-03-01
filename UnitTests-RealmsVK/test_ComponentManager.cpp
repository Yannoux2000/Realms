#include "pch.h"
#include "pch_loggers.h"

#include "Base/Allocators/MasqueradeAllocator.h"
#include "Module/ECS/ComponentManager.h"
#include "Module/ECS/ComponentManager.cpp"

class TestComponentManager : public ::testing::Test {
protected:
	static constexpr size_t size_mem = 4096;
	static void* mem_ptr;

	virtual void SetUp () {
		mem_ptr = malloc (size_mem);
		rlms::MasqueradeAllocator alloc (size_mem, mem_ptr);
		rlms::ComponentManager::Initialize (&alloc, size_mem);
	}

	virtual void TearDown () {
		rlms::ComponentManager::Terminate ();
		free (mem_ptr);
	}
};

void* TestComponentManager::mem_ptr;

TEST_F (TestComponentManager, proxy_test) {
	ASSERT_TRUE (true);
}

