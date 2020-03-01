#pragma once
#include "lua.h"
#include "../ECS/EntityManager.h"

#include "Bindings_Component.h"

namespace rlms {
	class Bindings_EntityManager {
	public:
		//static int rlms::EntityManager::n_errors ;
		static int Bind_GetNErrors (lua_State* L);
		static int Bind_ResetNErrors (lua_State* L);
		static int Bind_NErrors (lua_State* L);

		//template<class C> const COMPONENT_ID createComponent ();
		//template<class C> const COMPONENT_ID createComponent (Entity* entity);
		//template<class C> const COMPONENT_ID createComponent (Entity* entity, COMPONENT_ID c_id);
		//template<class C> const COMPONENT_ID createComponent (COMPONENT_ID c_id);

		//const bool hasEntity (COMPONENT_ID c_id);
		//template<class C> const bool hasComponent (Entity* entity);
		//template<class C> const bool hasComponent (COMPONENT_ID c_id);
		//const bool hasComponent (COMPONENT_ID c_id);

		//const ENTITY_ID& getEntity (COMPONENT_ID const& c_id);
		//template<class C> C* getComponent (Entity* entity);
		//template<class C> C* getComponent (COMPONENT_ID const& c_id);
		//template<class C> static std::vector<C*> getComponents ();
		//IComponent* getComponent (COMPONENT_ID const& c_id);

		//template<class C> void destroyComponent (Entity* entity);
		//void destroyComponent (COMPONENT_ID c_id);

		static int Bind_isValid (lua_State* L);
		
		static int Bind_CreateEntity (lua_State* L);
		static int Bind_GetEntity (lua_State* L);
		static int Bind_HasEntity (lua_State* L);
		static int Bind_DestroyEntity (lua_State* L);

		static void Bind (lua_State* L);
	};
}

