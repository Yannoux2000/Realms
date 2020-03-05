#pragma once
#include "lua.h"

#include "../ECS/Entity.h"
#include "../ECS/EntityManager.h"
#include "../../Base/IBase.h"

#include <sstream>

namespace rlms {
	class Bindings_Entity {
	public:
		static int jsp (lua_State* L) {
			return 0;
		}

		static int Bind_New (lua_State* L) {
			ENTITY_ID e_id = EntityManager::Create ();
			Entity* e = EntityManager::Get (e_id);
			ConstructLuaInstance (L, e);
			return 1;
		}

		static int Bind_ToString (lua_State* L) {
			assert (lua_istable (L, -1));

			std::ostringstream ss("");

			lua_pushstring (L, "_ref");
			lua_rawget (L, -2);
			IBase* b = static_cast<IBase*>(lua_touserdata (L, -1));
			if (IBase::is<Entity*> (b)) {
				Entity* e = IBase::to<Entity*> (b);

				//TO STRING
				ss << "{";

				ss << "} setmetatable()";

			} else {
				ss << "nil";
			}

			lua_pushstring (L, ss.str().c_str());
			return 1;
		}

		static int Bind_Delete (lua_State* L) {
			assert (lua_istable (L, -1));

			lua_pushstring (L, "_ref");
			lua_rawget (L, -2);
			Entity* e = static_cast<Entity*>(lua_touserdata (L, -1));



			return 0;
		}

		static int Bind_Add (lua_State* L) {
			assert (lua_istable (L, -2));
			assert (lua_istable (L, -1));

			return 0;
		}

		static int Bind_Has (lua_State* L) {
			assert (lua_istable (L, -2));
			assert (lua_isstring (L, -1));

			return 0;
		}

		static int Bind_Get (lua_State* L) {
			assert (lua_istable (L, -2));
			assert (lua_isstring (L, -1));

			return 0;
		}

		static int Bind_Rem (lua_State* L) {
			assert (lua_istable (L, -2));
			assert (lua_isstring (L, -1) || lua_istable (L, -1));

			return 0;
		}

		static int Bind_Index (lua_State* L) {
			assert (lua_istable (L, -2));
			assert (lua_isstring (L, -1));
			const char* index = lua_tostring (L, -1);

			lua_pushstring (L, "_ref");
			lua_rawget (L, -3);
			Entity* e = static_cast<Entity*>(lua_touserdata (L, -1));
			if (strcmp (index, "id") == 0) {
				lua_pushnumber (L, e->id());
				return 1;
			}
			if (strcmp (index, "type") == 0) {
				lua_pushstring (L, e->getType ().c_str ());
				return 1;
			}

			/* ADD COMPONENT NAME RECOGNISION HERE */

			lua_getglobal (L, "Entity");
			lua_pushstring (L, index);
			lua_rawget (L, -2);
			return 1;
		}

		static int Bind_NewIndex (lua_State* L) {
			assert (lua_istable (L, -3));
			assert (lua_isstring (L, -2));
			// -1 is the value to be set
			const char* index = lua_tostring (L, -2);

			lua_pushstring (L, "_ref");
			lua_rawget (L, -4);
			IBase* b = static_cast<IBase*>(lua_touserdata (L, -1));
			if(IBase::is<Entity*>(b)){
				Entity* e = IBase::to<Entity*> (b);

				if (strncmp (index, "type", 4) == 0) {
					e->setType(lua_tostring (L, -3));
					return 0;
				}
			/* ADD COMPONENT ASSIGNATION HERE */

			}

			return 0;
		}

		//Entity class
		//lua does not create metatables for light userdata, so i had to improvise
		static inline void ConstructLuaInstance (lua_State*& L, Entity* e) {
			lua_newtable (L);
			lua_pushlightuserdata (L, e);
			lua_setfield (L, -2, "_ref");

			luaL_getmetatable (L, "EntityMetaTable");
			lua_setmetatable (L, -2);
		}

		static void Bind (lua_State* L) {
			lua_newtable (L);
			int EntityIdx = lua_gettop (L);
			lua_pushvalue (L, EntityIdx);
			lua_setglobal (L, "Entity");

			lua_pushcfunction (L, Bind_New);
			lua_setfield (L, -2, "new");

			lua_pushcfunction (L, Bind_ToString);
			lua_setfield (L, -2, "tostring");

			lua_pushcfunction (L, Bind_Delete);
			lua_setfield (L, -2, "delete");



			luaL_newmetatable (L, "EntityMetaTable");
			lua_pushcfunction (L, Bind_Index);
			lua_setfield (L, -2, "__index");

			lua_pushcfunction (L, Bind_NewIndex);
			lua_setfield (L, -2, "__newindex");

			lua_pushcfunction (L, Bind_ToString);
			lua_setfield (L, -2, "__tostring");
		}
	};
}