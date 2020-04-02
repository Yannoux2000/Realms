#pragma once
#include "../ECS/IComponent.h"

#include <vector>
#include <map>
#include <sstream>

#include "lua.h"
#include "Attribute.h"

namespace rlms {
	class LuaComponent : public IComponent {
	public:
		LuaComponent(ENTITY_ID const& entity_id, COMPONENT_ID const& component_id, COMPONENT_TYPE_ID const& type_id) : IComponent(entity_id, component_id, type_id) {}
	};

	class LuaComponentPrototype : public IComponentPrototype {
	private:
		std::map<std::string, Attribute> attributes_sub_address;

		void SetAttributeType (lua_State*& L);

	public:
		LuaComponentPrototype (lua_State* L, std::string& component_type);

		void Populate (IComponent* const& c) override;
		void* GetAttrib (IComponent* const& c, std::string& member) override;
		void SetAttrib (IComponent* const c, std::string& member);

		std::string debug ();
		std::string debug (IComponent* const& c);
	};
}
