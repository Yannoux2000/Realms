#include "pch.h"
#include "pch_loggers.h"

#include "Base/Allocators/MasqueradeAllocator.h"
#include "Module/LuaBindings/lua.h"
#include "Module/LuaBindings/Bindings_ComponentManager.h"
#include "Module/LuaBindings/Bindings_ComponentManager.cpp"

class TestBindingsComponentManager : public ::testing::Test {
protected:
	static constexpr size_t size_mem = 4096;
	static void* mem_ptr;
	static lua_State* L;

	virtual void SetUp () {
		mem_ptr = malloc (size_mem);
		rlms::MasqueradeAllocator alloc (size_mem, mem_ptr);
		rlms::ComponentManager::Initialize (&alloc, size_mem);
		L = luaL_newstate ();
	}

	virtual void TearDown () {
		lua_close (L);
		rlms::ComponentManager::Terminate ();
		free (mem_ptr);
	}

	void test_lua_dostring (const char* string) {
		int res = luaL_dostring (L, string);
		if (res != LUA_OK) {
			ASSERT_NO_THROW (
				throw std::exception (lua_tostring (L, -1));
			);
		}
	}
};

void* TestBindingsComponentManager::mem_ptr;
lua_State* TestBindingsComponentManager::L;

TEST_F (TestBindingsComponentManager, proxy_test) {
	ASSERT_TRUE (true);
}
