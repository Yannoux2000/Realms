#pragma once
#include "IComponent.h"

#include <string>

namespace rlms {
	class TransformComponent : public IComponent {
	public:
		std::string phrase = "wow";
		TransformComponent (ENTITY_ID entity_id, COMPONENT_ID const& component_id) : IComponent (entity_id, component_id) {}
	};
}