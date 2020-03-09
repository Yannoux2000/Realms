#include "pch.h"
#include "pch_loggers.h"

#include "Base/Allocators/MasqueradeAllocator.h"
#include "Module/ECS/ECS_Core.h"
#include "Module/ECS/ECS_Core.cpp"

class TestECSManager : public ::testing::Test {
protected:
	static constexpr size_t size_mem = 4096;
	static void* mem_ptr;

	virtual void SetUp () {
		mem_ptr = malloc (size_mem);
		rlms::MasqueradeAllocator alloc (size_mem, mem_ptr);
		rlms::ECS_Core::Initialize (&alloc, size_mem);
	}

	virtual void TearDown () {
		rlms::ECS_Core::Terminate ();
		free (mem_ptr);
	}
};

void* TestECSManager::mem_ptr;


TEST_F (TestECSManager, proxy_test) {
	ASSERT_TRUE (true);
}
