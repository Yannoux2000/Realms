#pragma once
#include "lua.h"
#include "../ECS/ComponentManager.h"

#include "Bindings_Component.h"

namespace rlms {
	class Bindings_ComponentManager {
	public:
		static int Bind_isValid (lua_State* L);

		static void Bind (lua_State* L);
	};
}
