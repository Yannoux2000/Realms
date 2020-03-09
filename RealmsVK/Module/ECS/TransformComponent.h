#pragma once
#include "IComponent.h"

#include <string>
#include <regex>

#include "../../Base/Math/Vec3.h"

namespace rlms {
	struct TransformComponent : public IComponent {
		Vec3<double> position;
		TransformComponent (ENTITY_ID const& entity_id, COMPONENT_ID const& component_id) : position(), IComponent(entity_id, component_id) {}
	};

	class TransformComponentPrototype : public IComponentPrototype {
	public:
		TransformComponentPrototype () : IComponentPrototype ("transform", sizeof (TransformComponent), alignof(TransformComponent)) {}
		~TransformComponentPrototype() {}

		TransformComponent* Create (Allocator* const& alloc, ENTITY_ID const& entity_id, COMPONENT_ID const& component_id) override {
			return new (alloc->allocate (_size, _align)) TransformComponent (entity_id, component_id);
		}

		void* Get (IComponent* const c, std::string&& member) override {
			if (IBase::is<TransformComponent> (c)) {
				if (std::regex_match (member, std::regex ("[(vec)(vector)]"))) {
					return &(IBase::to<TransformComponent> (c)->position);
				}
			}

			return _Get (c, member);
		}
	};
}