#include "pch.h"
#include "pch_loggers.h"

#include "Base/Allocators/MasqueradeAllocator.h"
#include "Module/LuaBindings/lua.h"
#include "Module/LuaBindings/Bindings_EntityManager.h"
#include "Module/LuaBindings/Bindings_Entity.h"
#include "Module/LuaBindings/Bindings_Entity.cpp"
#include "Module/ECS/Entity.h"

class TestBindingsEntity : public ::testing::Test {
protected:
	static constexpr size_t size_mem = 4096;
	static void* mem_ptr;
	static lua_State* L;
	static rlms::MasqueradeAllocator* alloc;

	virtual void SetUp () {
		mem_ptr = malloc (size_mem);
		alloc = new rlms::MasqueradeAllocator(size_mem, mem_ptr);
		rlms::EntityManager::Initialize (alloc, size_mem);
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

lua_State* TestBindingsEntity::L;
void* TestBindingsEntity::mem_ptr;
rlms::MasqueradeAllocator* TestBindingsEntity::alloc;

using namespace rlms;

TEST_F (TestBindingsEntity, proxy_test) {
	ASSERT_TRUE (true);
}

TEST_F (TestBindingsEntity, BindingTest) {
	rlms::Bindings_EntityManager::Bind (L);
	ASSERT_NO_THROW (
		rlms::Bindings_Entity::Bind (L);
	);

	test_lua_dostring ("");

	lua_getglobal (L, "Entity");
	ASSERT_TRUE (lua_istable (L, -1));

	lua_getfield (L, -1, "new");
	ASSERT_FALSE (lua_isnil (L, -1));
	ASSERT_TRUE (lua_iscfunction (L, -1));

	lua_getglobal (L, "Entity");
	lua_getfield (L, -1, "tostring");
	ASSERT_FALSE (lua_isnil (L, -1));
	ASSERT_TRUE (lua_iscfunction (L, -1));

	lua_getglobal (L, "Entity");
	lua_getfield (L, -1, "delete");
	ASSERT_FALSE (lua_isnil (L, -1));
	ASSERT_TRUE (lua_iscfunction (L, -1));
}

TEST_F (TestBindingsEntity, Contructor) {

	rlms::Bindings_EntityManager::Bind (L);
	rlms::Bindings_Entity::Bind (L);

	test_lua_dostring ("e = Entity.new()");
}

TEST_F (TestBindingsEntity, Instance) {
	rlms::Bindings_EntityManager::Bind (L);
	rlms::Bindings_Entity::Bind (L);
	ENTITY_ID e_id = EntityManager::Create ();
	Entity* e = EntityManager::Get (e_id);

	rlms::Bindings_Entity::ConstructLuaInstance (L, e);
	lua_setglobal (L, "e");

	test_lua_dostring ("ret = e.id");
	lua_getglobal (L, "ret");
	e = EntityManager::Get (lua_tointeger (L, -1));
	ASSERT_NE (e, nullptr);
	
	test_lua_dostring ("ret = e.type");
	lua_getglobal (L, "ret");
	ASSERT_STREQ (e->getType ().c_str(), lua_tostring (L, -1));
}

TEST_F (TestBindingsEntity, Destructor) {
	Entity* e = new Entity (1);
	ASSERT_NO_THROW (
		delete e;
	);
}

TEST_F (TestBindingsEntity, addComponent) {
	Entity e (1);
	IComponent t_start;
	ASSERT_NO_THROW (
		e.add (&t_start);
	);
}

TEST_F (TestBindingsEntity, hasComponent) {
	Entity e (1);
	IComponent t_start;
	bool ret = false;

	ASSERT_NO_THROW (
		ret = e.has<IComponent> ();
	);
	ASSERT_EQ (ret, false);

	ret = true;
	e.add (&t_start);
	ASSERT_NO_THROW (
		ret = e.has<IComponent> ();
	);

	ASSERT_EQ (ret, true);
}

TEST_F (TestBindingsEntity, getComponent) {
	Entity e (1);
	IComponent* c, t_start;

	ASSERT_NO_THROW (
		c = e.get<IComponent> ();
	);
	ASSERT_EQ (c, nullptr);

	e.add (&t_start);
	ASSERT_NO_THROW (
		c = e.get<IComponent> ();
	);
	ASSERT_EQ (c, &t_start);
}

TEST_F (TestBindingsEntity, remComponent) {
	Entity e (1);
	IComponent t_start;

	ASSERT_NO_THROW (
		e.rem<IComponent> ();
	);

	e.add (&t_start);
	ASSERT_NO_THROW (
		e.rem<IComponent> ();
	);
}

TEST_F (TestBindingsEntity, id) {
	int id = 45;

	Entity e (id);
	ASSERT_EQ (e.id (), id);
}

#include "Module/ECS/TransformComponent.h"

TEST_F (TestBindingsEntity, Destructor2) {
	Entity* e = new Entity (1);
	TransformComponentPrototype transformProto;
	TransformComponent* t_start = transformProto.Create(alloc, e->id(), 5);
	t_start->position = Vec3<double> (5, 3, 1);

	e->add (t_start);
	ASSERT_NO_THROW (
		delete e;
	);

	ASSERT_NE (t_start, nullptr);
	ASSERT_NO_THROW (
		ASSERT_EQ (t_start->position.getX (), 5);
	ASSERT_EQ (t_start->position.getY (), 3);
	ASSERT_EQ (t_start->position.getZ (), 1);
	);

}

TEST_F (TestBindingsEntity, getComponents) {
	int id = 45;

	Entity e (id);
	ASSERT_EQ (e.id (), id);

	IComponent c;
	TransformComponentPrototype transformProto;
	TransformComponent* t_start = transformProto.Create (alloc, e.id (), 5);
	std::vector<IComponent*> v;
	bool ret;

	v = e.getComponents ();

	ASSERT_EQ (v.size (), 0);
	ret = false;
	for (auto it = v.begin (); it != v.end (); it++) {
		if (*it == &c) {
			ret = true;
		}
	}
	ASSERT_FALSE (ret);

	e.add (&c);
	e.add (t_start);

	ASSERT_EQ (e.has<IComponent> (), true);
	ASSERT_EQ (e.has<TransformComponent> (), true);

	ASSERT_NE (e.get<TransformComponent> (), nullptr);
	ASSERT_EQ (e.get<TransformComponent> (), t_start);

	ASSERT_NE (e.get<IComponent> (), nullptr);
	ASSERT_EQ (e.get<IComponent> (), &c);

	v = e.getComponents ();

	ASSERT_EQ (v.size (), 2);
	ret = false;
	for (auto it = v.begin (); it != v.end (); it++) {
		if (*it == &c) {
			ret = true;
		}
	}
	ASSERT_EQ (ret, true);
}

TEST_F (TestBindingsEntity, nominalUseCase) {
	int id = 45;

	Entity e (id);
	ASSERT_EQ (e.id (), id);

	IComponent ca, cb;
	TransformComponentPrototype transformProto;
	TransformComponent* t_start = transformProto.Create (alloc, e.id (), 5);

	ASSERT_EQ (e.has<IComponent> (), false);
	ASSERT_EQ (e.has<TransformComponent> (), false);
	ASSERT_EQ (e.get<IComponent> (), nullptr);
	ASSERT_EQ (e.get<TransformComponent> (), nullptr);

	e.add (&ca);

	ASSERT_EQ (e.has<IComponent> (), true);
	ASSERT_EQ (e.has<TransformComponent> (), false);
	ASSERT_NE (e.get<IComponent> (), nullptr);
	ASSERT_EQ (e.get<IComponent> (), &ca);
	ASSERT_EQ (e.get<TransformComponent> (), nullptr);

	e.add (&cb);

	ASSERT_EQ (e.has<IComponent> (), true);
	ASSERT_EQ (e.has<TransformComponent> (), false);
	ASSERT_NE (e.get<IComponent> (), nullptr);
	ASSERT_EQ (e.get<IComponent> (), &cb);
	ASSERT_EQ (e.get<TransformComponent> (), nullptr);

	e.add (t_start);

	ASSERT_EQ (e.has<IComponent> (), true);
	ASSERT_EQ (e.has<TransformComponent> (), true);
	ASSERT_NE (e.get<TransformComponent> (), nullptr);
	ASSERT_EQ (e.get<TransformComponent> (), t_start);
}
