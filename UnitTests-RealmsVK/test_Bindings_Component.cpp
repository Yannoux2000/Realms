#include "pch.h"
#include "pch_loggers.h"

#include "Base/Allocators/MasqueradeAllocator.h"
#include "Module/LuaBindings/lua.h"
#include "Module/LuaBindings/Bindings_ComponentManager.h"
#include "Module/LuaBindings/Bindings_Component.h"
#include "Module/LuaBindings/Bindings_Component.cpp"
#include "Module/ECS/IComponent.h"

class TestBindingsComponent : public ::testing::Test {
protected:
	static constexpr size_t size_mem = 4096;
	static void* mem_ptr;
	static lua_State* L;

	virtual void SetUp () {
		mem_ptr = malloc (size_mem);
		rlms::MasqueradeAllocator alloc (size_mem, mem_ptr);
		rlms::EntityManager::Initialize (&alloc, size_mem);
		L = luaL_newstate ();
	}

	virtual void TearDown () {
		lua_close (L);
		rlms::EntityManager::Terminate ();
		free (mem_ptr);
	}

	void test_lua_dostring (const char* string) {
		int res = luaL_dostring (L, string);
		if (res != LUA_OK) {
			throw std::exception (lua_tostring (L, -1));
		}

	}
};

lua_State* TestBindingsComponent::L;
void* TestBindingsComponent::mem_ptr;

using namespace rlms;

TEST_F (TestBindingsComponent, proxy_test) {
	ASSERT_TRUE (true);
}
