#pragma once
#include "../ECS/IComponent.h"

namespace rlms {
	class LuaComponent : public IComponent {
	private:
		void* prototype;
		//member pointer is this + member

	public:
		LuaComponent(void* proto, ENTITY_ID const& entity_id, COMPONENT_ID const& component_id) : IComponent(entity_id, component_id), prototype(proto) {}
		void* getPrototype () {
			return prototype;
		}
	};
}
