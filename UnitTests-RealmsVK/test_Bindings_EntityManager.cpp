#include "pch.h"
#include "pch_loggers.h"

#include "Base/Allocators/MasqueradeAllocator.h"
#include "Module/LuaBindings/lua.h"
#include "Module/LuaBindings/Bindings_EntityManager.h"
#include "Module/LuaBindings/Bindings_EntityManager.cpp"

class TestBindingsEntityManager : public ::testing::Test {
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
			ASSERT_NO_THROW (
				throw std::exception (lua_tostring (L, -1));
			);
		}
	}
};

void* TestBindingsEntityManager::mem_ptr;
lua_State* TestBindingsEntityManager::L;

TEST_F (TestBindingsEntityManager, proxy_test) {
	ASSERT_TRUE (true);
}

TEST_F (TestBindingsEntityManager, BindingTest) {
	ASSERT_NO_THROW (
		rlms::Bindings_EntityManager::Bind (L);
	);
	test_lua_dostring ("");
}

TEST_F (TestBindingsEntityManager, isValid) {

	ASSERT_NO_THROW (
		rlms::Bindings_EntityManager::Bind (L);
	);

	{
		SCOPED_TRACE ("Null ID");
		test_lua_dostring ("ret = EntityManager.isValid (0)");
		lua_getglobal (L, "ret");
		ASSERT_FALSE (lua_toboolean (L, -1));
	}

	{
		SCOPED_TRACE ("Nominal_1");
		test_lua_dostring ("ret = EntityManager.isValid (45)");
		lua_getglobal (L, "ret");
		ASSERT_TRUE (lua_toboolean (L, -1));
	}

	{
		SCOPED_TRACE ("Nominal_Max");
		test_lua_dostring ("ret = EntityManager.isValid (65525)");
		lua_getglobal (L, "ret");
		ASSERT_TRUE (lua_toboolean (L, -1));
	}
}

//static const ENTITY_ID Create ();
TEST_F (TestBindingsEntityManager, CreateEntity) {
	rlms::ENTITY_ID e_id1 = rlms::Entity::NULL_ID;
	rlms::ENTITY_ID e_id2 = rlms::Entity::NULL_ID;

	rlms::Bindings_EntityManager::Bind (L);

	{
		SCOPED_TRACE ("Nominal_1");

		test_lua_dostring ("ret = EntityManager.Create ()");
		lua_getglobal (L, "ret");
		e_id1 = lua_tointeger (L, -1);

		ASSERT_NE (e_id1, rlms::Entity::NULL_ID);
		ASSERT_TRUE (rlms::EntityManager::isValid (e_id1));
		//ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}

	{
		SCOPED_TRACE ("Nominal_2");

		test_lua_dostring ("ret = EntityManager.Create ()");
		lua_getglobal (L, "ret");
		e_id2 = lua_tointeger (L, -1);

		ASSERT_NE (e_id2, rlms::Entity::NULL_ID);
		ASSERT_NE (e_id2, e_id1);
		ASSERT_TRUE (rlms::EntityManager::isValid (e_id2));
		//ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}
}

////////////////////////////////////////
// CASE FOR MEMORY OVERFLOW
////////////////////////////////////////

//static const ENTITY_ID Create (ENTITY_ID id);
TEST_F (TestBindingsEntityManager, CreateEntityWithID) {
	rlms::ENTITY_ID e_id_asked = 40;
	rlms::ENTITY_ID e_id_created = rlms::Entity::NULL_ID;

	rlms::Bindings_EntityManager::Bind (L);

	{
		SCOPED_TRACE ("Nominal");
		lua_pushinteger (L, e_id_asked);
		lua_setglobal (L, "e_id");

		test_lua_dostring ("ret = EntityManager.Create (e_id)");

		lua_getglobal (L, "ret");
		e_id_created = lua_tointeger (L, -1);

		ASSERT_NE (e_id_created, rlms::Entity::NULL_ID);
		ASSERT_EQ (e_id_created, e_id_asked);
		ASSERT_TRUE (rlms::EntityManager::isValid (e_id_created));
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}

	{
		SCOPED_TRACE ("Override");
		lua_pushinteger (L, e_id_asked);
		lua_setglobal (L, "e_id");

		test_lua_dostring ("ret = EntityManager.Create (e_id)");

		lua_getglobal (L, "ret");
		e_id_created = lua_tointeger (L, -1);

		ASSERT_EQ (e_id_created, rlms::Entity::NULL_ID);
		ASSERT_NE (e_id_created, e_id_asked);
		ASSERT_FALSE (rlms::EntityManager::isValid (e_id_created));
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}

	{
		SCOPED_TRACE ("Invalid ID");
		lua_pushinteger (L, rlms::Entity::NULL_ID);
		lua_setglobal (L, "e_id");

		test_lua_dostring ("ret = EntityManager.Create (e_id)");

		lua_getglobal (L, "ret");
		e_id_created = lua_tointeger (L, -1);

		ASSERT_EQ (e_id_created, rlms::Entity::NULL_ID);
		ASSERT_NE (e_id_created, e_id_asked);
		ASSERT_FALSE (rlms::EntityManager::isValid (e_id_created));
		ASSERT_EQ (rlms::EntityManager::n_errors, 2);

	}
}

//static bool Has (ENTITY_ID id);
TEST_F (TestBindingsEntityManager, HasEntity) {
	rlms::ENTITY_ID e_id1 = 600;
	bool e = true;

	rlms::Bindings_EntityManager::Bind (L);

	{
		SCOPED_TRACE ("PreConstruction");
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("ret = EntityManager.Has (e_id)");
		lua_getglobal (L, "ret");
		ASSERT_FALSE (lua_toboolean(L, -1));
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}

	e_id1 = rlms::EntityManager::Create (e_id1);

	{
		SCOPED_TRACE ("Nominal");
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("ret = EntityManager.Has (e_id)");
		lua_getglobal (L, "ret");
		ASSERT_TRUE (lua_toboolean (L, -1));
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}

	rlms::EntityManager::Destroy (e_id1);

	{
		SCOPED_TRACE ("PostDestruction");
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("ret = EntityManager.Has (e_id)");
		lua_getglobal (L, "ret");
		ASSERT_FALSE (lua_toboolean (L, -1));
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
	}

	{
		SCOPED_TRACE ("Invalid ID");
		lua_pushinteger (L, rlms::Entity::NULL_ID);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("ret = EntityManager.Has (e_id)");
		lua_getglobal (L, "ret");
		ASSERT_FALSE (lua_toboolean (L, -1));
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}
}

TEST_F (TestBindingsEntityManager, SampleTest) {
	lua_pushlightuserdata (L, nullptr);
	lua_setglobal (L, "ref");
	lua_getglobal (L, "ref");
	ASSERT_EQ (lua_touserdata (L, -1), nullptr);

	rlms::Entity *e = new rlms::Entity (1);

	lua_pushlightuserdata (L, e);
	lua_setglobal (L, "ref");
	lua_getglobal (L, "ref");
	ASSERT_TRUE (lua_islightuserdata(L, -1));
	ASSERT_EQ (lua_touserdata (L, -1), e);

	lua_newtable (L);
	lua_pushinteger (L, 20);
	lua_setfield (L, -2, "int");
	lua_setglobal (L, "tab");

	lua_getglobal (L, "tab");
	ASSERT_TRUE (lua_istable (L, -1));
	lua_pushstring (L, "int");
	lua_rawget (L, -2);
	ASSERT_TRUE (lua_isinteger (L, -1));
	ASSERT_EQ (lua_tointeger (L, -1), 20);

	lua_pushinteger (L, 10);
	lua_pushinteger (L, 25);
	ASSERT_EQ (lua_tointeger (L, -1), 25);
	lua_remove (L, -1);
	ASSERT_EQ (lua_tointeger (L, -1), 10);


}

//static Entity* Get (ENTITY_ID id);
TEST_F (TestBindingsEntityManager, GetEntity) {
	rlms::ENTITY_ID e_id1 = 40;
	rlms::Entity* e = nullptr;

	rlms::Bindings_EntityManager::Bind (L);

	{
		SCOPED_TRACE ("PreConstruction");
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("ret = EntityManager.Get (e_id)");
		lua_getglobal (L, "ret");
		lua_pushstring (L, "_ref");
		lua_rawget (L, -2);
		ASSERT_EQ (lua_touserdata (L, -1), nullptr);
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}

	e_id1 = rlms::EntityManager::Create (e_id1);

	{
		SCOPED_TRACE ("Nominal_1");
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("ret = EntityManager.Get (e_id)");
		lua_getglobal (L, "ret");
		ASSERT_TRUE (lua_istable (L, -1));
		lua_getglobal (L, "ret");
		lua_pushstring (L, "_ref");
		lua_rawget (L, -2);
		ASSERT_TRUE (lua_islightuserdata (L, -1));
		e = static_cast<rlms::Entity*>(lua_touserdata (L, -1));
		ASSERT_NE (e, nullptr);
		ASSERT_NO_THROW (
			ASSERT_EQ (e->id (), e_id1);
		);
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}

	{
		SCOPED_TRACE ("Nominal_2");
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring (R"(
			e_id = EntityManager.Create()
			ret = EntityManager.Get (e_id)
		)");
		lua_getglobal (L, "ret");
		lua_pushstring (L, "_ref");
		lua_rawget (L, -2);
		ASSERT_NE (lua_touserdata (L, -1), nullptr);
		ASSERT_NO_THROW (
			ASSERT_EQ (e->id (), e_id1);
		);
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}

	rlms::EntityManager::Destroy (e_id1);

	{
		SCOPED_TRACE ("PostDestruction");
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("ret = EntityManager.Get (e_id)");
		lua_getglobal (L, "ret");
		lua_pushstring (L, "_ref");
		lua_rawget (L, -2);
		ASSERT_EQ (lua_touserdata (L, -1), nullptr);
		ASSERT_EQ (rlms::EntityManager::n_errors, 2);
	}

	{
		SCOPED_TRACE ("Invalid ID");
		lua_pushinteger (L, rlms::Entity::NULL_ID);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("ret = EntityManager.Get (e_id)");
		lua_getglobal (L, "ret");
		lua_pushstring (L, "_ref");
		lua_rawget (L, -2);
		ASSERT_EQ (lua_touserdata (L, -1), nullptr);
		ASSERT_EQ (rlms::EntityManager::n_errors, 3);
	}
}

//static void Destroy (ENTITY_ID id);
TEST_F (TestBindingsEntityManager, DestroyEntity) {
	rlms::ENTITY_ID e_id1 = 40;
	rlms::Entity* e = nullptr;

	rlms::Bindings_EntityManager::Bind (L);

	{
		SCOPED_TRACE ("PreConstruction");
		ASSERT_EQ (rlms::EntityManager::n_errors, 0);
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("EntityManager.Destroy (e_id)");
		ASSERT_EQ (rlms::EntityManager::n_errors, 1);
	}

	e_id1 = rlms::EntityManager::Create (e_id1);
	e = rlms::EntityManager::Get (e_id1);

	{
		SCOPED_TRACE ("Nominal");
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("EntityManager.Destroy (e_id)");
	}

	{
		SCOPED_TRACE ("PostDestruction");
		lua_pushinteger (L, e_id1);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("EntityManager.Destroy (e_id)");
		ASSERT_EQ (rlms::EntityManager::n_errors, 2);
	}

	{
		SCOPED_TRACE ("Invalid ID");
		lua_pushinteger (L, rlms::Entity::NULL_ID);
		lua_setglobal (L, "e_id");
		test_lua_dostring ("EntityManager.Destroy (e_id)");
		ASSERT_EQ (rlms::EntityManager::n_errors, 3);
	}
}
