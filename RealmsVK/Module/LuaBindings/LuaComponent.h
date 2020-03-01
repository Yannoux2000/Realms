#pragma once
#include "../ECS/IComponent.h"
#include "LuaComponentPrototype.h"

namespace rlms {
	class LuaComponent : public IComponent {
	private:
		LuaComponentPrototype& prototype;
		//member pointer is this + member

	public:
		LuaComponent(LuaComponentPrototype& proto, ENTITY_ID const& entity_id, COMPONENT_ID const& component_id) : IComponent(entity_id, component_id), prototype(proto) {}
		LuaComponentPrototype& getPrototype () {
			return prototype;
		}
	};
}
