#include "Bindings_EntityManager.h"

//static int rlms::EntityManager::n_errors ;

int rlms::Bindings_EntityManager::Bind_GetNErrors (lua_State* L) {
	lua_pushinteger (L, rlms::EntityManager::n_errors);
	return 1;
}

int rlms::Bindings_EntityManager::Bind_ResetNErrors (lua_State* L) {
	rlms::EntityManager::n_errors = 0;
	return 0;
}

int rlms::Bindings_EntityManager::Bind_NErrors (lua_State* L) {
	if (lua_isinteger (L, -1)) {
		rlms::EntityManager::n_errors = static_cast<int>(lua_tointeger (L, -1));
	}
	lua_pushinteger (L, rlms::EntityManager::n_errors);
	return 1;
}

//static bool isValid (ENTITY_ID id);

int rlms::Bindings_EntityManager::Bind_isValid (lua_State* L) {
	rlms::ENTITY_ID e_id = Entity::NULL_ID;

	if (lua_isinteger (L, -1)) {
		e_id = static_cast<rlms::ENTITY_ID>(lua_tointeger (L, -1));
		bool flag = EntityManager::isValid (e_id);
		lua_pushboolean (L, flag);
	} else {
		lua_pushboolean (L, false);
	}
	return 1;
}

//static const ENTITY_ID Create ();
//static const ENTITY_ID Create (ENTITY_ID id);

int rlms::Bindings_EntityManager::Bind_CreateEntity (lua_State* L) {
	int e_id = Entity::NULL_ID;

	if (lua_isinteger (L, -1)) {
		e_id = static_cast<int>(lua_tointeger (L, -1));
		e_id = EntityManager::Create (e_id);
	} else {
		e_id = EntityManager::Create ();
	}

	lua_pushinteger (L, e_id);
	return 1;
}

//static Entity* Get (ENTITY_ID id);

int rlms::Bindings_EntityManager::Bind_GetEntity (lua_State* L) {
	if (lua_isinteger (L, -1)) {
		int e_id = static_cast<int>(lua_tointeger (L, -1));
		Entity* e = EntityManager::Get (e_id);
		Bindings_Entity::ConstructLuaInstance (L, e);
	} else {
		Bindings_Entity::ConstructLuaInstance (L, nullptr);
	}
	return 1;
}

//static bool Has (ENTITY_ID id);

int rlms::Bindings_EntityManager::Bind_HasEntity (lua_State* L) {
	if (lua_isinteger (L, -1)) {
		int e_id = static_cast<int>(lua_tointeger (L, -1));
		bool flag = EntityManager::Has (e_id);
		lua_pushboolean (L, flag);
	} else {
		lua_pushboolean (L, false);
	}
	return 1;
}

//static void Destroy (ENTITY_ID id);

int rlms::Bindings_EntityManager::Bind_DestroyEntity (lua_State* L) {
	if (lua_isinteger (L, -1)) {
		int e_id = static_cast<int>(lua_tointeger (L, -1));
		EntityManager::Destroy (e_id);
	}
	return 0;
}

void rlms::Bindings_EntityManager::Bind (lua_State* L) {
	lua_newtable (L);
	int EntityManagerIdx = lua_gettop (L);
	lua_pushvalue (L, EntityManagerIdx);
	lua_setglobal (L, "EntityManager");

	lua_pushcfunction (L, Bind_NErrors);
	lua_setfield (L, -2, "NError");

	lua_pushcfunction (L, Bind_isValid);
	lua_setfield (L, -2, "isValid");

	lua_pushcfunction (L, Bind_CreateEntity);
	lua_setfield (L, -2, "Create");

	lua_pushcfunction (L, Bind_GetEntity);
	lua_setfield (L, -2, "Get");

	lua_pushcfunction (L, Bind_HasEntity);
	lua_setfield (L, -2, "Has");

	lua_pushcfunction (L, Bind_DestroyEntity);
	lua_setfield (L, -2, "Destroy");
}
