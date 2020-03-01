#pragma once
#include "lua.h"

#include "../ECS/IComponent.h"

namespace rlms {
	class Bindings_Component {
		static int jsp (lua_State* L) {
			return 0;
		}

		static inline void ConstructLuaInstance (lua_State*& L, IComponent* e) {
			lua_newtable (L);
			lua_pushlightuserdata (L, e);
			lua_setfield (L, -2, "_ref");
		}

		static void Bind (lua_State* L) {

		}
	};
}